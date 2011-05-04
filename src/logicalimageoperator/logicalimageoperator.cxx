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
 \brief Perform a logical operation on an image.
 
 \verbinclude logicalimageoperator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "LogicalImageOperatorHelper.h"


int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText("No help provided!");

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
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string ops = "AND";
  parser->GetCommandLineArgument( "-ops", ops );

  double argument = 0.0;
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Check if the required arguments are given. */
  if ( inputFileNames.size() != 2 && ops != "NOT" && ops != "NOT_NOT" && ops != "EQUAL" )
  {
    std::cerr << "ERROR: You should specify two input images." << std::endl;
    return 1;
  }

  std::string inputFileName1 = inputFileNames[ 0 ];
  std::string inputFileName2 = "";
  if( (inputFileNames.size() == 2) & (ops != "NOT") )
  {
    inputFileName2 = inputFileNames[ 1 ];
  }
  if ( ops == "EQUAL" && inputFileNames.size() > 1 && !retarg )
  {
    std::cerr << "ERROR: The operator \"EQUAL\" expects 1 input image and a \"-arg\"." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName1,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );

  if ( retgip != 0 )
  {
    return 1;
  }
  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** Let the user overrule this */
  bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if ( retdim | retpt )
  {
    std::cout << "The user has overruled this by specifying -pt and/or -dim:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

  /** outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2;
    if ( inputFileName2 != "" )
    {
      part2 = itksys::SystemTools::GetFilenameName(inputFileName2);
    }
    else
    {
      part2 = "";
      part2 += itksys::SystemTools::GetFilenameLastExtension(inputFileName1);
    }
    /** compose outputfilename */
    outputFileName = part1 + ops + part2;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    /** NB: do not add floating point support, since logical operators are
     * not defined on those types */
    run( LogicalImageOperator, unsigned char, 2 );
    run( LogicalImageOperator, unsigned char, 3 );
    run( LogicalImageOperator, char, 2 );
    run( LogicalImageOperator, char, 3 );
    run( LogicalImageOperator, unsigned short, 2 );
    run( LogicalImageOperator, unsigned short, 3 );
    run( LogicalImageOperator, short, 2 );
    run( LogicalImageOperator, short, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
