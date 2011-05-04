/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
/** \file
 \brief Perform PCA.
 
 \verbinclude pca.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <itksys/SystemTools.hxx>
#include <sstream>
#include "itkPCAImageToImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( componentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > OutputImageType; \
  function< OutputImageType >( inputFileNames, outputDirectory, numberOfPCs ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare PerformPCA. */
template< class OutputImageType >
void PerformPCA(
  const std::vector< std::string > & inputFileNames,
  const std::string & outputDirectory,
  unsigned int numberOfPCs );

/** Declare other functions. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::vector<std::string>  inputFileNames( 0, "" );
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string base = itksys::SystemTools::GetFilenamePath( inputFileNames[ 0 ] );
  if ( base != "" ) base = base + "/";
  std::string outputDirectory = base;
  parser->GetCommandLineArgument( "-out", outputDirectory );
  bool endslash = itksys::SystemTools::StringEndsWith( outputDirectory.c_str(), "/" );
  if ( !endslash ) outputDirectory += "/";

  unsigned int numberOfPCs = inputFileNames.size();
  parser->GetCommandLineArgument( "-npc", numberOfPCs );

  std::string componentType = "";
  bool retpt = parser->GetCommandLineArgument( "-opct", componentType );

  /** Check that numberOfOutputs <= numberOfInputs. */
  if ( numberOfPCs > inputFileNames.size() )
  {
    std::cerr << "ERROR: you should specify less than " << inputFileNames.size() << " output pc's." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileNames[ 0 ],
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  if ( !retpt ) componentType = ComponentTypeIn;

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( componentType );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( PerformPCA, unsigned char, 2 );
    run( PerformPCA, char, 2 );
    run( PerformPCA, unsigned short, 2 );
    run( PerformPCA, short, 2 );
    run( PerformPCA, unsigned int, 2 );
    run( PerformPCA, int, 2 );
    run( PerformPCA, unsigned long, 2 );
    run( PerformPCA, long, 2 );
    run( PerformPCA, float, 2 );
    run( PerformPCA, double, 2 );

    run( PerformPCA, unsigned char, 3 );
    run( PerformPCA, char, 3 );
    run( PerformPCA, unsigned short, 3 );
    run( PerformPCA, short, 3 );
    run( PerformPCA, unsigned int, 3 );
    run( PerformPCA, int, 3 );
    run( PerformPCA, unsigned long, 3 );
    run( PerformPCA, long, 3 );
    run( PerformPCA, float, 3 );
    run( PerformPCA, double, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentTypeIn
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main()


  /**
   * ******************* PerformPCA *******************
   */

template< class OutputImageType >
void PerformPCA(
  const std::vector< std::string > & inputFileNames,
  const std::string & outputDirectory,
  unsigned int numberOfPCs )
{
  const unsigned int Dimension = OutputImageType::ImageDimension;

  /** Typedefs. */
  typedef itk::Image< double, Dimension >               DoubleImageType;
  typedef itk::PCAImageToImageFilter<
    DoubleImageType, OutputImageType >                  PCAEstimatorType;
  typedef typename PCAEstimatorType::VectorOfDoubleType VectorOfDoubleType;
  typedef typename PCAEstimatorType::MatrixOfDoubleType MatrixOfDoubleType;
  typedef itk::ImageFileReader< DoubleImageType >       ReaderType;
  typedef typename ReaderType::Pointer                  ReaderPointer;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;
  typedef typename WriterType::Pointer                  WriterPointer;

  /** Get some sizes. */
  unsigned int noInputs = inputFileNames.size();

  /** Create the PCA estimator. */
  typename PCAEstimatorType::Pointer pcaEstimator = PCAEstimatorType::New();
  pcaEstimator->SetNumberOfFeatureImages( noInputs );
  pcaEstimator->SetNumberOfPrincipalComponentsRequired( numberOfPCs );

  /** For all inputs... */
  std::vector<ReaderPointer> readers( noInputs );
  for ( unsigned int i = 0; i < noInputs; ++i )
  {
    /** Read in the input images. */
    readers[ i ] = ReaderType::New();
    readers[ i ]->SetFileName( inputFileNames[ i ] );
    readers[ i ]->Update();

    /** Setup PCA estimator. */
    pcaEstimator->SetInput( i, readers[ i ]->GetOutput() );
  }

  /** Do the PCA analysis. */
  pcaEstimator->Update();

  /** Get eigenvalues and vectors, and print it to screen. */
  //pcaEstimator->Print( std::cout );
  VectorOfDoubleType vec = pcaEstimator->GetEigenValues();
  MatrixOfDoubleType mat = pcaEstimator->GetEigenVectors();

  std::cout << "Eigenvalues: " << std::endl;
  for ( unsigned int i = 0; i < vec.size(); ++i )
  {
    std::cout << vec[ i ] << " ";
  }
  std::cout << std::endl;

  std::cout << "Eigenvectors: " << std::endl;
  for ( unsigned int i = 0; i < vec.size(); ++i )
  {
    std::cout << mat.get_row( i ) << std::endl;
  }

  /** Setup and process the pipeline. */
  unsigned int noo = pcaEstimator->GetNumberOfOutputs();
  std::vector<WriterPointer> writers( noo );
  for ( unsigned int i = 0; i < noo; ++i )
  {
    /** Create output filename. */
    std::ostringstream makeFileName( "" );
    makeFileName << outputDirectory << "pc" << i << ".mhd";

    /** Write principal components. */
    writers[ i ] = WriterType::New();
    writers[ i ]->SetFileName( makeFileName.str().c_str() );
    writers[ i ]->SetInput( pcaEstimator->GetOutput( i ) );
    writers[ i ]->Update();
  }

} // end PerformPCA()


  /**
   * ******************* PrintHelp *******************
   */

std::string PrintHelp( void )
{
  std::string helpText = "Usage: \
  pxpca \
    -in      inputFilenames \
    [-out]   outputDirectory, default equal to the inputFilename directory \
    [-opc]   the number of principal components that you want to output, default all \
    [-opct]  output pixel component type, default derived from the input image \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return helpText;
} // end PrintHelp()

