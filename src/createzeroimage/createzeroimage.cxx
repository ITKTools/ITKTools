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
 \brief Create a blank image.

 \verbinclude createzeroimage.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "createzeroimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxcreatezeroimage\n"
    << "  [-in]    inputFilename\n"
    << "  -out     outputFilename\n"
    << "  -sz      size\n"
    << "  [-sp]    spacing\n"
    << "  [-o]     origin\n"
    << "  [-dim]   dimension, default 3\n"
    << "  [-opct]  pixelType, default short\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

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

  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back( "-sz" );
  exactlyOneArguments.push_back( "-in" );

  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments );

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
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int dim = 3;
  parser->GetCommandLineArgument( "-dim", dim );

  std::string componentTypeAsString = "short";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  std::vector<unsigned int> size( dim, 0 );
  bool retsz = parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( dim, 1.0 );
  bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( dim, 0.0 );
  bool reto = parser->GetCommandLineArgument( "-o", origin );

  std::vector<double> direction( dim * dim, 0.0 );
  for( unsigned int i = 0; i < dim; i++ )
  {
    direction[ i * ( dim + 1 ) ] = 1.0;
  }
  parser->GetCommandLineArgument( "-d", direction );

  /** Determine image properties. */
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  if( retin )
  {
    itk::IOPixelEnum pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
    unsigned int dim = 0;
    unsigned int numberOfComponents = 0;
    bool retgip = itktools::GetImageProperties(
      inputFileName, pixelType, componentType, dim, numberOfComponents,
      size, spacing, origin, direction );
    if( !retgip ) return EXIT_FAILURE;
  }

  /** Let the user overrule the component type. */
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Check size, spacing and origin. */
  if( retsz )
  {
    if( size.size() != dim && size.size() != 1 )
    {
      std::cout << "ERROR: The number of sizes should be 1 or Dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }
  if( retsp )
  {
    if( spacing.size() != dim && spacing.size() != 1 )
    {
      std::cout << "ERROR: The number of spacings should be 1 or Dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }
  if( reto )
  {
    if( origin.size() != dim && origin.size() != 1 )
    {
      std::cout << "ERROR: The number of origins should be 1 or Dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check size and spacing for nonpositive numbers. */
  for( unsigned int i = 0; i < dim; i++ )
  {
    if( size[ i ] < 1 )
    {
      std::cerr << "ERROR: For each dimension the size should be at least 1." << std::endl;
      return EXIT_FAILURE;
    }
    if( spacing[ i ] < 0.00001 )
    {
      std::cerr << "ERROR: No negative numbers are allowed in the spacing." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Class that does the work. */
  ITKToolsCreateZeroImageBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateZeroImage< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputFileName;
    filter->m_Size = size;
    filter->m_Spacing = spacing;
    filter->m_Origin = origin;

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
