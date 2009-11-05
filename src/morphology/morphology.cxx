#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "mainhelper.h"
#include <itksys/SystemTools.hxx>

#include "erosion.h"
#include "dilation.h"
#include "opening.h"
#include "closing.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check arguments for help. */
  if ( argc < 5 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::string operation = "";
  bool retop = parser->GetCommandLineArgument( "-op", operation );
  operation = itksys::SystemTools::UnCapitalizedWords( operation );

  std::string type = "Grayscale";
  bool rettype = parser->GetCommandLineArgument( "-type", type );
  type = itksys::SystemTools::UnCapitalizedWords( type );

  std::string boundaryCondition = "";
  bool retbc = parser->GetCommandLineArgument( "-bc", boundaryCondition );

  std::vector<unsigned int> radius;
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::string outputFileName =
    itksys::SystemTools::GetFilenameWithoutLastExtension( inputFileName );
  std::string ext =
    itksys::SystemTools::GetFilenameLastExtension( inputFileName );
  outputFileName += "_" + operation + "_" + type + ext;
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<std::string> bin;
  bool retbin = parser->GetCommandLineArgument( "-bin", bin );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( !retop )
  {
    std::cerr << "ERROR: You should specify \"-op\"." << std::endl;
    return 1;
  }
  if ( !retr )
  {
    std::cerr << "ERROR: You should specify \"-r\"." << std::endl;
    return 1;
  }

  /** Check for valid input options. */
  if ( operation != "erosion" 
    && operation != "dilation"
    && operation != "opening"
    && operation != "closing" )
  {
    std::cerr << "ERROR: \"-op\" should be one of {erosion, dilation, opening, closing}." << std::endl;
    return 1;
  }
  if ( type != "grayscale" && type != "binary" && type != "parabolic" )
  {
    std::cerr << "ERROR: \"-type\" should be one of {grayscale, binary, parabolic}." << std::endl;
    return 1;
  }
  if ( retbin && bin.size() != 3 )
  {
    std::cerr << "ERROR: \"-bin\" should contain three value: foreground, background, erosion." << std::endl;
    return 1;
  }
  
  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  
  /** Let the user overrule this */
  bool retopct = parser->GetCommandLineArgument( "-opct", ComponentType );

  if ( NumberOfComponents > 1 )
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }
  
  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

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
    /** Erosion. */
    run( erosion, unsigned char, 2 );
    run( erosion, char, 2 );
    run( erosion, unsigned short, 2 );
    run( erosion, short, 2 );

    run( erosion, unsigned char, 3 );
    run( erosion, char, 3 );
    run( erosion, unsigned short, 3 );
    run( erosion, short, 3 );

    /** Dilation. */
    run( dilation, unsigned char, 2 );
    run( dilation, char, 2 );
    run( dilation, unsigned short, 2 );
    run( dilation, short, 2 );

    run( dilation, unsigned char, 3 );
    run( dilation, char, 3 );
    run( dilation, unsigned short, 3 );
    run( dilation, short, 3 );

    /** Opening. */
    run( opening, unsigned char, 2 );
    run( opening, char, 2 );
    run( opening, unsigned short, 2 );
    run( opening, short, 2 );

    run( opening, unsigned char, 3 );
    run( opening, char, 3 );
    run( opening, unsigned short, 3 );
    run( opening, short, 3 );

    /** Closing. */
    run( closing, unsigned char, 2 );
    run( closing, char, 2 );
    run( closing, unsigned short, 2 );
    run( closing, short, 2 );

    run( closing, unsigned char, 3 );
    run( closing, char, 3 );
    run( closing, unsigned short, 3 );
    run( closing, short, 3 );
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
      << "pixel (component) type = " << ComponentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main
