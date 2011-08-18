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
 \brief Intensity windowing.
 
 \verbinclude intensitywindowing.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxintensitywindowing" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + WINDOWED.mhd" << std::endl
  << "  -w       windowMinimum windowMaximum" << std::endl
  << "  [-pt]    pixel type of input and output images;" << std::endl
  << "           default: automatically determined from the first input image." << std::endl
  << "Supported: 2D, 3D, (unsigned) short, (unsigned) char, float.";

  return ss.str();

} // end GetHelpString()



/** IntensityWindowing */

class ITKToolsIntensityWindowingBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsIntensityWindowingBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    //std::vector<double> this->m_Window;
  };
  ~ITKToolsIntensityWindowingBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_Window;

    
}; // end IntensityWindowingBase


template< unsigned int VImageDimension, class TComponentType >
class ITKToolsIntensityWindowing : public ITKToolsIntensityWindowingBase
{
public:
  typedef ITKToolsIntensityWindowing Self;

  ITKToolsIntensityWindowing(){};
  ~ITKToolsIntensityWindowing(){};

  static Self * New( unsigned int imageDimension, itktools::ComponentType componentType )
  {
    if ( VImageDimension == imageDimension && itktools::IsType<TComponentType>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VImageDimension>     InputImageType;
    typedef itk::IntensityWindowingImageFilter<
      InputImageType, InputImageType >                  WindowingType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< InputImageType >      WriterType;
    typedef typename InputImageType::PixelType          InputPixelType;

    /** Declarations. */
    typename WindowingType::Pointer windowfilter = WindowingType::New();
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Setup the pipeline. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    InputPixelType min = static_cast<InputPixelType>( this->m_Window[ 0 ] );
    InputPixelType max = static_cast<InputPixelType>( this->m_Window[ 1 ] );
    windowfilter->SetWindowMinimum( min );
    windowfilter->SetWindowMaximum( max );
    windowfilter->SetOutputMinimum( min );
    windowfilter->SetOutputMaximum( max );

    /** Connect and execute the pipeline. */
    windowfilter->SetInput( reader->GetOutput() );
    writer->SetInput( windowfilter->GetOutput() );
    writer->Update();
  }

}; // end IntensityWindowing

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

  /** Get input file name. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Determine input image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Let the user overrule this. */
  parser->GetCommandLineArgument( "-pt", ComponentType );

  /** Error checking. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get the output file name. */
  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "WINDOWED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get the window. */
  std::vector<double> window;
  bool retw = parser->GetCommandLineArgument( "-w", window );

  //unsigned int Dimension = 3;
  //bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

  /** Check if the required arguments are given. */
  if ( !retw )
  {
    std::cerr << "ERROR: You should specify \"-w\"." << std::endl;
    return 1;
  }

  /** Check window. */
  if( window.size() != 2 )
  {
    std::cout << "ERROR: The window should consist of two numbers." << std::endl;
    return 1;
  }
  if ( window[ 1 ] < window[ 0 ] )
  {
    double temp = window[ 0 ];
    window[ 0 ] = window[ 1 ];
    window[ 1 ] = temp;
  }
  if ( window[ 0 ] == window[ 1 ] )
  {
    std::cerr << "ERROR: The window should be larger." << std::endl;
    return 1;
  }


  /** Class that does the work */
  ITKToolsIntensityWindowingBase * intensityWindowing = NULL; 

  unsigned int imageDimension = 0;
  itktools::GetImageDimension(inputFileName, imageDimension);

  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  
  try
  {    
    // now call all possible template combinations.
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 2, unsigned char >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 2, char >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 2, unsigned short >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 2, short >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 2, float >::New( imageDimension, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 3, unsigned char >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 3, char >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 3, unsigned short >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 3, short >::New( imageDimension, componentType );
    if (!intensityWindowing) intensityWindowing = ITKToolsIntensityWindowing< 3, float >::New( imageDimension, componentType );
#endif
    if (!intensityWindowing) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << imageDimension << std::endl
        << " pixel type = " << componentType << std::endl
        << std::endl;
      return 1;
    }

    intensityWindowing->m_OutputFileName = outputFileName;
    intensityWindowing->m_InputFileName = inputFileName;
    intensityWindowing->m_Window = window;
    
    intensityWindowing->Run();
    
    delete intensityWindowing;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete intensityWindowing;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
