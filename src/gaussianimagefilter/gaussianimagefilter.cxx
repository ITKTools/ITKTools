#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "gaussianImageFilterHelper.h"

//-------------------------------------------------------------------------------------
/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim >  OutputImageType; \
  if ( retmag ) \
  { \
    function##Magnitude< OutputImageType >( inputFileName, outputFileName, sigma, order ); \
  } \
  if ( retlap ) \
  { \
    function##Laplacian< OutputImageType >( inputFileName, outputFileName, sigma ); \
  } \
  if ( retinv ) \
  { \
    function##Invariants< OutputImageType >( inputFileName, outputFileName, sigma, invariant ); \
  } \
  if ( !retmag && !retlap && !retinv ) \
  { \
    function< OutputImageType >( inputFileName, outputFileName, sigma, order ); \
  } \
  supported = true; \
}

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
  }

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<float> sigma;
  sigma.push_back( 1.0 ); // default 1.0 for each resolution
  parser->GetCommandLineArgument( "-std", sigma );

  std::vector<unsigned int> order;
  parser->GetCommandLineArgument( "-ord", order );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "BLURRED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  bool retmag = parser->ArgumentExists( "-mag" );

  bool retlap = parser->ArgumentExists( "-lap" );

  std::string invariant = "LiLi";
  bool retinv = parser->GetCommandLineArgument( "-inv", invariant );

  std::string componentType = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentType );

  /** Check options. */
  for ( unsigned int i = 0; i < order.size(); ++i )
  {
    if ( order[ i ] > 2 )
    {
      std::cerr << "ERROR: The order should not be higher than 2." << std::endl;
      std::cerr << "Only zeroth, first and second order derivatives are supported." << std::endl;
      return 1;
    }
  }

  /** Check that not both mag and lap are given. */
  if ( retmag && retlap )
  {
    std::cerr << "ERROR: only one of \"-mag\" and \"-lap\" should be given!" << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 ) return 1;

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  if ( !retopct ) componentType = ComponentTypeIn;

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check order. */
  if ( !retlap && !retinv )
  {
    if ( order.size() != Dimension )
    {
      std::cerr << "ERROR: the # of orders should be equal to the image dimension!" << std::endl;
      return 1;
    }
  }

  /** Check sigma. */
  if ( sigma.size() != 1 && sigma.size() != Dimension )
  {
    std::cerr << "ERROR: the # of sigmas should be equal to 1 or the image dimension!" << std::endl;
    return 1;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( GaussianImageFilter, char, 2 );
    run( GaussianImageFilter, unsigned char, 2 );
    run( GaussianImageFilter, short, 2 );
    run( GaussianImageFilter, unsigned short, 2 );
    run( GaussianImageFilter, int, 2 );
    run( GaussianImageFilter, unsigned int, 2 );
    run( GaussianImageFilter, long, 2 );
    run( GaussianImageFilter, unsigned long, 2 );
    run( GaussianImageFilter, float, 2 );
    run( GaussianImageFilter, double, 2 );

    run( GaussianImageFilter, char, 3 );
    run( GaussianImageFilter, unsigned char, 3 );
    run( GaussianImageFilter, short, 3 );
    run( GaussianImageFilter, unsigned short, 3 );
    run( GaussianImageFilter, int, 3 );
    run( GaussianImageFilter, unsigned int, 3 );
    run( GaussianImageFilter, long, 3 );
    run( GaussianImageFilter, unsigned long, 3 );
    run( GaussianImageFilter, float, 3 );
    run( GaussianImageFilter, double, 3 );
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
      << "pixel (component) type = " << ComponentTypeIn
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main

