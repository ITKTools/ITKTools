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
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include <itksys/SystemTools.hxx>
#include <sstream>

#include "itkPCAImageToImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
  << "Usage:" << std::endl
  << "pxpca" << std::endl
  << "  -in      inputFilenames" << std::endl
  << "  [-out]   outputDirectory, default equal to the inputFilename directory" << std::endl
  << "  [-opc]   the number of principal components that you want to output, default all" << std::endl
  << "  [-opct]  output pixel component type, default derived from the input image" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()


/** PCA */

class ITKToolsPCABase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsPCABase()
  {
    //std::vector< std::string > this->m_InputFileNames;
    this->m_OutputDirectory = "";
    this->m_NumberOfPCs = 0;
  };
  ~ITKToolsPCABase(){};

  /** Input parameters */
  std::vector< std::string > m_InputFileNames;
  std::string m_OutputDirectory;
  unsigned int m_NumberOfPCs;

    
}; // end ReplaceVoxelBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsPCA : public ITKToolsPCABase
{
public:
  typedef ITKToolsPCA Self;

  ITKToolsPCA(){};
  ~ITKToolsPCA(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TComponentType, VDimension >      OutputImageType;
    typedef itk::Image< double, VDimension >              DoubleImageType;
    typedef itk::PCAImageToImageFilter<
      DoubleImageType, OutputImageType >                  PCAEstimatorType;
    typedef typename PCAEstimatorType::VectorOfDoubleType VectorOfDoubleType;
    typedef typename PCAEstimatorType::MatrixOfDoubleType MatrixOfDoubleType;
    typedef itk::ImageFileReader< DoubleImageType >       ReaderType;
    typedef typename ReaderType::Pointer                  ReaderPointer;
    typedef itk::ImageFileWriter< OutputImageType >       WriterType;
    typedef typename WriterType::Pointer                  WriterPointer;

    /** Get some sizes. */
    unsigned int noInputs = this->m_InputFileNames.size();

    /** Create the PCA estimator. */
    typename PCAEstimatorType::Pointer pcaEstimator = PCAEstimatorType::New();
    pcaEstimator->SetNumberOfFeatureImages( noInputs );
    pcaEstimator->SetNumberOfPrincipalComponentsRequired( this->m_NumberOfPCs );

    /** For all inputs... */
    std::vector<ReaderPointer> readers( noInputs );
    for ( unsigned int i = 0; i < noInputs; ++i )
    {
      /** Read in the input images. */
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( this->m_InputFileNames[ i ] );
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
      makeFileName << this->m_OutputDirectory << "pc" << i << ".mhd";

      /** Write principal components. */
      writers[ i ] = WriterType::New();
      writers[ i ]->SetFileName( makeFileName.str().c_str() );
      writers[ i ]->SetInput( pcaEstimator->GetOutput( i ) );
      writers[ i ]->Update();
    }
  }

}; // end PCA

//-------------------------------------------------------------------------------------

/* Declare PerformPCA. */
template< class OutputImageType >
void PerformPCA(
  const std::vector< std::string > & inputFileNames,
  const std::string & outputDirectory,
  unsigned int numberOfPCs );


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
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

  std::string componentTypeString = "";
  bool retpt = parser->GetCommandLineArgument( "-opct", componentTypeString );

  /** Check that numberOfOutputs <= numberOfInputs. */
  if ( numberOfPCs > inputFileNames.size() )
  {
    std::cerr << "ERROR: you should specify less than " << inputFileNames.size() << " output pc's." << std::endl;
    return 1;
  }

  unsigned int numberOfComponents = 0;
  itktools::GetImageNumberOfComponents(inputFileNames[0], numberOfComponents);
  /** Check for vector images. */
  if ( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);
 
  if ( !retpt ) 
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeString );
  }
  
  /** Class that does the work */
  ITKToolsPCABase * pca = 0; 

  unsigned int imageDimension = 0;
  itktools::GetImageDimension(inputFileNames[0], imageDimension);
  
  std::cout << "Internal image component type: " << 
    itk::ImageIOBase::GetComponentTypeAsString( componentType ) << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!pca) pca = ITKToolsPCA< unsigned char, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< char, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned short, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< short, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned int, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< int, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned long, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< long, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< float, 2 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< double, 2 >::New( componentType, imageDimension );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!pca) pca = ITKToolsPCA< unsigned char, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< char, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned short, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< short, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned int, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< int, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< unsigned long, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< long, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< float, 3 >::New( componentType, imageDimension );
    if (!pca) pca = ITKToolsPCA< double, 3 >::New( componentType, imageDimension );
#endif
    if (!pca) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << imageDimension
        << std::endl;
      return 1;
    }

    pca->m_InputFileNames = inputFileNames;
    pca->m_OutputDirectory = outputDirectory;
    pca->m_NumberOfPCs = numberOfPCs;
    
    pca->Run();
    
    delete pca;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete pca;
    return 1;
  }

  /** End program. */
  return 0;

} // end main()

