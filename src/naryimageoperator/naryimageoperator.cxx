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
 \brief N-ary image operator.
 
 \verbinclude naryimageoperator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "NaryImageOperatorMainHelper.h"
#include "NaryImageOperatorHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Performs n-ary operations on multiple (n) images." << std::endl
  << "Usage:\npxnaryimageoperator" << std::endl
  << "  -in      inputFilenames, at least 2" << std::endl
  << "  -out     outputFilename" << std::endl
  << "  -ops     n-ary operator of the following form:" << std::endl
  << "           {+,-,*,/,^,%}" << std::endl
  << "           notation:" << std::endl
  << "             {ADDITION, MINUS, TIMES, DIVIDE," << std::endl
  << "             MEAN," << std::endl
  << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE," << std::endl
  << "             NARYMAGNITUDE }" << std::endl
  << "           notation examples:" << std::endl
  << "             MINUS = I_0 - I_1 - ... - I_n " << std::endl
  << "             ABSDIFF = |I_0 - I_1 - ... - I_n|" << std::endl
  << "             MIN = min( I_0, ..., I_n )" << std::endl
  << "             MAGNITUDE = sqrt( I_0 * I_0 + ... + I_n * I_n )" << std::endl
//   std::cout << "  [-arg]   argument, necessary for some ops\n"
//             << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
//             << "             MASK[NEG]: background value, e.g. 0." << std::endl;
  << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
  << "  [-s]     number of streams, default equals number of inputs." << std::endl
  << "  [-opct]  output component type, by default the largest of the two input images" << std::endl
  << "             choose one of: {[unsigned_]{char,short,int,long},float,double}" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;

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
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-ops", "Operation." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string argument = "0";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  std::string opct = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", opct );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Support for streaming. */
  unsigned int numberOfStreams = inputFileNames.size();
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** You should specify at least two input files. */
  if ( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two input file names." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "long";
  std::string ComponentTypeOut = "long";
  unsigned int inputDimension = 2;
  int retdip = DetermineImageProperties( inputFileNames,
    ComponentTypeIn, ComponentTypeOut, inputDimension );
  if ( retdip ) return 1;

  /** Let the user override the output component type. */
  if ( retopct )
  {
    if ( !CheckForValidComponentType( opct ) )
    {
      std::cerr << "ERROR: the you specified a wrong opct." << std::endl;
      return 1;
    }
    ComponentTypeOut = opct;
    ReplaceUnderscoreWithSpace( ComponentTypeOut );
  if ( !TypeIsInteger( opct ) ) ComponentTypeIn = "double";
  }

  /** Check if a valid operator is given. */
  std::string opsCopy = ops;
  int retCO  = CheckOperator( ops );
  if ( retCO ) return retCO;

  /** For certain ops an argument is mandatory. */
  bool retCOA = CheckOperatorAndArgument( ops, argument, retarg );
  if ( !retCOA ) return 1;

  /** Run the program. */
  bool supported = false;
  try
  {
    run( NaryImageOperator, long, char, 2 );
    run( NaryImageOperator, long, unsigned char, 2 );
    run( NaryImageOperator, long, short, 2 );
    run( NaryImageOperator, long, unsigned short, 2 );
    run( NaryImageOperator, long, int, 2 );
    run( NaryImageOperator, long, unsigned int, 2 );
    run( NaryImageOperator, long, long, 2 );
    run( NaryImageOperator, long, unsigned long, 2 );
    run( NaryImageOperator, double, float, 2 );
    run( NaryImageOperator, double, double, 2 );

    run( NaryImageOperator, long, char, 3 );
    run( NaryImageOperator, long, unsigned char, 3 );
    run( NaryImageOperator, long, short, 3 );
    run( NaryImageOperator, long, unsigned short, 3 );
    run( NaryImageOperator, long, int, 3 );
    run( NaryImageOperator, long, unsigned int, 3 );
    run( NaryImageOperator, long, long, 3 );
    run( NaryImageOperator, long, unsigned long, 3 );
    run( NaryImageOperator, double, float, 3 );
    run( NaryImageOperator, double, double, 3 );

  } // end try
  catch ( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentTypeOut
      << " ; dimension = " << inputDimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
