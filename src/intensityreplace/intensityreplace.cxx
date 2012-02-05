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
 \brief This program replaces some user specified intensity values in an image.
 
 \verbinclude intensityreplace.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkChangeLabelImageFilter.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program replaces some user specified intensity values in an image." << std::endl
    << "Usage:" << std::endl
    << "pxintensityreplace" << std::endl
    << "  -in      inputFilename" << std::endl
    << "  [-out]   outputFilename, default in + LUTAPPLIED.mhd" << std::endl
    << "  -i       input pixel values that should be replaced" << std::endl
    << "  -o       output pixel values that replace the corresponding input values" << std::endl
    << "  [-pt]    output pixel type, default equal to input" << std::endl
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int," << std::endl
    << "(unsigned) long, float, double." << std::endl
    << "If \"-pt\" is used, the input is immediately converted to that particular" << std::endl
    << "type, after which the intensity replacement is performed.";

  return ss.str();

} // end GetHelpString()


/** IntensityReplace */

class ITKToolsIntensityReplaceBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsIntensityReplaceBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    //std::vector<std::string> this->m_InValues;
    //std::vector<std::string> this->m_OutValues;
  };
  ~ITKToolsIntensityReplaceBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<std::string> m_InValues;
  std::vector<std::string> m_OutValues;

}; // end IntensityReplaceBase

template< class TComponentType, unsigned int VDimension >
//template< unsigned int VImageDimension, class TValue >
class ITKToolsIntensityReplace : public ITKToolsIntensityReplaceBase
{
public:
  typedef ITKToolsIntensityReplace Self;

  ITKToolsIntensityReplace(){};
  ~ITKToolsIntensityReplace(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  //static Self * New( unsigned int imageDimension, itktools::ComponentType componentType )
  //{
    //if ( VImageDimension == imageDimension && itktools::IsType<TValue>( componentType ) )
    //{
      //return new Self;
    //}
    //return 0;
  //}

  void Run( void )
  {
    /** Typedefs. */
    //typedef TValue                                          OutputPixelType;
    typedef TComponentType                                  OutputPixelType;
    const unsigned int Dimension = VDimension;

    typedef OutputPixelType                                 InputPixelType;

    typedef itk::Image< InputPixelType, Dimension >         InputImageType;
    typedef itk::Image< OutputPixelType, Dimension >        OutputImageType;

    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::ChangeLabelImageFilter<
      InputImageType, OutputImageType >                     ReplaceFilterType;
    typedef itk::ImageFileWriter< OutputImageType >         WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename ReplaceFilterType::Pointer replaceFilter = ReplaceFilterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Set up reader */
    reader->SetFileName( this->m_InputFileName );

    /** Setup the the input and the 'change map' of the replace filter. */
    replaceFilter->SetInput( reader->GetOutput() );
    if ( itk::NumericTraits<OutputPixelType>::is_integer )
    {
      for (unsigned int i = 0; i < this->m_InValues.size(); ++i)
      {
        const InputPixelType inval = static_cast< InputPixelType >(
          atoi( this->m_InValues[i].c_str() )   );
        const OutputPixelType outval = static_cast< OutputPixelType >(
          atoi( this->m_OutValues[i].c_str() )   );
        replaceFilter->SetChange( inval, outval );
      }
    }
    else
    {
      for (unsigned int i = 0; i < this->m_InValues.size(); ++i)
      {
        const InputPixelType inval = static_cast< InputPixelType >(
          atof( this->m_InValues[i].c_str() )   );
        const OutputPixelType outval = static_cast< OutputPixelType >(
          atof( this->m_OutValues[i].c_str() )   );
        replaceFilter->SetChange( inval, outval );
      }
    }

    /** Set up writer. */
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( replaceFilter->GetOutput() );
    writer->Update();
  }

}; // end IntensityReplace

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-i", "In values." );
  parser->MarkArgumentAsRequired( "-o", "Out values." );

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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Read as vector of strings, since we don't know yet if it will be
   * integers or floats */
  std::vector< std::string > inValues;
  parser->GetCommandLineArgument( "-i", inValues );
  std::vector< std::string > outValues;
  parser->GetCommandLineArgument( "-o", outValues );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "LUTAPPLIED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ComponentTypeString = "";
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentTypeString );

  /** Check if the required arguments are given. */
  if ( inValues.size() != outValues.size() )
  {
    std::cerr << "ERROR: \"-i\" and \"-o\" should be followed by an equal number of values!" << std::endl;
    return 1;
  }

  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  if ( retpt )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( ComponentTypeString );
  }

  /** Check for vector images. */
  unsigned int numberOfComponents = 0;
  itktools::GetImageNumberOfComponents( inputFileName, numberOfComponents );
  
  if ( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1;
  }
  
  /** Class that does the work */
  ITKToolsIntensityReplaceBase * intensityReplace = NULL; 

  unsigned int dim = 0;
  itktools::GetImageDimension( inputFileName, dim );
  
  try
  {    
    // now call all possible template combinations.
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< char, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned char, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< short, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned short, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< int, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned int, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< long, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned long, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< float, 2 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< double, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< char, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned char, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< short, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned short, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< int, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned int, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< long, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< unsigned long, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< float, 3 >::New( componentType, dim );
    if (!intensityReplace) intensityReplace = ITKToolsIntensityReplace< double, 3 >::New( componentType, dim );
#endif
    if (!intensityReplace) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << dim << std::endl
        << " pixel type = " << itk::ImageIOBase::GetComponentTypeAsString( componentType ) << std::endl
        << std::endl;
      return 1;
    }

    intensityReplace->m_OutputFileName = outputFileName;
    intensityReplace->m_InputFileName = inputFileName;
    intensityReplace->m_InValues = inValues;
    intensityReplace->m_OutValues = outValues;
    
    intensityReplace->Run();
    
    delete intensityReplace;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete intensityReplace;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
