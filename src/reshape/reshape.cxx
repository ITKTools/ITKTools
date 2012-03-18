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
 \brief Reshape an image.

 \verbinclude filter.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "reshape.h"
#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxpca\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFileName, default inputFileName_reshaped\n"
    << "  -s       size of the output image\n"
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
  parser->MarkArgumentAsRequired( "-s", "Output size." );

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

  std::string base = itksys::SystemTools::GetFilenameWithoutLastExtension(
    inputFileName );
  std::string ext  = itksys::SystemTools::GetFilenameLastExtension(
    inputFileName );
  std::string outputFileName = base + "_reshaped" + ext;
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned long> outputSize;
  parser->GetCommandLineArgument( "-s", outputSize );

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

  /** Check dimensions. */
  if( dim != outputSize.size() )
  {
    std::cerr << "ERROR: input and output dimension should be the same.\n";
    std::cerr << "  Please, specify only " << dim
      << "numbers with \"-s\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsReshapeBase * filter = 0;

  try
  {
    if( !filter ) filter = ITKToolsReshape< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsReshape< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReshape< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_OutputSize = outputSize;

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
