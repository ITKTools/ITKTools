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
 \brief Extract every other slice of an image.

 \verbinclude extracteveryotherslice.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "extracteveryotherslice.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxextracteveryotherslice\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + EveryOtherKExtracted.mhd\n"
    << "  [-K]     every other slice K, default 2\n"
    << "  [-of]    offset, default 0\n"
    << "  [-d]     direction, default is z-axes\n"
    << "Supported: 3D, (unsigned) char, (unsigned) short, float, double.";
  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  RegisterMevisDicomTiff();

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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "EveryOtherKExtracted.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int everyOther = 2;
  parser->GetCommandLineArgument( "-K", everyOther );

  unsigned int offset = 0;
  parser->GetCommandLineArgument( "-of", offset );
  offset = offset % everyOther;

  unsigned int direction = 2;
  parser->GetCommandLineArgument( "-d", direction );

  /** Check everyOther. */
  if( everyOther < 2 )
  {
    std::cout << "ERROR: K should be larger than 1." << std::endl;
    return EXIT_FAILURE;
  }

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

  /** Check for dimension. */
  if( dim != 3 )
  {
    std::cerr << "ERROR: The image dimension equals " << dim << "." << std::endl;
    std::cerr << "Only 3D images are supported." << std::endl;
    return EXIT_FAILURE;
  }

  /** Check direction. */
  if( direction + 1 > dim )
  {
    std::cout << "ERROR: direction should be 0, 1 or 2." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsExtractEveryOtherSliceBase * filter = nullptr;

  try
  {
    // now call all possible template combinations.

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractEveryOtherSlice< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_EveryOther = everyOther;
    filter->m_Offset = offset;
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
