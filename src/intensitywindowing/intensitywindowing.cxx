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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "intensitywindowing.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxintensitywindowing\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + WINDOWED.mhd\n"
    << "  -w       windowMinimum windowMaximum\n"
    << "  [-pt]    pixel type of input and output images\n"
    << "           default: automatically determined from the first input image.\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, float.";

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

  /** Get input file name. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get the output file name. */
  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "WINDOWED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get the window. */
  std::vector<double> window;
  bool retw = parser->GetCommandLineArgument( "-w", window );

  /** Check if the required arguments are given. */
  if( !retw )
  {
    std::cerr << "ERROR: You should specify \"-w\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Check window. */
  if( window.size() != 2 )
  {
    std::cout << "ERROR: The window should consist of two numbers." << std::endl;
    return EXIT_FAILURE;
  }
  if( window[ 1 ] < window[ 0 ] )
  {
    double temp = window[ 0 ];
    window[ 0 ] = window[ 1 ];
    window[ 1 ] = temp;
  }
  if( window[ 0 ] == window[ 1 ] )
  {
    std::cerr << "ERROR: The window should be larger." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsIntensityWindowingBase * filter = nullptr;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 2, float >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsIntensityWindowing< 3, float >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputFileName;
    filter->m_InputFileName = inputFileName;
    filter->m_Window = window;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
