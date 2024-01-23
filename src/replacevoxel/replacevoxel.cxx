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
 \brief This program replaces the value of a user specified voxel.

 \verbinclude replacevoxel.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "replacevoxel.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program replaces the value of a user specified voxel.\n"
    << "Usage:\n"
    << "pxreplacevoxel\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + VOXELREPLACED.mhd\n"
    << "  -vox     input voxel index\n"
    << "  -val     value that replaces the voxel\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int,\n"
    << "(unsigned) long, float, double.";

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

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-vox", "Voxel." );
  parser->MarkArgumentAsRequired( "-val", "Value." );

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
  outputFileName += "VOXELREPLACED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector< unsigned int > voxel;
  parser->GetCommandLineArgument( "-vox", voxel );

  double value = 0; // This is a required argument, but we must initialize it to prevent compiler warnings.
  parser->GetCommandLineArgument( "-val", value );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Check if the specified voxel-size has Dimension number of components. */
  if( voxel.size() != dim )
  {
    std::cerr << "ERROR: You should specify "
      << dim
      << " numbers with \"-vox\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsReplaceVoxelBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsReplaceVoxel< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReplaceVoxel< 2, float >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsReplaceVoxel< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsReplaceVoxel< 3, float >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Voxel = voxel;
    filter->m_Value = value;

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
