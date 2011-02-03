#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "NaryImageOperatorMainHelper.h"
#include "NaryImageOperatorHelper.h"

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 5 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::vector<std::string> inputFileNames;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileName = "";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "";
  bool retops = parser->GetCommandLineArgument( "-ops", ops );

  std::string argument = "0";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  std::string opct = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", opct );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Support for streaming. */
  unsigned int numberOfStreams = inputFileNames.size();
  bool rets = parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( !retout )
  {
    std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
    return 1;
  }
  if ( !retops )
  {
    std::cerr << "ERROR: You should specify \"-ops\"." << std::endl;
    return 1;
  }

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

