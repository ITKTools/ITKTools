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
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "LogicalImageOperatorHelper.h"

/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Logical operations on one or two images." << std::endl
    << "NOTE: The output of this filter is an image with pixels of values 0 and 1." << std::endl
    << "An appropriate scaling must be performed either manually (with pxrescaleintensityimagefilter)" << std::endl
    << "or with the application used to view the image." << std::endl << std::endl
    << "In the case of a vector image, this is a componentwise logical operator." << std::endl
    << "Usage:" << std::endl << "pxlogicalimageoperator" << std::endl
    << "  -in      inputFilename1 [inputFilename2]" << std::endl
    << "  [-out]   outputFilename, default in1 + <ops> + in2 + .mhd" << std::endl
    << "  -ops     LogicalOperator of the following form:" << std::endl
    << "             [!]( ([!] A) [{&,|,^} ([!] B])] )" << std::endl
    << "           notation:" << std::endl
    << "             [NOT_][NOT][{AND,OR,XOR}[NOT]]" << std::endl
    << "           notation examples:" << std::endl
    << "             ANDNOT = A & (!B)" << std::endl
    << "             NOTAND = (!A) & B" << std::endl
    << "             NOTANDNOT = (!A) & (!B)" << std::endl
    << "             NOT_NOTANDNOT = !( (!A) & (!B) )" << std::endl
    << "             NOT_AND = !(A & B)" << std::endl
    << "             OR = A | B" << std::endl
    << "             XOR = A ^ B" << std::endl
    << "             NOT = !A " << std::endl
    << "             NOT_NOT = A" << std::endl
    << "           Internally this expression is simplified." << std::endl
    << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
    << "  [-arg]   argument, necessary for some ops" << std::endl
    << "  [-dim]   dimension, default: automatically determined from inputimage1" << std::endl
    << "  [-pt]    pixelType, default: automatically determined from inputimage1" << std::endl
    << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;

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
  if ( ( inputFileNames.size() == 2 ) & (ops != "NOT") )
  {
    inputFileName2 = inputFileNames[ 1 ];
  }
  if ( ops == "EQUAL" && inputFileNames.size() > 1 && !retarg )
  {
    std::cerr << "ERROR: The operator \"EQUAL\" expects 1 input image and a \"-arg\"." << std::endl;
    return 1;
  }
  
  bool unary = false;
  if ( ops.compare("EQUAL") == 0 || ops.compare("NOT") == 0 )
  {
    unary = true;
  }

  /** outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1
      = itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
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

  /** Class that does the work */
  ITKToolsLogicalImageOperatorBase * logicalImageOperator = NULL; 

  unsigned int imageDimension = 0;
  itktools::GetImageDimension( inputFileName1, imageDimension );

  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName1 );
    
  /** NB: do not add floating point support, since logical operators are
   * not defined on those types */
    
  try
  {    
    // now call all possible template combinations.
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 2, unsigned char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 2, char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 2, unsigned short >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 2, unsigned short >::New( imageDimension, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 3, unsigned char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 3, char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 3, unsigned short >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = ITKToolsLogicalImageOperator< 3, unsigned short >::New( imageDimension, componentType );
#endif
    if (!logicalImageOperator) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << imageDimension << std::endl
        << " pixel type = " << componentType << std::endl
        << std::endl;
      return 1;
    }

    logicalImageOperator->m_OutputFileName = outputFileName;
    logicalImageOperator->m_InputFileName1 = inputFileName1;
    logicalImageOperator->m_InputFileName2 = inputFileName2;
    logicalImageOperator->m_Ops = ops;
    logicalImageOperator->m_UseCompression = useCompression;
    logicalImageOperator->m_Argument = argument;
    logicalImageOperator->m_Unary = unary;
    
    logicalImageOperator->Run();
    
    delete logicalImageOperator;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete logicalImageOperator;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main
