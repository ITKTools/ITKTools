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
 \brief Stack images into one big vector image.
 
 \verbinclude imagestovectorimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

//-------------------------------------------------------------------------------------

/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pximagetovectorimage" << std::endl
  << "  -in      inputFilenames, at least 2" << std::endl
  << "  [-out]   outputFilename, default VECTOR.mhd" << std::endl
  << "  [-s]     number of streams, default 1." << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short," << std::endl
  << "(unsigned) int, (unsigned) long, float, double." << std::endl
  << "Note: make sure that the input images are of the same type, size, etc.";

  return ss.str();

} // end GetHelpString()

/** ImagesToVectorImage */

class ImagesToVectorImageBase : public itktools::ITKToolsBase
{ 
public:
  ImagesToVectorImageBase(){};
  ~ImagesToVectorImageBase(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  unsigned int m_NumberOfStreams;
    
}; // end ImagesToVectorImageBase


template< class TComponentType, unsigned int VDimension >
class ImagesToVectorImage : public ImagesToVectorImageBase
{
public:
  typedef ImagesToVectorImage Self;

  ImagesToVectorImage(){};
  ~ImagesToVectorImage(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return NULL;
  }

  void Run(void)
  {
    /** Typedef's. */
    typedef itk::VectorImage<TComponentType, VDimension>    VectorImageType;
    typedef VectorImageType                                  OutputImageType;
    typedef itk::ImageFileReader< VectorImageType >          ReaderType;
    typedef itk::Image<TComponentType, VDimension> ScalarImageType;
    typedef itk::ImageFileWriter< VectorImageType >         WriterType;
    
    /** Read in the input images. */
    std::vector<typename ReaderType::Pointer> readers( m_InputFileNames.size() );
    for ( unsigned int i = 0; i < m_InputFileNames.size(); ++i )
    {
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( m_InputFileNames[ i ] );
      readers[ i ]->Update();
    }

    /** Create assembler. */
    typedef itk::ImageToVectorImageFilter< ScalarImageType > ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    // For each input image
    std::cout << "There are " << m_InputFileNames.size() << " input images." << std::endl;
    unsigned int currentOutputIndex = 0;
    for ( unsigned int inputImageIndex = 0; inputImageIndex < m_InputFileNames.size(); ++inputImageIndex )
    {
      typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;

      // For each component of the current image
      std::cout << "There are " << readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel() << " components in image " 
		<< inputImageIndex << std::endl;
      for ( unsigned int component = 0; component < readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel(); ++component )
      {
	typename ComponentExtractionType::Pointer componentExtractionFilter = ComponentExtractionType::New();
	componentExtractionFilter->SetIndex(component);
	componentExtractionFilter->SetInput(readers[inputImageIndex]->GetOutput());
	componentExtractionFilter->Update();
	
	imageToVectorImageFilter->SetNthInput( currentOutputIndex, componentExtractionFilter->GetOutput());
	currentOutputIndex++;
      }
    }
    
    imageToVectorImageFilter->Update();
    
    std::cout << "Output image has " << imageToVectorImageFilter->GetOutput()->GetNumberOfComponentsPerPixel() << " components." << std::endl;

    /** Write vector image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( m_OutputFileName );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetNumberOfStreamDivisions( m_NumberOfStreams );
    writer->Update();

  }

}; // end ImagesToVectorImage

//-------------------------------------------------------------------------------------

/* Declare ComposeVectorImage. */
template< class InputImageType, class OutputImageType >
void ComposeVectorImage(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const unsigned int & numberOfStreams );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

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

  std::string outputFileName = "VECTOR.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Support for streaming. */
  unsigned int numberOfStreams = 1;
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Check if the required arguments are given. */
  if ( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two (2) input files." << std::endl;
    return 1;
  }

  /** Class that does the work */
  ImagesToVectorImageBase * imagesToVectorImage = NULL;

  unsigned int dim = 0;
  itktools::GetImageDimension(inputFileNames[0], dim);
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< char, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned char, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< short, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned short, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< int, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned int, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< long, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned long, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< float, 2 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< double, 2 >::New( componentType, dim );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< char, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned char, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< short, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned short, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< int, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned int, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< long, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< unsigned long, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< float, 3 >::New( componentType, dim );
    if (!imagesToVectorImage) imagesToVectorImage = ImagesToVectorImage< double, 3 >::New( componentType, dim );    
#endif
    if (!imagesToVectorImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << dim
        << std::endl;
      return 1;
    }

    imagesToVectorImage->m_InputFileNames = inputFileNames;
    imagesToVectorImage->m_OutputFileName = outputFileName;
    imagesToVectorImage->m_NumberOfStreams = numberOfStreams;

    imagesToVectorImage->Run();
    
    delete imagesToVectorImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete imagesToVectorImage;
    return 1;
  }


  /** End program. */
  return 0;

} // end main
