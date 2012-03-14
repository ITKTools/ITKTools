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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"
#include "ITKToolsImageProperties.h"

#include "BinaryImageOperatorMainHelper.h"
#include "BinaryImageOperatorHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream  ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Performs binary operations on two images." << std::endl
    << "Usage:\npxbinaryimageoperator\n"
    << "  -in      inputFilenames\n"
    << "  [-out]   outputFilename, default in1 + ops + arg + in2 + .mhd\n"
    << "  -ops     binary operator of the following form:\n"
    << "           {+,-,*,/,^,%}\n"
    << "           notation:\n"
    << "             {ADDITION, WEIGHTEDADDITION, MINUS, TIMES,DIVIDE,POWER,\n"
    << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE, SQUAREDDIFFERENCE,\n"
    << "             BINARYMAGNITUDE, MASK, MASKNEGATED, LOG}\n"
    << "           notation examples:\n"
    << "             MINUS = A - B\n"
    << "             ABSDIFF = |A - B|\n"
    << "             MIN = min( A, B )\n"
    << "             MAGNITUDE = sqrt( A * A + B * B )\n"
    << "  [-arg]   argument, necessary for some ops\n"
    << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
    << "             MASK[NEG]: background value, e.g. 0.\n"
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "  [-opct]  output component type, by default the largest of the two input images\n"
    << "           choose one of: {[unsigned_]{char,short,int,long},float,double}\n"
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
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  // You must specify two input files.
  if( inputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify two input file names." << std::endl;
    return EXIT_FAILURE;
  }

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string argument = "0";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  std::string outputComponentTypeString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", outputComponentTypeString );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Create outputFileName. */
  if( outputFileName == "" )
  {
    CreateOutputFileName( inputFileNames, outputFileName, ops, argument );
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOComponentType componentType1;
  itk::ImageIOBase::IOComponentType componentType2;
  itk::ImageIOBase::IOComponentType componentTypeOut;
  DetermineComponentTypes( inputFileNames, componentType1, componentType2, componentTypeOut );

  /** Let the user override the output component type. */
  if( retopct )
  {
    componentTypeOut = itk::ImageIOBase::GetComponentTypeFromString( outputComponentTypeString );
    if( !itktools::ComponentTypeIsValid( componentTypeOut ) )
    {
      std::cerr << "ERROR: the you specified a wrong opct." << std::endl;
      return EXIT_FAILURE;
    }
    if( !itktools::ComponentTypeIsInteger( componentTypeOut ) )
    {
      componentType1 = componentType2 = itk::ImageIOBase::DOUBLE;
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
  ITKToolsBinaryImageOperatorBase * filter = NULL;

  unsigned int dim = 0;
  itktools::GetImageDimension( inputFileNames[1], dim );

  /** Short aliases. */
  itk::ImageIOBase::IOComponentType inCType1 = componentType1;
  itk::ImageIOBase::IOComponentType inCType2 = componentType2;
  itk::ImageIOBase::IOComponentType outCType = componentTypeOut;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, char >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, unsigned char >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, short >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, unsigned short >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, int >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, unsigned int >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, long >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, long, long, unsigned long >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, double, double, float >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 2, double, double, double >::New( dim, inCType1, inCType2, outCType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, char >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, unsigned char >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, short >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, unsigned short >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, int >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, unsigned int >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, long >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, long, long, unsigned long >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, double, double, float >::New( dim, inCType1, inCType2, outCType );
    if( !filter ) filter = ITKToolsBinaryImageOperator< 3, double, double, double >::New( dim, inCType1, inCType2, outCType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, inCType1, inCType2, outCType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName1 = inputFileNames[0];
    filter->m_InputFileName2 = inputFileNames[1];
    filter->m_OutputFileName = outputFileName;
    filter->m_Ops = ops;
    filter->m_UseCompression = useCompression;
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
