/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria, Hakim Achterberg. 2011.
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
 \brief Threshold an image.

 \verbinclude thresholdimage.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "meanstdimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program creates a mean and standard deviation image of a set of images.\n"
    << "Usage:\n"
    << "pxmeanstdimage\n"
    << "  -in        list of inputFilenames\n"
	<< "  -inMask    list of inputMaskFilenames\n"
    << "  [-outmean] outputFilename for mean image; always written as float\n"
    << "  [-outstd]  outputFilename for standard deviation image; always written as float,\n"
	<< "  [-popstd]  population standard deviation flag; if provided, use population standard deviation\n"
	<< "             rather than sample standard deviation (divide by N instead of N-1)\n"
    << "  [-z]       compression flag; if provided, the output image is compressed\n"
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

  parser->MarkArgumentAsRequired( "-in", "The input filenames." );

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
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );
  
  std::vector<std::string> inputMaskFileNames;
  parser->GetCommandLineArgument( "-inMask", inputMaskFileNames );

  std::string outputFileNameMean = "";
  parser->GetCommandLineArgument( "-outmean", outputFileNameMean );
  bool retoutmean = parser->GetCommandLineArgument( "-outmean", outputFileNameMean );

  std::string outputFileNameStd = "";
  parser->GetCommandLineArgument( "-outstd", outputFileNameStd );
  bool retoutstd  = parser->GetCommandLineArgument( "-outstd", outputFileNameStd );

  /** Use population standard deviation */
  const bool usePopulationStd = parser->ArgumentExists( "-popstd" );

  /** Use compression */
  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ], pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Input check for number of masks */
  if (inputFileNames.size() != inputMaskFileNames.size() && inputMaskFileNames.size() > 0)
  {
	  std::cerr << "ERROR: the number of masks has to match the number of input images"  << std::endl;
	  return EXIT_FAILURE;
  }  

  /** Class that does the work. */
  ITKToolsMeanStdImageBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsMeanStdImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsMeanStdImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsMeanStdImage< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileNames = inputFileNames;
	filter->m_InputMaskFileNames = inputMaskFileNames;
    filter->m_OutputFileNameMean= outputFileNameMean;
    filter->m_OutputFileNameStd = outputFileNameStd;
    filter->m_CalcMean = retoutmean;
    filter->m_CalcStd = retoutstd;
	filter->m_UsePopulationStd = usePopulationStd;
	filter->m_UseCompression = useCompression;

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
