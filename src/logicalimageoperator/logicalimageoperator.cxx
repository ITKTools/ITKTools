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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "logicalimageoperator.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Logical operations on one or two images.\n"
    << "NOTE: The output of this filter is an image with pixels of values 0 and 1.\n"
    << "An appropriate scaling must be performed either manually (with pxrescaleintensityimagefilter)\n"
    << "or with the application used to view the image.\n"
    << "In the case of a vector image, this is a componentwise logical operator.\n"
    << "Usage:" << std::endl << "pxlogicalimageoperator\n"
    << "  -in      inputFilename1 [inputFilename2]\n"
    << "  [-out]   outputFilename, default in1 + <ops> + in2 + .mhd\n"
    << "  -ops     LogicalOperator of the following form:\n"
    << "             [!]( ([!] A) [{&,|,^} ([!] B])] )\n"
    << "           notation:\n"
    << "             [NOT_][NOT][{AND,OR,XOR}[NOT]]\n"
    << "           notation examples:\n"
    << "             ANDNOT = A & (!B)\n"
    << "             NOTAND = (!A) & B\n"
    << "             NOTANDNOT = (!A) & (!B)\n"
    << "             NOT_NOTANDNOT = !( (!A) & (!B) )\n"
    << "             NOT_AND = !(A & B)\n"
    << "             OR = A | B\n"
    << "             XOR = A ^ B\n"
    << "             NOT = !A\n"
    << "             NOT_NOT = A\n"
    << "           Internally this expression is simplified.\n"
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "  [-arg]   argument, necessary for some ops\n"
    << "  [-dim]   dimension, default: automatically determined from inputimage1\n"
    << "  [-pt]    pixelType, default: automatically determined from inputimage1\n"
    << "Supported: 2D, 3D, (unsigned) short, (unsigned) char.\n"
    << "NOTE: for historical reasons this functionality is not part of the unary or binary image operator." << std::endl;

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
  parser->MarkArgumentAsRequired( "-ops", "The operation to perform." );

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
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string ops = "AND";
  parser->GetCommandLineArgument( "-ops", ops );

  double argument = 0.0;
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Check if the required arguments are given. */
  if( inputFileNames.size() != 2 && ops != "NOT" && ops != "NOT_NOT" && ops != "EQUAL" )
  {
    std::cerr << "ERROR: You should specify two input images." << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFileName1 = inputFileNames[ 0 ];
  std::string inputFileName2 = "";
  if( ( inputFileNames.size() == 2 ) & (ops != "NOT") )
  {
    inputFileName2 = inputFileNames[ 1 ];
  }
  if( ops == "EQUAL" && inputFileNames.size() > 1 && !retarg )
  {
    std::cerr << "ERROR: The operator \"EQUAL\" expects 1 input image and a \"-arg\"." << std::endl;
    return EXIT_FAILURE;
  }

  bool unary = false;
  if( ops.compare("EQUAL") == 0 || ops.compare("NOT") == 0 )
  {
    unary = true;
  }

  /** outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1
      = itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2;
    if( inputFileName2 != "" )
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

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName1, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsLogicalImageOperatorBase * filter = nullptr;

  try
  {
    /** NB: do not add floating point support, since logical operators are
     * not defined on those types.
     */

    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsLogicalImageOperator< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 2, short >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsLogicalImageOperator< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsLogicalImageOperator< 3, short >::New( dim, componentType );
#endif
     /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputFileName;
    filter->m_InputFileName1 = inputFileName1;
    filter->m_InputFileName2 = inputFileName2;
    filter->m_Ops = ops;
    filter->m_UseCompression = useCompression;
    filter->m_Argument = argument;
    filter->m_Unary = unary;

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
