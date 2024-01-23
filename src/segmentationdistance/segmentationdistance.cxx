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
 \brief This program computes a spatial segmentation error map.

 \verbinclude segmentationdistance.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "segmentationdistance.h"
#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program computes a spatial segmentation error map.\n"
    << "Usage:\n"
    << "pxsegmentationdistance\n"
    << "  -in      inputFilename1 inputFileName2\n"
    << "  [-out]   outputFilename, default <in1>DISTANCE<in2>.mhd\n"
    << "  [-c]     Center of rotation, used to compute the spherical transform. In world coordinates.\n"
    << "  [-s]     samples [unsigned int]; maximum number of samples per pixel, used to do the spherical transform; default 20.\n"
    << "  [-t]     theta size; the size of the theta dimension. default: 180, which yields a spacing of 2 degrees.\n"
    << "  [-p]     phi size; the size of the phi dimension. default: 90, which yields a spacing of 2 degrees.\n"
    << "  [-car]   skip the polar transform and return two output images (outputFileNameDIST and outputFileNameEDGE): true or false; default = false\n"
    << "           The EDGE output image is an edge mask for inputfile2. The DIST output image contains the distance at each edge pixel to the first inputFile.\n"
    << "Supported: 3D short for inputImage1, and everything convertable to short.\n"
    << "           3D short for inputImage2, and everything convertable to short.";

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

  /** Get the input file names. */
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  if( (inputFileNames.size() != 2)  )
  {
    std::cout << "ERROR: You should specify two input images." << std::endl;
    return EXIT_FAILURE;
  }
  std::string inputFileName1 = inputFileNames[ 0 ];
  std::string inputFileName2 = inputFileNames[ 1 ];

  /** Get the outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2 =
      itksys::SystemTools::GetFilenameName(inputFileName2);

    std::string ops = "DISTANCE";

    /** compose outputfilename */
    outputFileName = part1 + ops + part2;
  }

  std::vector<double> manualcor;
  parser->GetCommandLineArgument( "-c", manualcor);

  unsigned int samples = 20;
  parser->GetCommandLineArgument( "-s", samples );

  unsigned int thetasize = 180;
  parser->GetCommandLineArgument( "-t", thetasize );

  unsigned int phisize = 90;
  parser->GetCommandLineArgument( "-p", phisize );

  std::string cartesianstr = "false";
  bool cartesianonly = false;
  parser->GetCommandLineArgument( "-car", cartesianstr);
  if( cartesianstr == "true" )
  {
    cartesianonly = true;
  }

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName1, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** The input images  are assumed to be short. Input images other
   * than short are supported, but automatically converted to short.
   * The output distance image is always float.
   */
  componentType = itk::ImageIOBase::FLOAT;

  /** Class that does the work. */
  ITKToolsSegmentationDistanceBase * filter = nullptr;

  try
  {
    if( !filter ) filter = ITKToolsSegmentationDistance< 2, float >::New( dim, componentType );

    // now call all possible template combinations.
#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsSegmentationDistance< 3, float >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName1 = inputFileName1;
    filter->m_InputFileName2 = inputFileName2;
    filter->m_OutputFileName = outputFileName;
    filter->m_Mancor = manualcor;
    filter->m_Samples = samples;
    filter->m_Thetasize = thetasize;
    filter->m_Phisize = phisize;
    filter->m_Cartesianonly = cartesianonly;

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
