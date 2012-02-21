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
 \brief Resize an image.
 
 \verbinclude resizeimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "resizeimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxresizeimage\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + RESIZED.mhd\n"
    << "  [-f]     factor\n"
    << "  [-sp]    spacing\n"
    << "  [-io]    interpolation order, default 1\n"
    << "  [-dim]   dimension, default 3\n"
    << "One of -f and -sp should be given.\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back( "-f" );
  exactlyOneArguments.push_back( "-sp" );
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
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "RESIZED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<double> factor;
  bool retf = parser->GetCommandLineArgument( "-f", factor );
  bool isFactor = retf;

  std::vector<double> spacing;
  bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  unsigned int interpolationOrder = 1;
  parser->GetCommandLineArgument( "-io", interpolationOrder );

  /** Check factor and spacing. */
  if( retf )
  {
    if( factor.size() != Dimension && factor.size() != 1 )
    {
      std::cout << "ERROR: The number of factors should be 1 or Dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }
  if( retsp )
  {
    if( spacing.size() != Dimension && spacing.size() != 1 )
    {
      std::cout << "ERROR: The number of spacings should be 1 or Dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Get the factor or spacing. */
  double vector0 = ( retf ? factor[ 0 ] : spacing[ 0 ] );
  std::vector<double> factorOrSpacing( Dimension, vector0 );
  if( retf && factor.size() == Dimension )
  {
    for( unsigned int i = 1; i < Dimension; i++ )
    {
      factorOrSpacing[ i ] = factor[ i ];
    }
  }
  if( retsp && spacing.size() == Dimension )
  {
    for( unsigned int i = 1; i < Dimension; i++ )
    {
      factorOrSpacing[ i ] = spacing[ i ];
    }
  }

  /** Check factorOrSpacing for nonpositive numbers. */
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    if( factorOrSpacing[ i ] < 0.00001 )
    {
      std::cout << "ERROR: No negative numbers are allowed in factor or spacing." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsResizeImageBase * filter = 0; 

  try
  {    
    if( !filter ) filter = ITKToolsResizeImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsResizeImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsResizeImage< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_FactorOrSpacing = factorOrSpacing;
    filter->m_IsFactor = isFactor;
    filter->m_InterpolationOrder = interpolationOrder;

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
