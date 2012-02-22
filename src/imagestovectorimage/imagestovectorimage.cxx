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
#include "imagestovectorimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pximagetovectorimage\n"
    << "  -in      inputFilenames, at least 2\n"
    << "  [-out]   outputFilename, default VECTOR.mhd\n"
    << "  [-s]     number of streams, default 1.\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short,\n"
    << "(unsigned) int, (unsigned) long, float, double.\n"
    << "Note: make sure that the input images are of the same type, size, etc.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
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

  std::string outputFileName = "VECTOR.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int numberOfStreams = 1;
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Check if the required arguments are given. */
  if( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two (2) input files." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ], pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsImagesToVectorImageBase * filter = NULL;

  try
  {    
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsImagesToVectorImage< 3, double >::New( dim, componentType );    
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileNames = inputFileNames;
    filter->m_OutputFileName = outputFileName;
    filter->m_NumberOfStreams = numberOfStreams;

    filter->Run();
    
    delete filter;  
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
