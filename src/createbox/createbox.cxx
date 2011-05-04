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
 \brief Create a box image.
 
 \verbinclude createbox.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "CreateBoxHelper.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( referenceIOBase, outputFileName, input1, input2, input, orientation ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpText() );

  /** Required argument checking. */
  parser->MarkArgumentAsRequired( "-out", "" );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Output image information is specified using either a reference
   * input image, or by -sz.
   */
  std::vector<std::string> exactlyOneArguments1;
  exactlyOneArguments1.push_back( "-in" );
  exactlyOneArguments1.push_back( "-sz" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments1,
    "ERROR: You should specify either \"-in\" or \"-sz\"." );

  /** Three ways to specify a box, see --help. */
  std::vector<std::string> exactlyOneArguments2;
  exactlyOneArguments2.push_back( "-c" );
  exactlyOneArguments2.push_back( "-cp1" );
  exactlyOneArguments2.push_back( "-ci1" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments2,
    "ERROR: You should specify either \"-c\" or \"-cp1\" or \"-ci1\"." );

  std::vector<std::string> exactlyOneArguments3;
  exactlyOneArguments3.push_back( "-r" );
  exactlyOneArguments3.push_back( "-cp2" );
  exactlyOneArguments3.push_back( "-ci2" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments3,
    "ERROR: You should specify either \"-r\" or \"-cp2\" or \"-ci2\"." );

  /** Get arguments: output image file name. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get arguments: output image information. */
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::vector<unsigned int> size( Dimension );

  parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( Dimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( Dimension, 0.0 );
  parser->GetCommandLineArgument( "-io", origin );

  std::vector<double> direction( Dimension * Dimension, 0.0 );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    direction[ i * ( Dimension + 1 ) ] = 1.0;
  }
  parser->GetCommandLineArgument( "-d", direction );

  /** Get arguments: box position, size and orientation. */
  std::vector<double> center( Dimension );
  bool retc = parser->GetCommandLineArgument( "-c", center );

  std::vector<double> radius( Dimension );
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::vector<double> corner1( Dimension );
  bool retcp1 = parser->GetCommandLineArgument( "-cp1", corner1 );

  std::vector<double> corner2( Dimension );
  bool retcp2 = parser->GetCommandLineArgument( "-cp2", corner2 );

  std::vector<double> cornerindex1( Dimension );
  bool retci1 = parser->GetCommandLineArgument( "-ci1", cornerindex1 );

  std::vector<double> cornerindex2( Dimension );
  bool retci2 = parser->GetCommandLineArgument( "-ci2", cornerindex2 );

  std::vector<double> orientation( Dimension, 0.0 );
  parser->GetCommandLineArgument( "-o", orientation );

  /** Additional check. */
  if ( ( !retc | !retr | retcp1 | retcp2 | retci1 | retci2 )
    && ( retc | retr | !retcp1 | !retcp2 | retci1 | retci2 )
    && ( retc | retr | retcp1 | retcp2 | !retci1 | !retci2 ) )
  {
    std::cerr << "ERROR: Either you should specify \"-c\" and \"-r\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-cp1\" and \"-cp2\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-ci1\" and \"-ci2\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine output image properties. */
  std::string componentType = "short";
  itk::ImageIOBase::Pointer referenceIOBase;
  if ( retin )
  {
    /** Get the properties of the reference image. */
    bool retgip = GetImageProperties( inputFileName, referenceIOBase );
    if ( !retgip ) return EXIT_FAILURE;

    /** Extract dimension and component type for template selection. */
    Dimension = referenceIOBase->GetNumberOfDimensions();
    componentType = referenceIOBase->GetComponentTypeAsString(
      referenceIOBase->GetComponentType() );

    /** Fix the output to be scalar. */
    referenceIOBase->SetNumberOfComponents( 1 );
  }
  else
  {
    FillImageIOBase( referenceIOBase,
      "scalar", componentType, Dimension, 1,
      size, spacing, origin, direction );
  }

  /** Let the user overrule this. */
  parser->GetCommandLineArgument( "-pt", componentType );

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( componentType );

  /** How was the input supplied by the user? */
  std::vector<double> input1, input2;
  std::string input = "";
  if ( retc )
  {
    input = "CenterRadius";
    input1 = center;
    input2 = radius;
  }
  else if ( retcp1 )
  {
    input = "CornersAsPoints";
    input1 = corner1;
    input2 = corner2;
  }
  else if ( retci1 )
  {
    input = "CornersAsIndices";
    input1 = cornerindex1;
    input2 = cornerindex2;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CreateBox, unsigned char, 2 );
    run( CreateBox, char, 2 );
    run( CreateBox, unsigned short, 2 );
    run( CreateBox, short, 2 );

    run( CreateBox, unsigned char, 3 );
    run( CreateBox, char, 3 );
    run( CreateBox, unsigned short, 3 );
    run( CreateBox, short, 3 );
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
      << "pixel (component) type = " << componentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main
