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
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "UnaryImageOperatorMainHelper.h"
#include "UnaryImageOperatorHelper.h"

/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Unary operations on one image." << std::endl
  << "Usage:" << std::endl
  << "pxunaryimageoperator" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  -ops     UnaryOperator of the following form:" << std::endl
  << "           {+,-,*,/,^,%}" << std::endl
  << "           notation:" << std::endl
  << "             {PLUS,{R,L}MINUS,TIMES,{R,L}DIVIDE,{R,L}POWER,{R,L}MOD," << std::endl
  << "             NEG,SIGN,ABS,FLOOR,CEIL,ROUND," << std::endl
  << "             LN,LOG10,NLOG,EXP,[ARC]SIN,[ARC]COS,[ARC]TAN}" << std::endl
  << "           notation examples:" << std::endl
  << "             RMINUS = A - arg" << std::endl
  << "             LMINUS = arg - A" << std::endl
  << "             SIN = sin(A)" << std::endl
  << "             RPOWER = A ^ arg" << std::endl
  << "             TIMES = A * arg" << std::endl
  << "  [-arg]   argument, necessary for some ops" << std::endl
  << "  [-out]   outputFilename, default in + <ops> + <arg> + .mhd" << std::endl
  << "  [-pto]   outputPixelType, default: same as inputimage" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, float.";
  return ss.str();
} // end GetHelpString()

/* Functions to do the work: this splits things up
 * in different compilation units.
 */
extern int UnaryImageOperatorScalar( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  std::string ops = "PLUS";
  parser->GetCommandLineArgument( "-ops", ops );
  std::string argument = "1";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  /** Create outputFileName. */
  if ( outputFileName == "" )
  {
    CreateOutputFileName( inputFileName, outputFileName, ops, argument );
  }

  /** The input is only templated over int and double. */
  itktools::ComponentType inputComponentType = itk::ImageIOBase::DOUBLE;

  bool inputIsInteger = itktools::ComponentTypeIsInteger( inputComponentType );
  if ( inputIsInteger ) 
  {
    inputComponentType = itk::ImageIOBase::INT;
  }

  /** Get the output component type. */
  itktools::ComponentType outputComponentType = inputComponentType;
  std::string componentTypeOutString = "";
  bool retpto = parser->GetCommandLineArgument( "-pto", componentTypeOutString );
  if(retpto)
  {
    outputComponentType = itktools::GetComponentTypeFromString(componentTypeOutString);
  }
  
  /** Get the correct form of ops. For some operators
   * there are integer and double versions, in which case
   * ops is concatenated with INT or DOUBLE. For example
   * for the ABS functor this is true. For these functors
   * the decision is made based on the inputComponentType
   * and the argument type. If both are of integer type then
   * INT is used, otherwise DOUBLE.
   */
  bool argumentIsInteger = itktools::StringIsInteger( argument );
  /** Append ops and at the same time check if ops is a valid
   * functor.
   */
  std::string opsOld = ops;
  int retCO  = CheckOps( ops, inputIsInteger & argumentIsInteger );
  if ( retCO ) return retCO;

  /** For certain ops an argument is mandatory. */
  bool operatorNeedsArgument = OperatorNeedsArgument( opsOld );
  if ( operatorNeedsArgument && !retarg )
  {
    std::cerr << "ERROR: operator " << opsOld << " needs an argument." << std::endl;
    std::cerr << "Specify the argument with \"-arg\"." << std::endl;
    return 1;
  }
  if ( !operatorNeedsArgument && retarg )
  {
    std::cerr << "WARNING: operator " << opsOld << " does not need an argument." << std::endl;
    std::cerr << "The argument (" << argument << ") is ignored." << std::endl;
  }


  /** Class that does the work */
  UnaryImageOperatorBase * unaryImageOperator = NULL;

  unsigned int dim = 0;
  itktools::GetImageDimension(inputFileName, dim);

  try
  {    
    // now call all possible template combinations.
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned char, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, char, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned short, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, short, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned int, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, int, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, float, 2 >::New( inputComponentType, outputComponentType, dim );
    
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned char, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, char, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned short, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, short, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned int, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, int, 2 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, float, 2 >::New( inputComponentType, outputComponentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned char, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, char, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned short, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, short, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, unsigned int, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, int, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< int, float, 3 >::New( inputComponentType, outputComponentType, dim );
    
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned char, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, char, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned short, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, short, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, unsigned int, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, int, 3 >::New( inputComponentType, outputComponentType, dim );
    if (!unaryImageOperator) unaryImageOperator = UnaryImageOperator< double, float, 3 >::New( inputComponentType, outputComponentType, dim );
#endif
    if (!unaryImageOperator) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "input pixel (component) type = " << inputComponentType
        << "output pixel (component) type = " << outputComponentType
        << " ; dimension = " << dim
        << std::endl;
      return 1;
    }

    unaryImageOperator->m_InputFileName = inputFileName;
    unaryImageOperator->m_OutputFileName = outputFileName;
    unaryImageOperator->m_Ops = ops;
    unaryImageOperator->m_Arg = argument;

    unaryImageOperator->Run();
    
    delete unaryImageOperator;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete unaryImageOperator;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
