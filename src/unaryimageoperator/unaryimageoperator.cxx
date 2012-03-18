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
 \brief Unary operations on an image.

 \verbinclude unaryimageoperator.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "UnaryImageOperatorMainHelper.h"
#include "UnaryImageOperatorHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Unary operations on one image." << std::endl
    << "Usage:\n"
    << "pxunaryimageoperator\n"
    << "  -in      inputFilename\n"
    << "  -ops     UnaryOperator of the following form:\n"
    << "           {+,-,*,/,^,%}\n"
    << "           notation:\n"
    << "             {PLUS,{R,L}MINUS,TIMES,{R,L}DIVIDE,{R,L}POWER,{R,L}MOD,\n"
    << "             NEG,SIGN,ABS,FLOOR,CEIL,ROUND,\n"
    << "             LN,LOG10,NLOG,EXP,[ARC]SIN,[ARC]COS,[ARC]TAN}\n"
    << "           notation examples:\n"
    << "             RMINUS = A - arg\n"
    << "             LMINUS = arg - A\n"
    << "             SIN = sin(A)\n"
    << "             RPOWER = A ^ arg\n"
    << "             TIMES = A * arg\n"
    << "  [-arg]   argument, necessary for some ops\n"
    << "  [-out]   outputFilename, default in + <ops> + <arg> + .mhd\n"
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "  [-opct]  outputPixelComponentType, default: same as input image\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, float.";
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
  parser->MarkArgumentAsRequired( "-ops", "Operation." );

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

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "PLUS";
  parser->GetCommandLineArgument( "-ops", ops );

  std::vector<std::string> arguments( 1, "" );
  bool retarg = parser->GetCommandLineArgument( "-arg", arguments );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Create outputFileName. */
  if( outputFileName == "" )
  {
    CreateOutputFileName( inputFileName, outputFileName, ops, arguments[0] );
  }

  /** Get the input and output component type. */
  itk::ImageIOBase::IOComponentType inputComponentType;
  itktools::GetImageComponentType( inputFileName, inputComponentType );

  itk::ImageIOBase::IOComponentType outputComponentType = inputComponentType;
  std::string componentTypeOutString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeOutString );
  if( retopct )
  {
    outputComponentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeOutString );
  }

  /** The input is only templated over int and double. */
  bool inputIsInteger = itktools::ComponentTypeIsInteger( inputComponentType );
  if( inputIsInteger )
  {
    inputComponentType = itk::ImageIOBase::INT;
  }
  else
  {
    inputComponentType = itk::ImageIOBase::DOUBLE;
  }

  /** Get the correct form of ops. For some operators
   * there are integer and double versions, in which case
   * ops is concatenated with INT or DOUBLE. For example
   * for the ABS functor this is true. For these functors
   * the decision is made based on the inputComponentType
   * and the argument type. If both are of integer type then
   * INT is used, otherwise DOUBLE.
   */
  bool argumentIsInteger = false;
  for( unsigned int i = 0; i < arguments.size(); i++ )
  {
    argumentIsInteger |= itktools::StringIsInteger( arguments[ i ] );
  }

  /** Append ops and at the same time check if ops is a valid
   * functor.
   */
  std::string opsOld = ops;
  int retCO  = CheckOps( ops, inputIsInteger & argumentIsInteger );
  if( retCO ) return retCO;

  /** For certain ops an argument is mandatory. */
  bool operatorNeedsArgument = OperatorNeedsArgument( opsOld );
  if( operatorNeedsArgument && !retarg )
  {
    std::cerr << "ERROR: operator " << opsOld << " needs an argument." << std::endl;
    std::cerr << "Specify the argument with \"-arg\"." << std::endl;
    return EXIT_FAILURE;
  }
  if( !operatorNeedsArgument && retarg )
  {
    std::cerr << "WARNING: operator " << opsOld << " does not need an argument." << std::endl;
    std::cerr << "The argument (" << arguments[0] << ") is ignored." << std::endl;
  }

  /** Class that does the work. */
  ITKToolsUnaryImageOperatorBase * filter = NULL;

  unsigned int dim = 0;
  itktools::GetImageDimension( inputFileName, dim );

  /** Short aliases. */
  itk::ImageIOBase::IOComponentType inputType = inputComponentType;
  itk::ImageIOBase::IOComponentType outputType = outputComponentType;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, unsigned char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, unsigned short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, unsigned int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, int, float >::New( dim, inputType, outputType );

    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, unsigned char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, unsigned short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, unsigned int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 2, double, float >::New( dim, inputType, outputType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, unsigned char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, unsigned short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, unsigned int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, int, float >::New( dim, inputType, outputType );

    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, unsigned char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, char >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, unsigned short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, short >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, unsigned int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, int >::New( dim, inputType, outputType );
    if( !filter ) filter = ITKToolsUnaryImageOperator< 3, double, float >::New( dim, inputType, outputType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, inputType, outputType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_UnaryOperatorName = ops;
    filter->m_UseCompression = useCompression;
    filter->m_Arguments = arguments;

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
