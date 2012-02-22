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
 \brief Compute statistics on an image. For vector images, the magnitude is used.

 This program determines the minimum, maximum,
 mean, sigma, variance, and sum of an image, or its magnitude/jacobian.
 \verbinclude statisticsonimage.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "statisticsonimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
  << "Compute statistics on an image. For vector images, the magnitude is used." << std::endl
  << "Usage:" << std::endl
  << "pxstatisticsonimage" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFileName for histogram;" << std::endl
  << "           if omitted, no histogram is written; default: <empty>" << std::endl
  << "  [-mask]  MaskFileName, mask should have the same size as the input image" << std::endl
  << "           and be of pixeltype (convertable to) unsigned char," << std::endl
  << "           1 = within mask, 0 = outside mask;" << std::endl
  << "  [-b]     NumberOfBins to use for histogram, default: 100;" << std::endl
  << "           for an accurate estimate of median and quartiles" << std::endl
  << "           for integer images, choose the number of bins" << std::endl
  << "           much larger (~100x) than the number of gray values." << std::endl
  << "           if equal 0, then the intensity range (max - min) is chosen." << std::endl
  << "  [-s]     select which to compute {arithmetic, geometric, histogram}, default all;" << std::endl
  << "Supported: 2D, 3D, 4D, float, (unsigned) short, (unsigned) char, 1, 2 or 3 components per pixel." << std::endl
  << "For 4D, only 1 or 4 components per pixel are supported.";

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

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string histogramOutputFileName = "";
  parser->GetCommandLineArgument( "-out", histogramOutputFileName );

  unsigned int numberOfBins = 100;
  parser->GetCommandLineArgument( "-b", numberOfBins );

  std::string select = "";
  bool rets = parser->GetCommandLineArgument( "-s", select );

  /** Check selection. */
  if( rets && ( select != "arithmetic" && select != "geometric"
    && select != "histogram" ) )
  {
    std::cerr << "ERROR: -s should be one of {arithmetic, geometric, histogram}"
      << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  std::cout << "The input image has the following properties:" << std::endl;
  std::cout << "\tPixelType:          " << itk::ImageIOBase::GetPixelTypeAsString( pixelType ) << std::endl;
  std::cout << "\tComponentType:      " << itk::ImageIOBase::GetComponentTypeAsString( componentType ) << std::endl;
  std::cout << "\tDimension:          " << dim << std::endl;
  std::cout << "\tNumberOfComponents: " << numberOfComponents << std::endl;

  /** Force images to sneaky be converted to float. */
  componentType = itk::ImageIOBase::FLOAT;

  /** Class that does the work. */
  ITKToolsStatisticsOnImageBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsStatisticsOnImage< 2, 1, float >::New( dim, numberOfComponents, componentType );
    if( !filter ) filter = ITKToolsStatisticsOnImage< 2, 2, float >::New( dim, numberOfComponents, componentType );
    if( !filter ) filter = ITKToolsStatisticsOnImage< 2, 3, float >::New( dim, numberOfComponents, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsStatisticsOnImage< 3, 1, float >::New( dim, numberOfComponents, componentType );
    if( !filter ) filter = ITKToolsStatisticsOnImage< 3, 2, float >::New( dim, numberOfComponents, componentType );
    if( !filter ) filter = ITKToolsStatisticsOnImage< 3, 3, float >::New( dim, numberOfComponents, componentType );
#endif
#ifdef ITKTOOLS_4D_SUPPORT
    if( !filter ) filter = ITKToolsStatisticsOnImage< 4, 1, float >::New( dim, numberOfComponents, componentType );
    if( !filter ) filter = ITKToolsStatisticsOnImage< 4, 4, float >::New( dim, numberOfComponents, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_MaskFileName = maskFileName;
    filter->m_HistogramOutputFileName = histogramOutputFileName;
    filter->m_NumberOfBins = numberOfBins;
    filter->m_Select = select;

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
