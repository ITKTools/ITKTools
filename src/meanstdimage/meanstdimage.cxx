/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria, Hakim Achterberg. 2011.
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
 \brief Threshold an image.
 
 \verbinclude thresholdimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "meanstdimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "This program creates a mean and standard deviation image of a set of images." << std::endl
    << "Usage:" << std::endl
    << "pxmeanstdimage" << std::endl
    << "  -in        list of inputFilenames" << std::endl
    << "  [-outmean] outputFilename for mean image; always written as float" << std::endl
    << "  [-outstd]  outputFilename for standard deviation image; always written as float," << std::endl
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

  return ss.str();

} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filenames." );

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
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileNameMean("");
  parser->GetCommandLineArgument( "-out", outputFileNameMean );
  bool retoutmean = parser->GetCommandLineArgument( "-outmean", outputFileNameMean );

  std::string outputFileNameStd("");
  parser->GetCommandLineArgument( "-mask", outputFileNameStd );
  bool retoutstd  = parser->GetCommandLineArgument( "-outstd", outputFileNameStd );

  /** Determine image properties. */
  std::string ComponentTypeIn = "float";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileNames[0],
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


  /** Class that does the work */
  ITKToolsMeanStdImageBase * meanStdImage = 0; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileNames[0] );

  try
  {    
    // now call all possible template combinations.
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< char, 2 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< unsigned char, 2 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< short, 2 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< unsigned short, 2 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< float, 2 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< double, 2 >::New( componentType, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< char, 3 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< unsigned char, 3 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< short, 3 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< unsigned short, 3 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< float, 3 >::New( componentType, dim );
    if (!meanStdImage) meanStdImage = ITKToolsMeanStdImage< double, 3 >::New( componentType, dim );
#endif
    if (!meanStdImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    meanStdImage->m_InputFileNames = inputFileNames;
    meanStdImage->m_OutputFileNameMean= outputFileNameMean;
    meanStdImage->m_OutputFileNameStd = outputFileNameStd;
    meanStdImage->m_CalcMean = retoutmean;
    meanStdImage->m_CalcStd = retoutstd;

    meanStdImage->Run();
    
    delete meanStdImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete meanStdImage;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
