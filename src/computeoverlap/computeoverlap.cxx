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
 \brief Compute the overlap of two images.

 \verbinclude computeoverlap.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"

#include "ComputeOverlapOld.h"
//#include "ComputeOverlap2.h"
#include "ComputeOverlap3.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxcomputeoverlap\n"
    << "This program computes the overlap of two images.\n"
    << "By default the overlap of nonzero regions is computed.\n"
    << "Masks of a valid region are also taken into account.\n"
    << "If the images are not binary, you can specify threshold values.\n"
    << "The results is computed as:\n"
    << "   2 * L1( (im1 AND mask2) AND (im2 AND mask1) )\n"
    << "  ----------------------------------------------\n"
    << "       L1(im1 AND mask2) + L1(im2 AND mask1)\n\n"
    << "  -in      inputFilename1 inputFilename2" << std::endl
    << "  [-mask1] maskFilename1" << std::endl
    << "  [-mask2] maskFilename2" << std::endl
    << "  [-t1]    threshold1" << std::endl
    << "  [-t2]    threshold2" << std::endl
    << "  [-l]     alternative implementation using label values" << std::endl
    << "          the overlap of exactly corresponding labels is computed" << std::endl
    << "           if \"-l\" is specified with no arguments, all labels in im1 are used," << std::endl
    << "           otherwise (e.g. \"-l 1 6 19\") the specified labels are used." << std::endl
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  /** Get arguments. */
  std::vector<std::string> inputFileNames;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

  parser->MarkArgumentAsRequired( "-in", "Two input filenames." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  std::string maskFileName1 = "";
  parser->GetCommandLineArgument( "-mask1", maskFileName1 );
  std::string maskFileName2 = "";
  parser->GetCommandLineArgument( "-mask2", maskFileName2 );

  unsigned int t1 = 0;
  parser->GetCommandLineArgument( "-t1", t1 );
  unsigned int t2 = 0;
  parser->GetCommandLineArgument( "-t2", t2 );

  bool retlabel = parser->ArgumentExists( "-l" ); // default all labels
  std::vector<unsigned int> labels( 0 );
  parser->GetCommandLineArgument( "-l", labels );

  /** Checks. */
  if( !retin || inputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify two input file names with \"-in\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ], pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Select overlap compute filter. */
  if( retlabel )
  {
    /** Class that does the work. */
    ITKToolsComputeOverlap3Base * filter3 = 0;

    try
    {
      // now call all possible template combinations.
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 2, char >::New( dim, componentType );
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 2, short >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 3, char >::New( dim, componentType );
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 3, unsigned char >::New( dim, componentType );
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 3, short >::New( dim, componentType );
      if( !filter3 ) filter3 = ITKToolsComputeOverlap3< 3, unsigned short >::New( dim, componentType );
#endif
      /** Check if filter was instantiated. */
      bool supported = itktools::IsFilterSupportedCheck( filter3, dim, componentType );
      if( !supported ) return EXIT_FAILURE;

      /** Set the filter arguments. */
      filter3->m_InputFileNames = inputFileNames;
      filter3->m_Labels = labels;

      filter3->Run();

      delete filter3;
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Caught ITK exception: " << excp << std::endl;
      delete filter3;
      return EXIT_FAILURE;
    }

  }
  else
  {
    /** Class that does the work. */
    ITKToolsComputeOverlapOldBase * filterOld = 0;

    try
    {
      // now call all possible template combinations.
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 2, char >::New( dim, componentType );
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 2, short >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 3, char >::New( dim, componentType );
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 3, unsigned char >::New( dim, componentType );
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 3, short >::New( dim, componentType );
      if( !filterOld ) filterOld = ITKToolsComputeOverlapOld< 3, unsigned short >::New( dim, componentType );
#endif
      /** Check if filter was instantiated. */
      bool supported = itktools::IsFilterSupportedCheck( filterOld, dim, componentType );
      if( !supported ) return EXIT_FAILURE;

      /** Set the filter arguments. */
      filterOld->m_InputFileNames = inputFileNames;
      filterOld->m_MaskFileName1 = maskFileName1;
      filterOld->m_MaskFileName2 = maskFileName2;
      filterOld->m_T1 = t1;
      filterOld->m_T2 = t2;

      filterOld->Run();

      delete filterOld;
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
      delete filterOld;
      return EXIT_FAILURE;
    }
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
