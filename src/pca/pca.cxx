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
 \brief Perform PCA.

 \verbinclude filter.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "pca.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxpca\n"
    << "  -in      inputFilenames\n"
    << "  [-out]   outputDirectory, default equal to the inputFilename directory\n"
    << "  [-opc]   the number of principal components that you want to output, default all\n"
    << "  [-opct]  output pixel component type, default derived from the input image\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

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
  std::vector<std::string>  inputFileNames( 0, "" );
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string base = itksys::SystemTools::GetFilenamePath( inputFileNames[ 0 ] );
  if( base != "" ) base = base + "/";
  std::string outputDirectory = base;
  parser->GetCommandLineArgument( "-out", outputDirectory );
  bool endslash = itksys::SystemTools::StringEndsWith( outputDirectory.c_str(), "/" );
  if( !endslash ) outputDirectory += "/";

  unsigned int numberOfPCs = inputFileNames.size();
  parser->GetCommandLineArgument( "-npc", numberOfPCs );

  std::string componentTypeString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeString );

  /** Check that numberOfOutputs <= numberOfInputs. */
  if( numberOfPCs > inputFileNames.size() )
  {
    std::cerr << "ERROR: you should specify less than " << inputFileNames.size() << " output pc's." << std::endl;
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

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** The default output is equal to the input, but can be overridden by
   * specifying -opct in the command line.
   */
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeString );
  }

  /** Class that does the work. */
  ITKToolsPCABase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsPCA< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsPCA< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsPCA< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileNames = inputFileNames;
    filter->m_OutputDirectory = outputDirectory;
    filter->m_NumberOfPCs = numberOfPCs;

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

} // end main()
