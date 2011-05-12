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
 \brief Compute binary thinning of an image.
 
 \verbinclude binarythinning.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkImageFileWriter.h"


class BinaryThinningBase : public itktools::ITKToolsBase
{
public:
  BinaryThinningBase()
  {
    
  }

  ~BinaryThinningBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end BinaryThinningBase


template< itktools::EnumComponentType VComponentType, unsigned int VImageDimension >
class BinaryThinning : public BinaryThinningBase
{
public:
  typedef BinaryThinning Self;

  BinaryThinning(){};
  ~BinaryThinning(){};

  static Self * New( itktools::EnumComponentType componentType, unsigned int imageDimension )
  {
    if ( VComponentType == componentType && VImageDimension == imageDimension )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedef's. */
    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::BinaryThinningImageFilter<
      InputImageType, InputImageType >                      FilterType;
    typedef itk::ImageFileWriter< InputImageType >          WriterType;

    /** Read in the input images. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFileName );

    /** Thin the image. */
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );

    /** Write image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFileName );
    writer->SetInput( filter->GetOutput() );
    writer->Update();
  }

}; // end BinaryThinning
//-------------------------------------------------------------------------------------


/** Declare PrintHelp. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "THINNED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
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
    std::cerr << "Vector images are not supported. Thinning only works on binary images." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );


  /** Determine image properties. */

  EnumComponentType componentType = itktools::GetImageComponentType(inputFileName);
  
  unsigned int dimension = 0;
  GetImageDimension(inputFileName, dimension);

  BinaryThinningBase * binaryThinning = 0;

  try
  {
    // 2D
    if (!binaryThinning) binaryThinning = BinaryThinning< char, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned char, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< short, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned short, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< int, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned int, 2u >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< long, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned long, 2u >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< float, 2 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< double, 2 >::New( componentType, dimension );

    // 3D
    if (!binaryThinning) binaryThinning = BinaryThinning< char, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned char, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< short, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned short, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< int, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned int, 3u >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< long, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< unsigned long, 3u >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< float, 3 >::New( componentType, dimension );
    if (!binaryThinning) binaryThinning = BinaryThinning< double, 3 >::New( componentType, dimension );

    if ( !binaryThinning )
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    binaryThinning->m_InputFileName = inputFileName;
    binaryThinning->m_OutputFileName = outputFileName;

    binaryThinning->Run();

    delete binaryThinning;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete binaryThinning;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


  /**
   * ******************* GetHelpString *******************
   */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
     << "pxbinarythinning" << std::endl
     << "-in      inputFilename" << std::endl
     << "[-out]   outputFilename, default in + THINNED.mhd" << std::endl
     << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl
     << "Note that the thinning algorithm used here is really a 2D thinning algortihm." << std::endl
     << "In 3D the thinning is performed slice by slice.";
  return ss.str();
} // end GetHelpString()

