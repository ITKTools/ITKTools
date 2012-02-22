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
#include "ITKToolsHelpers.h"
#include "createbox.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxcreatebox\n"
    << "  -out     outputFilename\n"
    << "  Arguments to specify the output image:\n"
    << "  [-in]    inputFilename, to copy image information from\n"
    << "  [-sz]    image size (voxels)\n"
    << "  [-sp]    image spacing (mm), default 1.0\n"
    << "  [-io]    image origin, default 0.0\n"
    << "  [-d]     image direction, default identity\n"
    << "  [-dim]   dimension, default 3\n"
    << "  [-opct]  pixelType, default short\n"
    << "  Arguments to specify the box:\n"
    << "  [-c]     center (mm)\n"
    << "  [-r]     radii (mm)\n"
    << "  [-cp1]   cornerpoint 1 (mm)\n"
    << "  [-cp2]   cornerpoint 2 (mm)\n"
    << "  [-ci1]   cornerindex 1\n"
    << "  [-ci2]   cornerindex 2\n"
    << "  [-o]     orientation of the box, default xyz\n"
    << "- The user should EITHER specify the input filename OR the output image size.\n"
    << "- The user should EITHER specify the center and the radius,\n"
    << "    OR the positions of two opposite corner points.\n"
    << "    OR the positions of two opposite corner indices.\n"
    << "- The orientation is a vector with Euler angles (rad).\n"
    << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short.\n";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  /** Required argument checking. */
  parser->MarkArgumentAsRequired( "-out", "" );
  parser->MarkArgumentAsRequired( "-in", "" );

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

  unsigned int dim = 3;
  parser->GetCommandLineArgument( "-dim", dim );

  std::vector<unsigned int> size( dim );

  parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( dim, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( dim, 0.0 );
  parser->GetCommandLineArgument( "-io", origin );

  std::vector<double> direction( dim * dim, 0.0 );
  for( unsigned int i = 0; i < dim; i++ )
  {
    direction[ i * ( dim + 1 ) ] = 1.0;
  }
  parser->GetCommandLineArgument( "-d", direction );

  /** Get arguments: box position, size and orientation. */
  std::vector<double> center( dim );
  bool retc = parser->GetCommandLineArgument( "-c", center );

  std::vector<double> radius( dim );
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::vector<double> corner1( dim );
  bool retcp1 = parser->GetCommandLineArgument( "-cp1", corner1 );

  std::vector<double> corner2( dim );
  bool retcp2 = parser->GetCommandLineArgument( "-cp2", corner2 );

  std::vector<double> cornerindex1( dim );
  bool retci1 = parser->GetCommandLineArgument( "-ci1", cornerindex1 );

  std::vector<double> cornerindex2( dim );
  bool retci2 = parser->GetCommandLineArgument( "-ci2", cornerindex2 );

  std::vector<double> orientation( dim, 0.0 );
  parser->GetCommandLineArgument( "-o", orientation );

  /** Additional check. */
  if( ( !retc | !retr | retcp1 | retcp2 | retci1 | retci2 )
    && ( retc | retr | !retcp1 | !retcp2 | retci1 | retci2 )
    && ( retc | retr | retcp1 | retcp2 | !retci1 | !retci2 ) )
  {
    std::cerr << "ERROR: Either you should specify \"-c\" and \"-r\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-cp1\" and \"-cp2\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-ci1\" and \"-ci2\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine output image properties. */
  std::string componentTypeAsString = "short";
  itk::ImageIOBase::Pointer referenceIOBase;
  if( retin )
  {
    /** Get the properties of the reference image. */
    bool retgip = itktools::GetImageIOBase( inputFileName, referenceIOBase );
    if( !retgip ) return EXIT_FAILURE;

    /** Extract dimension and component type for template selection. */
    dim = referenceIOBase->GetNumberOfDimensions();
    componentTypeAsString = referenceIOBase->GetComponentTypeAsString(
      referenceIOBase->GetComponentType() );

    /** Fix the output to be scalar. */
    referenceIOBase->SetNumberOfComponents( 1 );
  }
  else
  {
    itktools::FillImageIOBase( referenceIOBase,
      "scalar", componentTypeAsString, dim, 1,
      size, spacing, origin, direction );
  }

  /** Let the user overrule this. */
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  itk::ImageIOBase::IOComponentType componentType
    = referenceIOBase->GetComponentTypeFromString( componentTypeAsString );
    
  /** How was the input supplied by the user? */
  std::vector<double> input1, input2;
  std::string boxDefinition = "";
  if( retc )
  {
    boxDefinition = "CenterRadius";
    input1 = center;
    input2 = radius;
  }
  else if( retcp1 )
  {
    boxDefinition = "CornersAsPoints";
    input1 = corner1;
    input2 = corner2;
  }
  else if( retci1 )
  {
    boxDefinition = "CornersAsIndices";
    input1 = cornerindex1;
    input2 = cornerindex2;
  }

  /** Class that does the work. */
  ITKToolsCreateBoxBase * filter = 0; 

  try
  {        
    if( !filter ) filter = ITKToolsCreateBox< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateBox< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateBox< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateBox< 2, short >::New( dim, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCreateBox< 3, unsigned char >::New( dim, componentType );    
    if( !filter ) filter = ITKToolsCreateBox< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateBox< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateBox< 3, short >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_ReferenceImageIOBase = referenceIOBase;
    filter->m_OutputFileName = outputFileName;
    filter->m_Input1 = input1;
    filter->m_Input2 = input2;
    filter->m_OrientationOfBox = orientation;
    filter->m_BoxDefinition = boxDefinition;
    
    filter->Run();
    
    delete filter;  
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. Return a value. */
  return EXIT_SUCCESS;

} // end main
