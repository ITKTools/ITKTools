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
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "deformationfieldoperator.h"
#include "itkExceptionObject.h"
#include <itksys/SystemTools.hxx>


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileName, outputFileName, ops, \
    numberOfStreams, numberOfIterations, stopValue ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare PrintHelp. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
  }
  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string ops = "MAGNITUDE";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
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
  std::string ComponentType = "float";
  std::string PixelType = "VECTOR";
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = Dimension;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Checks. */
  if ( NumberOfComponents != Dimension )
  {
    std::cerr << "ERROR: The NumberOfComponents must equal the Dimension!" << std::endl;
    return 1;
  }
  if ( NumberOfComponents == 1 )
  {
    std::cerr << "Scalar images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( DeformationFieldOperator, float, 2 );
    run( DeformationFieldOperator, float, 3 );

    run( DeformationFieldOperator, double, 2 );
    run( DeformationFieldOperator, double, 3 );
  }
  catch ( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not "
      << "supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentType
      << " ; dimension = " << Dimension
      << std::endl;
    std::cerr << "Call \"pxdeformationfieldoperator --help\" to get a list "
      << "of supported images." << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText ="Usage: \
  pxdeformationfieldoperator\n \
    This program converts between deformations (displacement fields) \
  and transformations, and computes the magnitude or Jacobian of a \
  deformation field.\n \
    -in      inputFilename\n \
    [-out]   outputFilename; default: in + {operation}.mhd\n \
    [-ops]   operation; options: DEF2TRANS, TRANS2DEF, \
  MAGNITUDE, JACOBIAN, DEF2JAC, TRANS2JAC, INVERSE. default: MAGNITUDE\n \
             TRANS2JAC == JACOBIAN\n \
    [-s]     number of streams, default 1.\n \
    [-it]    number of iterations, for the iterative inversion, default 1, increase to get better results.\n \
    [-stop]  allowed error, default 0.0, increase to get faster convergence.\n \
  Supported: 2D, 3D, vector of floats or doubles, number of components \
  must equal number of dimensions.";
  return helpText;
} // end PrintHelp()
