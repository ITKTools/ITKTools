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
  parser->SetProgramHelpText(PrintHelp());
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-ops", "The operation to perform." );

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

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
  }

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

