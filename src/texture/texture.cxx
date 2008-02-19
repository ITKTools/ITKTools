#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <itksys/SystemTools.hxx>

#include "itkTextureImageToImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( componentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > OutputImageType; \
  function< OutputImageType >( inputFileName, outputDirectory, \
  neighborhoodRadius, offsetScales, numberOfBins, numberOfOutputs ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare PerformTextureAnalysis. */
template< class InputImageType >
void PerformTextureAnalysis(
  const std::string & inputFileName,
  const std::string & outputDirectory,
  unsigned int neighborhoodRadius,
  const std::vector< unsigned int > & offsetScales,
  unsigned int numberOfBins,
  unsigned int numberOfOutputs );

/** Declare other functions. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 3 )
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

  std::string base = itksys::SystemTools::GetFilenamePath( inputFileName );
  if ( base != "" ) base = base + "/";
  std::string outputDirectory = base;
  bool retout = parser->GetCommandLineArgument( "-out", outputDirectory );
  bool endslash = itksys::SystemTools::StringEndsWith( outputDirectory.c_str(), "/" );
  if ( !endslash ) outputDirectory += "/";

  unsigned int neighborhoodRadius = 3;
  bool retr = parser->GetCommandLineArgument( "-r", neighborhoodRadius );

  std::vector<unsigned int> offsetScales( 1, 1 );
  bool retos = parser->GetCommandLineArgument( "-os", offsetScales );

  unsigned int numberOfBins = 128;
  bool retb = parser->GetCommandLineArgument( "-b", numberOfBins );

  unsigned int numberOfOutputs = 8;
  bool retnoo = parser->GetCommandLineArgument( "-noo", numberOfOutputs );

  //std::string componentType = "";
  //bool retpt = parser->GetCommandLineArgument( "-opct", componentType );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  /** Check that numberOfOutputs <= 8. */
  if ( numberOfOutputs > 8 )
  {
    std::cerr << "ERROR: The maximum number of outputs is 8. You requested " << numberOfOutputs << "." << std::endl;
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

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  //if ( !retpt ) componentType = ComponentTypeIn;
  std::string componentType = ComponentTypeIn;

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( componentType );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( PerformTextureAnalysis, unsigned char, 2 );
    run( PerformTextureAnalysis, char, 2 );
    run( PerformTextureAnalysis, unsigned short, 2 );
    run( PerformTextureAnalysis, short, 2 );
    run( PerformTextureAnalysis, unsigned int, 2 );
    run( PerformTextureAnalysis, int, 2 );
    run( PerformTextureAnalysis, unsigned long, 2 );
    run( PerformTextureAnalysis, long, 2 );
    run( PerformTextureAnalysis, float, 2 );
    run( PerformTextureAnalysis, double, 2 );

    run( PerformTextureAnalysis, unsigned char, 3 );
    run( PerformTextureAnalysis, char, 3 );
    run( PerformTextureAnalysis, unsigned short, 3 );
    run( PerformTextureAnalysis, short, 3 );
    run( PerformTextureAnalysis, unsigned int, 3 );
    run( PerformTextureAnalysis, int, 3 );
    run( PerformTextureAnalysis, unsigned long, 3 );
    run( PerformTextureAnalysis, long, 3 );
    run( PerformTextureAnalysis, float, 3 );
    run( PerformTextureAnalysis, double, 3 );
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

} // end main()


  /**
   * ******************* PerformTextureAnalysis *******************
   */

template< class InputImageType >
void PerformTextureAnalysis(
  const std::string & inputFileName,
  const std::string & outputDirectory,
  unsigned int neighborhoodRadius,
  const std::vector< unsigned int > & offsetScales,
  unsigned int numberOfBins,
  unsigned int numberOfOutputs )
{
  const unsigned int Dimension = InputImageType::ImageDimension;

  /** Typedefs. */
  typedef itk::Image< double, Dimension >               DoubleImageType;
  typedef typename DoubleImageType::Pointer             DoubleImagePointer;
  typedef itk::TextureImageToImageFilter<
    InputImageType, DoubleImageType >                   TextureFilterType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef typename ReaderType::Pointer                  ReaderPointer;
  typedef itk::ImageFileWriter< DoubleImageType >       WriterType;
  typedef typename WriterType::Pointer                  WriterPointer;

  /** Read the input. */
  ReaderPointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Setup the texture filter. */
  typename TextureFilterType::Pointer textureFilter = TextureFilterType::New();
  textureFilter->SetInput( reader->GetOutput() );
  textureFilter->SetNeighborhoodRadius( neighborhoodRadius );
  textureFilter->SetOffsetScales( offsetScales );
  textureFilter->SetNumberOfHistogramBins( numberOfBins );
  textureFilter->SetNormalizeHistogram( false );
  textureFilter->SetNumberOfRequestedOutputs( numberOfOutputs );

  /** Create the output file names. */
  std::vector< std::string > outputFileNames( 8, "" );
  outputFileNames[ 0 ] = outputDirectory + "energy.mhd";
  outputFileNames[ 1 ] = outputDirectory + "entropy.mhd";
  outputFileNames[ 2 ] = outputDirectory + "correlation.mhd";
  outputFileNames[ 3 ] = outputDirectory + "inverseDifferenceMoment.mhd";
  outputFileNames[ 4 ] = outputDirectory + "inertia.mhd";
  outputFileNames[ 5 ] = outputDirectory + "clusterShade.mhd";
  outputFileNames[ 6 ] = outputDirectory + "clusterProminence.mhd";
  outputFileNames[ 7 ] = outputDirectory + "HaralickCorrelation.mhd";

  /** Setup and process the pipeline. */
  for ( unsigned int i = 0; i < numberOfOutputs; ++i )
  {
    WriterPointer writer = WriterType::New();
    writer->SetFileName( outputFileNames[ i ].c_str() );
    writer->SetInput( textureFilter->GetOutput( i ) );
    writer->Update();
  }

} // end PerformTextureAnalysis()


  /**
   * ******************* PrintHelp *******************
   */

void PrintHelp( void )
{
  std::cout << "Usage:" << std::endl << "pxtexture" << std::endl;
  std::cout << "This program computes texture features based on the gray-level co-occurence matrix (GLCM)." << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-out]   outputDirectory, default equal to the inputFilename directory" << std::endl;
  std::cout << "  [-r]     the radius of the neighborhood on which to construct the GLCM, default 3" << std::endl;
  std::cout << "  [-os]    the desired offset scales to compute the GLCM, default 1, but can be e.g. 1 2 4" << std::endl;
  std::cout << "  [-b]     the number of bins of the GLCM, default 128" << std::endl;
  std::cout << "  [-noo]   the number of texture feature outputs, default all 8" << std::endl;
  //std::cout << "  [-opct]  output pixel component type, default derived from the input image" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double ";
  std::cout << "input image types. Output image type is double." << std::endl;

} // end PrintHelp()

