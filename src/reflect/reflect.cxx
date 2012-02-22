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

 \verbinclude filter.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "reflect.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program reflects an image.\n"
    << "Usage:\n"
    << "pxreflect\n"
    << "  -in      inputFilename\n"
    << "  -out     outputFilename\n"
    << "  -d       the image direction that should be reflected\n"
    << "  [-opct]  output pixel type, default equal to input\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int,\n"
    << "(unsigned) long, float, double.";

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
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-d", "Direction." );

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

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int direction = 0;
  parser->GetCommandLineArgument( "-d", direction );

  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** The default output is equal to the input, but can be overridden by
   * specifying -opct in the command line.
   */
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Check direction. */
  if( direction > dim - 1 )
  {
    std::cerr << "ERROR: invalid direction." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsReflectBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsReflect< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsReflect< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReflect< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Direction = direction;

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
