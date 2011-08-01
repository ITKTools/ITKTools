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
#include "ITKToolsBase.h"

#include "CreateBoxHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
    << "pxcreatebox" << std::endl
    << "  -out     outputFilename" << std::endl
    << "  Arguments to specify the output image:" << std::endl
    << "  [-in]    inputFilename, to copy image information from" << std::endl
    << "  [-sz]    image size (voxels)" << std::endl
    << "  [-sp]    image spacing (mm), default 1.0" << std::endl
    << "  [-io]    image origin, default 0.0" << std::endl
    << "  [-d]     image direction, default identity" << std::endl
    << "  [-dim]   dimension, default 3" << std::endl
    << "  [-pt]    pixelType, default short" << std::endl
    << "  Arguments to specify the box:" << std::endl
    << "  [-c]     center (mm)" << std::endl
    << "  [-r]     radii (mm)" << std::endl
    << "  [-cp1]   cornerpoint 1 (mm)" << std::endl
    << "  [-cp2]   cornerpoint 2 (mm)" << std::endl
    << "  [-ci1]   cornerindex 1" << std::endl
    << "  [-ci2]   cornerindex 2" << std::endl
    << "  [-o]     orientation of the box, default xyz" << std::endl
    << "- The user should EITHER specify the input filename OR the output image size." << std::endl
    << "- The user should EITHER specify the center and the radius," << std::endl
    << "    OR the positions of two opposite corner points." << std::endl
    << "    OR the positions of two opposite corner indices." << std::endl
    << "- The orientation is a vector with Euler angles (rad)." << std::endl
    << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

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

  if ( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if ( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
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
    bool retgip = itktools::GetImageIOBase( inputFileName, referenceIOBase );
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
    itktools::FillImageIOBase( referenceIOBase,
      "scalar", componentType, Dimension, 1,
      size, spacing, origin, direction );
  }

  /** Let the user overrule this. */
  parser->GetCommandLineArgument( "-pt", componentType );

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( componentType );

  itktools::ComponentType componentTypeAsEnum
    = referenceIOBase->GetComponentTypeFromString( componentType );
    
  /** How was the input supplied by the user? */
  std::vector<double> input1, input2;
  std::string boxDefinition = "";
  if ( retc )
  {
    boxDefinition = "CenterRadius";
    input1 = center;
    input2 = radius;
  }
  else if ( retcp1 )
  {
    boxDefinition = "CornersAsPoints";
    input1 = corner1;
    input2 = corner2;
  }
  else if ( retci1 )
  {
    boxDefinition = "CornersAsIndices";
    input1 = cornerindex1;
    input2 = cornerindex2;
  }

  /** Class that does the work */
  ITKToolsCreateBoxBase * createBox = 0; 

  try
  {        
    if (!createBox) createBox = ITKToolsCreateBox< unsigned char, 2 >::New( componentTypeAsEnum, Dimension );
    if (!createBox) createBox = ITKToolsCreateBox< char, 2 >::New( componentTypeAsEnum, Dimension );
    if (!createBox) createBox = ITKToolsCreateBox< unsigned short, 2 >::New( componentTypeAsEnum, Dimension );
    if (!createBox) createBox = ITKToolsCreateBox< short, 2 >::New( componentTypeAsEnum, Dimension );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createBox) createBox = ITKToolsCreateBox< unsigned char, 3 >::New( componentTypeAsEnum, Dimension );    
    if (!createBox) createBox = ITKToolsCreateBox< char, 3 >::New( componentTypeAsEnum, Dimension );
    if (!createBox) createBox = ITKToolsCreateBox< unsigned short, 3 >::New( componentTypeAsEnum, Dimension );
    if (!createBox) createBox = ITKToolsCreateBox< short, 3 >::New( componentTypeAsEnum, Dimension );
#endif
    if (!createBox) 
    {
      itk::ImageIOBase::Pointer imageIOBaseTmp;
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    createBox->m_ReferenceImageIOBase = referenceIOBase;
    createBox->m_OutputFileName = outputFileName;
    createBox->m_Input1 = input1;
    createBox->m_Input2 = input2;
    createBox->m_OrientationOfBox = orientation;
    createBox->m_BoxDefinition = boxDefinition;
    
    createBox->Run();
    
    delete createBox;  
  }
  catch ( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createBox;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main
