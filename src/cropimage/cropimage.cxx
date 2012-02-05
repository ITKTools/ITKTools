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
 \brief Crop an image.
 
 \verbinclude cropimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "cropimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:" << std::endl
    << "pxcropimage\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + CROPPED.mhd\n"
    << "  [-pA]    a point A\n"
    << "  [-pB]    a point B\n"
    << "  [-sz]    size\n"
    << "  [-lb]    lower bound\n"
    << "  [-ub]    upper bound\n"
    << "  [-force] force to extract a region of size sz, pad if necessary\n"
    << "pxcropimage can be called in different ways:\n"
    << "1: supply two points with \"-pA\" and \"-pB\".\n"
    << "2: supply a points and a size with \"-pA\" and \"-sz\".\n"
    << "3: supply a lower and an upper bound with \"-lb\" and \"-ub\".\n"
    << "The points are supplied in index coordinates.\n"
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

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "CROPPED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<int> pA;
  bool retpA = parser->GetCommandLineArgument( "-pA", pA );

  std::vector<int> pB;
  bool retpB = parser->GetCommandLineArgument( "-pB", pB );

  std::vector<int> sz;
  bool retsz = parser->GetCommandLineArgument( "-sz", sz );

  std::vector<int> lowBound;
  bool retlb = parser->GetCommandLineArgument( "-lb", lowBound );

  std::vector<int> upBound;
  bool retub = parser->GetCommandLineArgument( "-ub", upBound );

  bool force = parser->ArgumentExists( "-force" );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** Check which input option is used:
   * 1: supply two points with -pA and -pB
   * 2: supply a points and a size with -pA and -sz
   * 3: supply a lower and an upper bound with -lb and -ub
   */
  unsigned int option = 0;
  if ( !CheckWhichInputOption( retpA, retpB, retsz, retlb, retub, option ) )
  {
    std::cerr << "ERROR: Check your commandline arguments." << std::endl;
    return 1;
  }

  /** Check argument pA. Point A should only be positive if not force. */
  if ( retpA )
  {
    if ( !ProcessArgument( pA, Dimension, force ) )
    {
      std::cout << "ERROR: Point A should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument pB. Point B should always be positive. */
  if ( retpB )
  {
    if ( !ProcessArgument( pB, Dimension, false ) )
    {
      std::cout << "ERROR: Point B should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument sz. Size should always be positive. */
  if ( retsz )
  {
    if ( !ProcessArgument( sz, Dimension, false ) )
    {
      std::cout << "ERROR: The size sz should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument lb. */
  if ( retlb )
  {
    if ( !ProcessArgument( lowBound, Dimension, force ) )
    {
      std::cout << "ERROR: The lowerbound lb should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument ub. */
  if ( retub )
  {
    if ( !ProcessArgument( upBound, Dimension, force ) )
    {
      std::cout << "ERROR: The upperbound ub should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Get inputs. */
  std::vector<int> input1, input2;
  if ( option == 1 )
  {
    GetBox( pA, pB, Dimension );
    input1 = pA;
    input2 = pB;
  }
  else if ( option == 2 )
  {
    input1 = pA;
    input2 = sz;
  }
  else if ( option == 3 )
  {
    input1 = lowBound;
    input2 = upBound;
  }

  /** Run the program. */
  
  /** Class that does the work */
  CropImageBase * cropImage = 0; 

  /** Short alias */
  unsigned int dim = Dimension;

  /** Add "_" in ComponentType and convert. */
  itktools::ReplaceSpaceWithUnderscore( ComponentTypeIn );
  itktools::ComponentType componentType
    = itk::ImageIOBase::GetComponentTypeFromString( ComponentTypeIn );
   
  try
  {    
    // now call all possible template combinations.
    if (!cropImage) cropImage = CropImage< unsigned char, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< char, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned short, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< short, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned int, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< int, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned long, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< long, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< float, 2 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< double, 2 >::New( componentType, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!cropImage) cropImage = CropImage< unsigned char, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< char, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned short, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< short, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned int, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< int, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< unsigned long, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< long, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< float, 3 >::New( componentType, dim );
    if (!cropImage) cropImage = CropImage< double, 3 >::New( componentType, dim );
#endif
    if (!cropImage)
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << itk::ImageIOBase::GetComponentTypeAsString( componentType )
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    cropImage->m_InputFileName = inputFileName;
    cropImage->m_OutputFileName = outputFileName;
    cropImage->m_Input1 = input1;
    cropImage->m_Input2 = input2;
    cropImage->m_Option = option;
    cropImage->m_Force = force;
  
    cropImage->Run();
    
    delete cropImage;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete cropImage;
    return 1;
  }
  

  /** End program. */
  return 0;

} // end main()
