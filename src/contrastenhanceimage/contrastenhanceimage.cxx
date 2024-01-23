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
 \brief Enhance the contrast of an image.

 \verbinclude contrastenhanceimage.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "contrastenhanceimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program enhances an image.\n"
    << "alpha and beta control the exact behaviour of the filter. See the\n"
    << "ITK documentation of the AdaptiveHistogramEqualizationImageFilter\n"
    << "Usage:\n"
    << "pxcontrastenhanceimage\n"
    << "-in    \tInputImageFileName\n"
    << "-out   \tOutputImageFileName\n"
    << "-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>\n"
    << "Currently only char, uchar and short are supported.\n"
    << "-id    \tImageDimension <2,3>\n"
    << "-alpha \t0.0 < alpha < 1.0\n"
    << "-beta  \t0.0 < beta < 1.0\n"
    << "-r0    \tInteger radius of window, dimension 0\n"
    << "-r1    \tInteger radius of window, dimension 1\n"
    << "[-r2]  \tInteger radius of window, dimension 2\n"
    << "[-LUT] \tUse Lookup-table <true, false>;\n"
    << "default = true; Faster, but requires more memory.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-alpha", "Alpha." );
  parser->MarkArgumentAsRequired( "-beta", "Beta." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  // Get arguments
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName;
  parser->GetCommandLineArgument( "-out", outputFileName );

  float alpha = 0.0f;
  parser->GetCommandLineArgument( "-alpha", alpha );

  float beta = 0.0f;
  parser->GetCommandLineArgument( "-beta", beta );

  bool lookUpTable = true;
  parser->GetCommandLineArgument( "-LUT", lookUpTable );

  std::vector<unsigned int> radius;
  parser->GetCommandLineArgument( "-r", radius );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsContrastEnhanceImageBase * filter = nullptr;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 2, unsigned char >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsContrastEnhanceImage< 3, unsigned char >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Alpha = alpha;
    filter->m_Beta = beta;
    filter->m_LookUpTable = lookUpTable;
    filter->m_Radius = radius;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

} // end function main
