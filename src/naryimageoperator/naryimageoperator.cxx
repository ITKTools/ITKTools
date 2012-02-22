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
#include "ITKToolsHelpers.h"
#include "naryimageoperator.h"

#include "NaryImageOperatorMainHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Performs n-ary operations on multiple (n) images.\n"
    << "Usage:\npxnaryimageoperator\n"
    << "  -in      inputFilenames, at least 2\n"
    << "  -out     outputFilename\n"
    << "  -ops     n-ary operator of the following form:\n"
    << "           {+,-,*,/,^,%}\n"
    << "           notation:\n"
    << "             {ADDITION, MINUS, TIMES, DIVIDE,\n"
    << "             MEAN,\n"
    << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE,\n"
    << "             NARYMAGNITUDE }\n"
    << "           notation examples:\n"
    << "             MINUS = I_0 - I_1 - ... - I_n \n"
    << "             ABSDIFF = |I_0 - I_1 - ... - I_n|\n"
    << "             MIN = min( I_0, ..., I_n )\n"
    << "             MAGNITUDE = sqrt( I_0 * I_0 + ... + I_n * I_n )\n"
//   std::cout << "  [-arg]   argument, necessary for some ops\n"
//             << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
//             << "             MASK[NEG]: background value, e.g. 0.\n";
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "  [-s]     number of streams, default equals number of inputs.\n"
    << "  [-opct]  output component type, by default the largest of the two input images\n"
    << "             choose one of: {[unsigned_]{char,short,int,long},float,double}\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

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

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
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
  if( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two input file names." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOComponentType componentTypeIn = itk::ImageIOBase::LONG;
  itk::ImageIOBase::IOComponentType componentTypeOut = itk::ImageIOBase::LONG;
  unsigned int dim = 2;
  int retdip = DetermineImageProperties( inputFileNames,
    componentTypeIn, componentTypeOut, dim );
  if( retdip ) return EXIT_FAILURE;

  /** Let the user override the output component type. */
  if( retopct )
  {
    componentTypeOut = itk::ImageIOBase::GetComponentTypeFromString( opct );
    if( !itktools::ComponentTypeIsValid( componentTypeOut ) )
    {
      std::cerr << "ERROR: the you specified an invalid opct." << std::endl;
      return EXIT_FAILURE;
    }
    
    if( !itktools::ComponentTypeIsInteger( componentTypeOut ) )
    {
      componentTypeIn = itk::ImageIOBase::DOUBLE;
    }
  }

  /** Check if a valid operator is given. */
  std::string opsCopy = ops;
  int retCO  = CheckOperator( ops );
  if( retCO ) return retCO;

  /** For certain ops an argument is mandatory. */
  bool retCOA = CheckOperatorAndArgument( ops, argument, retarg );
  if( !retCOA ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsNaryImageOperatorBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, char >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, unsigned char >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, short >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, unsigned short >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, int >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, unsigned int >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, long >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, long, unsigned long >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, double, float >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 2, double, double >::New( dim, componentTypeIn, componentTypeOut );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, char >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, unsigned char >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, short >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, unsigned short >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, int >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, unsigned int >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, long >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, long, unsigned long >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, double, float >::New( dim, componentTypeIn, componentTypeOut );
    if( !filter ) filter = ITKToolsNaryImageOperator< 3, double, double >::New( dim, componentTypeIn, componentTypeOut );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentTypeIn, componentTypeOut );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileNames = inputFileNames;
    filter->m_OutputFileName = outputFileName;
    filter->m_NaryOperatorName = ops;
    filter->m_UseCompression = useCompression;
    filter->m_NumberOfStreams = numberOfStreams;
    filter->m_Arg = argument;

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
