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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "intensityreplace.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program replaces some user specified intensity values in an image.\n"
    << "Usage:\n"
    << "pxintensityreplace\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + LUTAPPLIED.mhd\n"
    << "  -i       input pixel values that should be replaced\n"
    << "  -o       output pixel values that replace the corresponding input values\n"
    << "  [-pt]    output pixel type, default equal to input\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int,\n"
    << "(unsigned) long, float, double.\n"
    << "If \"-pt\" is used, the input is immediately converted to that particular\n"
    << "type, after which the intensity replacement is performed.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  RegisterMevisDicomTiff();

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
  if( inValues.size() != outValues.size() )
  {
    std::cerr << "ERROR: \"-i\" and \"-o\" should be followed by an equal number of values!" << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsIntensityReplaceBase * filter = nullptr;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsIntensityReplace< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsIntensityReplace< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityReplace< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputFileName;
    filter->m_InputFileName = inputFileName;
    filter->m_InValues = inValues;
    filter->m_OutValues = outValues;

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
