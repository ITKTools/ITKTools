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
 \brief This program converts between deformations (displacement fields) and transformations, and computes the magnitude or Jacobian of a deformation field.

 \verbinclude deformationfieldoperator.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "deformationfieldoperator.h"

#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:" << std::endl
    << "pxdeformationfieldoperator\n"
    << "This program converts between deformations (displacement fields)\n"
    << "and transformations, and computes the magnitude or Jacobian of a\n"
    << "deformation field.\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename; default: in + {operation}.mhd\n"
    << "  [-ops]   operation, choose one of {DEF2TRANS, TRANS2DEF,\n"
    << "           MAGNITUDE, JACOBIAN, DEF2JAC, INVERSE}.\n"
    << "           default: MAGNITUDE\n"
    << "  [-s]     number of streams, default 1\n"
    << "  [-it]    number of iterations, for the iterative inversion, default 1, increase to get better results\n"
    << "  [-stop]  allowed error, default 0.0, increase to get faster convergence\n"
    << "Supported: 2D, 3D, vector of floats or doubles, number of components\n"
    << "must equal number of dimensions.";
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

  std::string ops = "MAGNITUDE";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if( outputFileName == "" )
  {
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName);
    std::string ext =
      itksys::SystemTools::GetFilenameLastExtension(inputFileName);
    outputFileName = part1 + ops + ext;
  }

  /** Support for streaming. */
  unsigned int numberOfStreams = 1;
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Parameters for the inversion. */
  unsigned int numberOfIterations = 1;
  parser->GetCommandLineArgument( "-it", numberOfIterations );

  double stopValue = 0.0;
  parser->GetCommandLineArgument( "-stop", stopValue );

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::IOPixelEnum::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Checks. */
  if( numberOfComponents != dim )
  {
    std::cerr << "ERROR: The NumberOfComponents must equal the Dimension!" << std::endl;
    return EXIT_FAILURE;
  }
  if( numberOfComponents == 1 )
  {
    std::cerr << "Scalar images are not supported!" << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsDeformationFieldOperatorBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsDeformationFieldOperator< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsDeformationFieldOperator< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsDeformationFieldOperator< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsDeformationFieldOperator< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Ops = ops;
    filter->m_NumberOfStreams = numberOfStreams;
    filter->m_NumberOfIterations = numberOfIterations;
    filter->m_StopValue = stopValue;

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
