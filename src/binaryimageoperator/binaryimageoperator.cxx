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
 \brief Apply a binary operator to a pair of images.
 
 \verbinclude binaryimageoperator.help
 */

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "BinaryImageOperatorMainHelper.h"
#include "BinaryImageOperatorHelper.h"

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-ops", "The operation to perform." );

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

  // You must specify two input files.
  if ( inputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify two input file names." << std::endl;
    return 1;
  }
  
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string argument = "0";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  std::string opct = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", opct );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Create outputFileName. */
  if ( outputFileName == "" )
  {
    CreateOutputFileName( inputFileNames, outputFileName, ops, argument );
  }

  /** Determine image properties. */
  std::string ComponentTypeIn1 = "long";
  std::string ComponentTypeIn2 = "long";
  std::string ComponentTypeOut = "long";
  unsigned int inputDimension = 2;
  int retdip = DetermineImageProperties( inputFileNames,
    ComponentTypeIn1, ComponentTypeIn2, ComponentTypeOut,
    inputDimension );
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
  if ( !TypeIsInteger( opct ) ) ComponentTypeIn1 = ComponentTypeIn2 = "double";
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
    run( BinaryImageOperator, long, long, char, 2 );
    run( BinaryImageOperator, long, long, unsigned char, 2 );
    run( BinaryImageOperator, long, long, short, 2 );
    run( BinaryImageOperator, long, long, unsigned short, 2 );
    run( BinaryImageOperator, long, long, int, 2 );
    run( BinaryImageOperator, long, long, unsigned int, 2 );
    run( BinaryImageOperator, long, long, long, 2 );
    run( BinaryImageOperator, long, long, unsigned long, 2 );
    run( BinaryImageOperator, double, double, float, 2 );
    run( BinaryImageOperator, double, double, double, 2 );

    run( BinaryImageOperator, long, long, char, 3 );
    run( BinaryImageOperator, long, long, unsigned char, 3 );
    run( BinaryImageOperator, long, long, short, 3 );
    run( BinaryImageOperator, long, long, unsigned short, 3 );
    run( BinaryImageOperator, long, long, int, 3 );
    run( BinaryImageOperator, long, long, unsigned int, 3 );
    run( BinaryImageOperator, long, long, long, 3 );
    run( BinaryImageOperator, long, long, unsigned long, 3 );
    run( BinaryImageOperator, double, double, float, 3 );
    run( BinaryImageOperator, double, double, double, 3 );

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
