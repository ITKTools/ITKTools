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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "cropimage.h"
#include "cropimageMainHelper.h"


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
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "pxcropimage can be called in different ways:\n"
    << "1: supply two points with \"-pA\" and \"-pB\".\n"
    << "2: supply a points and a size with \"-pA\" and \"-sz\".\n"
    << "3: supply a lower and an upper bound with \"-lb\" and \"-ub\".\n"
    << "The points are supplied in index coordinates.\n"
    << "Supported: 2D, 3D, 4D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

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

  bool useCompression = parser->ArgumentExists( "-z" );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Check which input option is used:
   * 1: supply two points with -pA and -pB
   * 2: supply a points and a size with -pA and -sz
   * 3: supply a lower and an upper bound with -lb and -ub
   */
  unsigned int option = 0;
  if( !CheckWhichInputOption( retpA, retpB, retsz, retlb, retub, option ) )
  {
    std::cerr << "ERROR: Check your commandline arguments." << std::endl;
    return EXIT_FAILURE;
  }

  /** Check argument pA. Point A should only be positive if not force. */
  if( retpA )
  {
    if( !ProcessArgument( pA, dim, force ) )
    {
      std::cout << "ERROR: Point A should consist of 1 or Dimension positive values." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check argument pB. Point B should always be positive. */
  if( retpB )
  {
    if( !ProcessArgument( pB, dim, false ) )
    {
      std::cout << "ERROR: Point B should consist of 1 or Dimension positive values." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check argument sz. Size should always be positive. */
  if( retsz )
  {
    if( !ProcessArgument( sz, dim, false ) )
    {
      std::cout << "ERROR: The size sz should consist of 1 or Dimension positive values." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check argument lb. */
  if( retlb )
  {
    if( !ProcessArgument( lowBound, dim, force ) )
    {
      std::cout << "ERROR: The lowerbound lb should consist of 1 or Dimension positive values." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check argument ub. */
  if( retub )
  {
    if( !ProcessArgument( upBound, dim, force ) )
    {
      std::cout << "ERROR: The upperbound ub should consist of 1 or Dimension positive values." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Get inputs. */
  std::vector<int> input1, input2;
  if( option == 1 )
  {
    GetBox( pA, pB, dim );
    input1 = pA;
    input2 = pB;
  }
  else if( option == 2 )
  {
    input1 = pA;
    input2 = sz;
  }
  else if( option == 3 )
  {
    input1 = lowBound;
    input2 = upBound;
  }

  /** Class that does the work. */
  ITKToolsCropImageBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsCropImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCropImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 3, double >::New( dim, componentType );
#endif

#ifdef ITKTOOLS_4D_SUPPORT
    if( !filter ) filter = ITKToolsCropImage< 4, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, unsigned long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, long >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCropImage< 4, double >::New( dim, componentType );
#endif

    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_Input1 = input1;
    filter->m_Input2 = input2;
    filter->m_Option = option;
    filter->m_Force = force;
    filter->m_UseCompression = useCompression;

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

} // end main()
