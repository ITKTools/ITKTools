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
 \brief Rescale the intensities of an image.

 \verbinclude rescaleintensityimagefilter.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "rescaleintensityimagefilter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxrescaleintensityimagefilter\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + INTENSITYRESCALED.mhd\n"
    << "  [-mm]    minimum maximum, default: range of pixeltype\n"
    << "  [-mv]    mean variance, default: 0.0 1.0\n"
    << "  [-opct]  pixel type of input and output images;\n"
    << "           default: automatically determined from the first input image.\n"
    << "Either \"-mm\" or \"-mv\" need to be specified.\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, float.\n"
    << "When applied to vector images, this program performs the operation on each channel separately.";

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
  exactlyOneArguments.push_back( "-mm" );
  exactlyOneArguments.push_back( "-mv" );
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
  outputFileName += "INTENSITYRESCALED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<double> extrema( 2, 0.0 );
  bool retmm = parser->GetCommandLineArgument( "-mm", extrema );

  std::vector<double> meanvariance( 2, 0.0 );
  meanvariance[ 1 ] = 1.0;
  bool retmv = parser->GetCommandLineArgument( "-mv", meanvariance );

  /** Check if the extrema are given (correctly). */
  if( retmm )
  {
    if( extrema.size() != 2 )
    {
      std::cerr << "ERROR: You should specify \"-mm\" with two values." << std::endl;
      return EXIT_FAILURE;
    }
    if( extrema[ 1 ] <= extrema[ 0 ] )
    {
      std::cerr << "ERROR: You should specify \"-mm\" with two values:" << std::endl;
      std::cerr << "minimum maximum, where it should hold that maximum > minimum." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check if the mean and variance are given correctly. */
  if( retmv )
  {
    if( meanvariance.size() != 2 )
    {
      std::cerr << "ERROR: You should specify \"-mv\" with two values." << std::endl;
      return EXIT_FAILURE;
    }
    if( meanvariance[ 1 ] <= 1e-5 )
    {
      std::cerr << "ERROR: The variance should be strictly positive." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check which option is selected. */
  bool valuesAreExtrema = true;
  if( retmv ) valuesAreExtrema = false;

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** If the option -mv is used then output is float. */
  if( retmv )
  {
    componentType = itk::ImageIOBase::FLOAT;
  }

  /** Let the user overrule this. */
  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );
  if ( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Get the values. */
  std::vector<double> values;
  if( valuesAreExtrema )
  {
    values = extrema;
  }
  else
  {
    values = meanvariance;
  }

  /** Class that does the work. */
  ITKToolsRescaleIntensityImageFilterBase * filter = 0;

  try
  {
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 2, float >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsRescaleIntensityFilter< 3, float >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Values = values;
    filter->m_ValuesAreExtrema = valuesAreExtrema;

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
