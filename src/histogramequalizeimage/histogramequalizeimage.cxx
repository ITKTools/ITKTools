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
 \brief Equalize the histogram of an image.
 
 \verbinclude histogramequalizeimage.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "histogramequalizeimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program applies histogram equalization to an image.\n"
    << "Works as described by Maintz, Introduction to Image Processing.\n"
    << "Usage:\n"
    << "pxhistogramequalizeimage\n"
    << "  -in      inputFileName\n"
    << "  -out     outputFileName\n"
    << "  -[mask]  maskFileName\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

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

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  /** Determine image properties. */
  std::string componentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int numberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    componentTypeIn,
    Dimension,
    numberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  itk::ImageIOBase::IOComponentType componentType
    = itk::ImageIOBase::GetComponentTypeFromString( componentTypeIn );

  /** Check for vector images. */
  if ( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** Class that does the work */
  ITKToolsHistogramEqualizeImageBase * equalizer = NULL;

  try
  {
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< char, 2 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned char, 2 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< short, 2 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned short, 2 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< int, 2 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned int, 2 >::New( componentType, Dimension );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< char, 3 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned char, 3 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< short, 3 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned short, 3 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< int, 3 >::New( componentType, Dimension );
    if( !equalizer ) equalizer = ITKToolsHistogramEqualizeImage< unsigned int, 3 >::New( componentType, Dimension );
#endif
    if( !equalizer )
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentTypeIn
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    equalizer->m_InputFileName = inputFileName;
    equalizer->m_OutputFileName = outputFileName;
    equalizer->m_MaskFileName = maskFileName;

    equalizer->Run();

    delete equalizer;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Caught ITK exception: " << excp << std::endl;
    delete equalizer;
    return 1;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end function main
