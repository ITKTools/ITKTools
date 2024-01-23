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
 \brief This program computes filter features based on the gray-level co-occurrence matrix (GLCM).

 \verbinclude filter.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "texture.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxtexture\n"
    << "This program computes filter features based on the gray-level co-occurrence matrix (GLCM).\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputDirectory, default equal to the inputFilename directory\n"
    << "  [-r]     the radius of the neighborhood on which to construct the GLCM, default 3\n"
    << "  [-os]    the desired offset scales to compute the GLCM, default 1, but can be e.g. 1 2 4\n"
    << "  [-b]     the number of bins of the GLCM, default 128\n"
    << "  [-noo]   the number of filter feature outputs, default all 8\n"
    << "  [-opct]  output pixel component type, default float\n"
    << "Supported: 2D, 3D, any input image type, float or double output type.";

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

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string base = itksys::SystemTools::GetFilenamePath( inputFileName );
  if( base != "" ) base = base + "/";
  std::string outputDirectory = base;
  parser->GetCommandLineArgument( "-out", outputDirectory );
  bool endslash = itksys::SystemTools::StringEndsWith( outputDirectory.c_str(), "/" );
  if( !endslash ) outputDirectory += "/";

  unsigned int neighborhoodRadius = 3;
  parser->GetCommandLineArgument( "-r", neighborhoodRadius );

  std::vector<unsigned int> offsetScales( 1, 1 );
  parser->GetCommandLineArgument( "-os", offsetScales );

  unsigned int numberOfBins = 128;
  parser->GetCommandLineArgument( "-b", numberOfBins );

  unsigned int numberOfOutputs = 8;
  parser->GetCommandLineArgument( "-noo", numberOfOutputs );

  std::string componentTypeOutString = "float";
  parser->GetCommandLineArgument( "-opct", componentTypeOutString );

  /** Check that numberOfOutputs <= 8. */
  if( numberOfOutputs > 8 )
  {
    std::cerr << "ERROR: The maximum number of outputs is 8. You requested "
      << numberOfOutputs << "." << std::endl;
    return EXIT_FAILURE;
  }

  /** Threads. */
  unsigned int maximumNumberOfThreads
    = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(
    maximumNumberOfThreads );

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

  /** Class that does the work. */
  ITKToolsTextureBase * filter = nullptr;

  /** Input images are read in as float, always. The default output is float,
   * but can be overridden by specifying -opct in the command line.
   */
  componentType = itk::ImageIOBase::FLOAT;
  itk::ImageIOBase::IOComponentType outputComponentType
    = itk::ImageIOBase::GetComponentTypeFromString( componentTypeOutString );

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsTexture< 2, float, float >::New( dim, componentType, outputComponentType );
    if( !filter ) filter = ITKToolsTexture< 2, float, double >::New( dim, componentType, outputComponentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsTexture< 3, float, float >::New( dim, componentType, outputComponentType );
    if( !filter ) filter = ITKToolsTexture< 3, float, double >::New( dim, componentType, outputComponentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType, outputComponentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputDirectory = outputDirectory;
    filter->m_NeighborhoodRadius = neighborhoodRadius;
    filter->m_OffsetScales = offsetScales;
    filter->m_NumberOfBins = numberOfBins;
    filter->m_NumberOfOutputs = numberOfOutputs;

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
