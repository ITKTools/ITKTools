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
 \brief Create a summary of segmentation overlap in two images.

 \verbinclude computeoverlapsummary.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "computeoverlapsummary.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Computes overlap measures between the set same set of labels of pixels of two images.\n"
    << "Usage:\n"
    << "pxcomputeoverlap summary\n"
    << "  -in1    Filename of first input image (Source Image)\n"
    << "  -in2    Filename of second input image (Target Image)\n"
    << "  -out    Filename to write the results to\n"
    << "  -seperator    Seperator to use in csv file; default '\\t'\n"
    << "The results file contains:\n"
    << "  Target overlap, Union(jaccard) overlap, Mean(dice) overlap, Volume similarity, False negative error, False positive error\n"
    << "Background is assumed to be 0. \n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short.";

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

  parser->MarkArgumentAsRequired( "-in1", "Filename of first input image (SourceImage)." );
  parser->MarkArgumentAsRequired( "-in2", "Filename of second input image (Target Image)." );
  parser->MarkArgumentAsRequired( "-out", "Filename to write the results to." );

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
  std::string inputFileName1 = "";
  parser->GetCommandLineArgument( "-in1", inputFileName1 );

  std::string inputFileName2 = "";
  parser->GetCommandLineArgument( "-in2", inputFileName2 );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string seperator("\t");
  parser->GetCommandLineArgument( "-seperator", seperator );

  if( seperator.compare("\\t") == 0 )
  {
    seperator = "\t";
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName1, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsComputeOverlapSummaryBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 2, unsigned short >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsComputeOverlapSummary< 3, unsigned short >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName1 = inputFileName1;
    filter->m_InputFileName2 = inputFileName2;
    filter->m_OutputFileName = outputFileName;
    filter->m_Seperator      = seperator;

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

