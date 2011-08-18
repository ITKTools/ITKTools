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
 \brief Perform a morphological operation on an image.
 
 \verbinclude morphology.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "mainhelper1.h"
#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
     << "pxmorphology" << std::endl
     << "  -in      inputFilename" << std::endl
     << "  -op      operation, choose one of {erosion, dilation, opening, closing, gradient}" << std::endl
     << "  [-type]  type, choose one of {grayscale, binary, parabolic}, default grayscale" << std::endl
     << "  [-out]   outputFilename, default in_operation_type.extension" << std::endl
     << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
     << "  -r       radius" << std::endl
     << "  [-bc]    boundaryCondition (grayscale): the gray value outside the image" << std::endl
     << "  [-bin]   foreground and background values" << std::endl
     << "  [-a]     algorithm type for op=gradient" << std::endl
     << "           BASIC = 0, HISTO = 1, ANCHOR = 2, VHGW = 3, default 0" << std::endl
     << "           BASIC and HISTO have radius dependent performance, ANCHOR and VHGW not" << std::endl
     << "  [-opct]  pixelType, default: automatically determined from input image" << std::endl
     << "For grayscale filters, supply the boundary condition." << std::endl
     << "  This value defaults to the maximum pixel value." << std::endl
     << "For binary filters, supply the foreground and background value." << std::endl
     << "  The foreground value refers to the value of the object of interest (default 1)," << std::endl
     << "  the background value is by default 0," << std::endl
     << "  It is not only intended for binary images, but also for grayscale images." << std::endl
     << "  In this case the foreground value selects which value to do the operation on." << std::endl
     << "Examples:" << std::endl
     << "  1) Dilate a binary image (1 = foreground, 0 = background)" << std::endl
     << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1" << std::endl
     << "  2) Dilate a binary image (255 = foreground, 0 = background)" << std::endl
     << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1 -bin 255 0" << std::endl
     << "Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

  return ss.str();

} // end GetHelpString()

extern bool Morphology2D(
  const std::string & componentType,
  const unsigned int & Dimension,
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & operation,
  const std::string & type,
  const std::string & boundaryCondition,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const int & algorithm,
  const bool useCompression );
extern bool Morphology3D(
  const std::string & componentType,
  const unsigned int & Dimension,
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & operation,
  const std::string & type,
  const std::string & boundaryCondition,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const int & algorithm,
  const bool useCompression );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-op", "Operation." );
  parser->MarkArgumentAsRequired( "-r", "Radius." );

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

  std::string operation = "";
  parser->GetCommandLineArgument( "-op", operation );
  operation = itksys::SystemTools::UnCapitalizedWords( operation );

  std::string type = "grayscale";
  parser->GetCommandLineArgument( "-type", type );
  type = itksys::SystemTools::UnCapitalizedWords( type );

  std::string boundaryCondition = "";
  parser->GetCommandLineArgument( "-bc", boundaryCondition );

  std::vector<unsigned int> radius;
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::string outputFileName =
    itksys::SystemTools::GetFilenameWithoutLastExtension( inputFileName );
  std::string ext =
    itksys::SystemTools::GetFilenameLastExtension( inputFileName );
  outputFileName += "_" + operation + "_" + type + ext;
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<std::string> bin;
  bool retbin = parser->GetCommandLineArgument( "-bin", bin );

  int algorithm = 0;
  bool reta = parser->GetCommandLineArgument( "-a", algorithm );

	const bool useCompression = parser->ArgumentExists( "-z" );


  /** Check for valid input options. */
  if ( operation != "erosion"
    && operation != "dilation"
    && operation != "opening"
    && operation != "closing"
    && operation != "gradient" )
  {
    std::cerr << "ERROR: \"-op\" should be one of {erosion, dilation, opening, closing, gradient}." << std::endl;
    return 1;
  }
  if ( type != "grayscale" && type != "binary" && type != "parabolic" )
  {
    std::cerr << "ERROR: \"-type\" should be one of {grayscale, binary, parabolic}." << std::endl;
    return 1;
  }
  if ( retbin && bin.size() != 2 )
  {
    std::cerr << "ERROR: \"-bin\" should contain two values: foreground and background." << std::endl;
    return 1;
  }
  if ( reta && ( algorithm < 0 || algorithm > 3 ) )
  {
    std::cerr << "ERROR: \"-a\" should have a value 0, 1, 2 or 3." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string componentType = "short";
  std::string pixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int numberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    pixelType,
    componentType,
    Dimension,
    numberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }

  /** Let the user overrule this */
  parser->GetCommandLineArgument( "-opct", componentType );

  if ( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The number of components is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( componentType );

  /** Check radius. */
  if ( retr )
  {
    if ( radius.size() != Dimension && radius.size() != 1 )
    {
      std::cout << "ERROR: The number of radii should be 1 or Dimension." << std::endl;
      return 1;
    }
  }

  /** Get the radius. */
  std::vector<unsigned int> Radius( Dimension, radius[ 0 ] );
  if ( retr && radius.size() == Dimension )
  {
    for ( unsigned int i = 1; i < Dimension; i++ )
    {
      Radius[ i ] = radius[ i ];
      if ( Radius[ i ] < 1 )
      {
        std::cout << "ERROR: No nonpositive numbers are allowed in radius." << std::endl;
        return 1;
      }
    }
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    if ( Dimension == 2 )
    {
      supported = Morphology2D( componentType, Dimension,
        inputFileName, outputFileName, operation, type,
        boundaryCondition, Radius, bin, algorithm, useCompression );
    }
    else if ( Dimension == 3 )
    {
      supported = Morphology3D( componentType, Dimension,
        inputFileName, outputFileName, operation, type,
        boundaryCondition, Radius, bin, algorithm, useCompression );
    }
  }
  catch( itk::ExceptionObject & e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** Check if this image type was supported. */
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixel type and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << componentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
