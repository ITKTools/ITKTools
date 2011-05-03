#include "itkCommandLineArgumentParser.h"
#include <string>
#include <vector>

int main( int argc, char** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Optionally set help text. */
  parser->SetProgramHelpText("Example help text.");

  /** Set any required arguments. */
  parser->MarkArgumentAsRequired( "-inputs", "The input filenames." );
  parser->MarkArgumentAsRequired( "-output", "The output filename." );
  parser->MarkArgumentAsRequired( "-myint", "An int." );

  /** Set any lists of required arguments from which one must be selected. */
  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-this");
  exactlyOneArguments.push_back("-that");
  parser->MarkExactlyOneOfArgumentsAsRequired(exactlyOneArguments);

  /** Validate the input arguments. */
  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
  }
  
  /** Get a list of string arguments with key -in. */
  std::vector<std::string> inputFileNames; // no default
  parser->GetCommandLineArgument( "-in", inputFileNames );
  
  for ( std::size_t i = 0; i < inputFileNames.size(); i++ )
  {
    std::cout << " " << inputFileNames[ i ];
  }
  std::cout << std::endl;

  /** Get a single string argument with key -out. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  std::cout << "-out: " << outputFileName << std::endl;

  /** A single int argument with key -myint. */
  short myint = 3; // default
  parser->GetCommandLineArgument( "-myint", myint );
  std::cout << "-myint: " << myint << std::endl;

  /** A single float argument with key -myfloat. */
  float myfloat = 4.1; // no default
  parser->GetCommandLineArgument( "-myfloat", myfloat );
  std::cout << "-myfloat: " << myfloat << std::endl;

	return EXIT_SUCCESS;

} // end main
