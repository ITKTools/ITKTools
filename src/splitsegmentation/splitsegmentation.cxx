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
 \brief Split a segmentation in subparts of equal volume.

 \verbinclude splitsegmentation.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "splitsegmentation.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxsplitsegmentation\n"
    << "  -in      inputFilename\n"
    << "  -out     outputFilename\n"
    << "  [-nz]    number of splits in the z direction, default 3\n"
    << "  [-ny]    number of splits in the y direction, default 2\n"
    << "  [-l]     labels for the splitted volumes\n"
    << "Supported: 3D, (unsigned) char, (unsigned) short, (unsigned) int.\n";

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

  unsigned int numSplitsZ = 3;
  bool retnz = parser->GetCommandLineArgument( "-nz", numSplitsZ );

  unsigned int numSplitsY = 2;
  bool retny = parser->GetCommandLineArgument( "-ny", numSplitsY );

  std::vector<long> labels( numSplitsZ * numSplitsY, 1 );
  for( unsigned int i = 0; i < numSplitsZ * numSplitsY; ++i ) labels[ i ] = i + 1;
  bool retl = parser->GetCommandLineArgument( "-l", labels );

  /** Clamp. */
  if( numSplitsZ < 1 )
  {
    std::cerr << "WARNING: \"-nz\" is clamped t0 1." << std::endl;
    numSplitsZ = 1;
  }
  if( numSplitsY < 1 )
  {
    std::cerr << "WARNING: \"-ny\" is clamped t0 1." << std::endl;
    numSplitsY = 1;
  }

  if( labels.size() != numSplitsZ * numSplitsY )
  {
    std::cerr << "ERROR: You should provide " << numSplitsZ * numSplitsY << " labels." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;
  if( dim != 3 )
  {
    std::cerr << "ERROR: only 3D images are supported." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsSplitSegmentationFilterBase * filter = 0;

  try
  {
#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsSplitSegmentationFilter< 3, unsigned int >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_NumberOfSplitsZ = numSplitsZ;
    filter->m_NumberOfSplitsY = numSplitsY;
    filter->m_ChunkLabels = labels;

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
