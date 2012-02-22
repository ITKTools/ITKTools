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
 \brief This program extracts a user specified component from a vector image.
 
 \verbinclude extractindexfromvectorimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "extractindexfromvectorimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program extracts a user specified component from a vector image.\n"
    << "Usage:\n"
    << "pxextractindexfromvectorimage\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + INDEXEXTRACTED.mhd\n"
    << "  -ind     one or more valid indices\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int,\n"
    << "long, float, double.";
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
  parser->MarkArgumentAsRequired( "-ind", "The index or indices to extract." );

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
  outputFileName += "INDEXEXTRACTED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> indices;
  parser->GetCommandLineArgument( "-ind", indices );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  if( numberOfComponents == 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is 1!" << std::endl;
    std::cerr << "  Cannot make extract index from a scalar image." << std::endl;
    return EXIT_FAILURE;
  }

  /** Sanity check. */
  for( unsigned int i = 0; i < indices.size(); ++i )
  {
    if( indices[ i ] > numberOfComponents - 1 )
    {
      std::cerr << "ERROR: You selected index "
        << indices[ i ] << ", where the input image only has "
        << numberOfComponents << " components." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Run the program. */
  ITKToolsExtractIndexBase * filter = 0;

  try
  {
    // 2D
    if( !filter ) filter = ITKToolsExtractIndex< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    // 3D
    if( !filter ) filter = ITKToolsExtractIndex< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractIndex< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Indices = indices;

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
