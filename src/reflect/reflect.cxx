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
 \brief This program reflects an image.
 
 \verbinclude reflect.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------


/**
 * ******************* GetHelpString *******************
 */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program reflects an image." << std::endl
  << "Usage:" << std::endl
  << "pxreflect" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  -out     outputFilename" << std::endl
  << "  -d       the image direction that should be reflected" << std::endl
  << "  [-opct]  output pixel type, default equal to input" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int," << std::endl
  << "(unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()


/** Reflect */

class ReflectBase : public itktools::ITKToolsBase
{ 
public:
  ReflectBase(){};
  ~ReflectBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  unsigned int m_Direction;
    
}; // end ReflectBase


template< class TComponentType, unsigned int VDimension >
class Reflect : public ReflectBase
{
public:
  typedef Reflect Self;

  Reflect(){};
  ~Reflect(){};

  static Self * New( itktools::EnumComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs. */
    typedef TComponentType                                    OutputPixelType;
    const unsigned int Dimension = VDimension;

    typedef OutputPixelType                                 InputPixelType;

    typedef itk::Image< InputPixelType, Dimension >         InputImageType;
    typedef itk::Image< OutputPixelType, Dimension >        OutputImageType;

    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::FlipImageFilter< InputImageType >          ReflectFilterType;
    typedef itk::ImageFileWriter< OutputImageType >         WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename ReflectFilterType::Pointer reflectFilter = ReflectFilterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Set up pipeline. */
    reader->SetFileName( m_InputFileName );
    
    itk::FixedArray<bool, Dimension> flipAxes(false);
    flipAxes[m_Direction] = true;
    
    reflectFilter->SetFlipAxes( flipAxes );
    writer->SetFileName( m_OutputFileName );

    reflectFilter->SetInput( reader->GetOutput() );
    writer->SetInput( reflectFilter->GetOutput() );
    writer->Update();
  }

}; // end Reflect

//-------------------------------------------------------------------------------------

/* Declare ReflectImageFilter. */
template< class TOutputPixel, unsigned int NDimension >
void ReflectImageFilter(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const unsigned int direction );

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
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-d", "Direction." );

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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int direction = 0;
  parser->GetCommandLineArgument( "-d", direction );

  std::string componentTypeString = "";
  bool retpt = parser->GetCommandLineArgument( "-opct", componentTypeString );

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.   */
  itktools::EnumComponentType componentType = itktools::GetImageComponentType(inputFileName);
  if ( !retpt ) 
  {
    componentType = itktools::EnumComponentTypeFromString(componentTypeString);
  }
  
  /** Check for vector images. */
  unsigned int numberOfComponents = 0;
  GetImageDimension(inputFileName, numberOfComponents);
  
  if ( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1;
  }

  unsigned int imageDimension = 0;
  GetImageDimension(inputFileName, imageDimension);
  
  /** Check direction. */
  if ( direction > imageDimension - 1 )
  {
    std::cerr << "ERROR: invalid direction." << std::endl;
    return 1;
  }

 
  /** Class that does the work */
  ReflectBase * reflect = NULL; 

  try
  {    
    // now call all possible template combinations.
    if (!reflect) reflect = Reflect< unsigned char, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< char, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned short, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< short, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned int, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< int, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned long, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< long, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< float, 2 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< double, 2 >::New( componentType, imageDimension );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!reflect) reflect = Reflect< unsigned char, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< char, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned short, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< short, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned int, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< int, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< unsigned long, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< long, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< float, 3 >::New( componentType, imageDimension );
    if (!reflect) reflect = Reflect< double, 3 >::New( componentType, imageDimension );
#endif
    if (!reflect) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << imageDimension
        << std::endl;
      return 1;
    }

    reflect->m_InputFileName = inputFileName;
    reflect->m_OutputFileName = outputFileName;
    reflect->m_Direction = direction;
    
    reflect->Run();
    
    delete reflect;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete reflect;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main
