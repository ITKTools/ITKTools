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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

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
    << "  [-f]     resize factor\n"
    << "  [-sp]    output spacing\n"
    << "  [-sz]    output size\n"
    << "  [-io]    interpolation order, default 1\n"
    << "One of {-f, -sp, -sz} should be given.\n"
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

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back( "-f" );
  exactlyOneArguments.push_back( "-sp" );
  exactlyOneArguments.push_back( "-sz" );
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

  std::vector<double> outputSpacing;
  bool retsp = parser->GetCommandLineArgument( "-sp", outputSpacing );

  std::vector<unsigned int> outputSize;
  bool retsz = parser->GetCommandLineArgument( "-sz", outputSize );

  unsigned int interpolationOrder = 1;
  parser->GetCommandLineArgument( "-io", interpolationOrder );

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::IOPixelEnum::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check resize factor, output spacing or output size. */
  std::string resizingSpecifiedBy = "";
  if( retf )
  {
    resizingSpecifiedBy = "ResizeFactor";
    if( factor.size() != dim )
    {
      std::cout << "ERROR: The number of factors should equal image dimension." << std::endl;
      return EXIT_FAILURE;
    }
    for( unsigned int i = 0; i < dim; ++i )
    {
      if( factor[ i ] < 0.00001 )
      {
        std::cout << "ERROR: No negative numbers are allowed in the resizing factor." << std::endl;
        return EXIT_FAILURE;
      }
    }
  }
  if( retsp )
  {
    resizingSpecifiedBy = "OutputSpacing";
    if( outputSpacing.size() != dim )
    {
      std::cout << "ERROR: The number of output spacings should equal image dimension." << std::endl;
      return EXIT_FAILURE;
    }
    for( unsigned int i = 0; i < dim; ++i )
    {
      if( outputSpacing[ i ] < 0.00001 )
      {
        std::cout << "ERROR: No negative numbers are allowed in the output spacing." << std::endl;
        return EXIT_FAILURE;
      }
    }
  }
  if( retsz )
  {
    resizingSpecifiedBy = "OutputSize";
    if( outputSize.size() != dim )
    {
      std::cout << "ERROR: The number of output sizes should equal image dimension." << std::endl;
      return EXIT_FAILURE;
    }
  }

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
    filter->m_ResizingSpecifiedBy = resizingSpecifiedBy;
    filter->m_ResizeFactor = factor;
    filter->m_OutputSpacing = outputSpacing;
    filter->m_OutputSize = outputSize;
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
