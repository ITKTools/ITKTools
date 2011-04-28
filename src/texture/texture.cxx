#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <itksys/SystemTools.hxx>
#include "itkTextureImageToImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMultiThreader.h"

//-------------------------------------------------------------------------------------

// To print the progress
class ShowProgressObject
{
public:
  ShowProgressObject( itk::ProcessObject* o )
  {
    m_Process = o;
  }
  void ShowProgress()
  {
    std::cout << "\rProgress: "
      << static_cast<unsigned int>( 100.0 * m_Process->GetProgress() ) << "%";
  }
  itk::ProcessObject::Pointer m_Process;
};

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, typeIn, typeOut, dim ) \
if ( componentTypeIn == #typeIn && componentTypeOut == #typeOut && Dimension == dim ) \
{ \
  typedef itk::Image< typeIn,  dim > InputImageType; \
  typedef itk::Image< typeOut, dim > OutputImageType; \
  function< InputImageType, OutputImageType >( inputFileName, outputDirectory, \
    neighborhoodRadius, offsetScales, numberOfBins, numberOfOutputs ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare PerformTextureAnalysis. */
template< class InputImageType, class OutputImageType >
void PerformTextureAnalysis(
  const std::string & inputFileName,
  const std::string & outputDirectory,
  unsigned int neighborhoodRadius,
  const std::vector< unsigned int > & offsetScales,
  unsigned int numberOfBins,
  unsigned int numberOfOutputs );

/** Declare other functions. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
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

  std::string base = itksys::SystemTools::GetFilenamePath( inputFileName );
  if ( base != "" ) base = base + "/";
  std::string outputDirectory = base;
  parser->GetCommandLineArgument( "-out", outputDirectory );
  bool endslash = itksys::SystemTools::StringEndsWith( outputDirectory.c_str(), "/" );
  if ( !endslash ) outputDirectory += "/";

  unsigned int neighborhoodRadius = 3;
  parser->GetCommandLineArgument( "-r", neighborhoodRadius );

  std::vector<unsigned int> offsetScales( 1, 1 );
  parser->GetCommandLineArgument( "-os", offsetScales );

  unsigned int numberOfBins = 128;
  parser->GetCommandLineArgument( "-b", numberOfBins );

  unsigned int numberOfOutputs = 8;
  parser->GetCommandLineArgument( "-noo", numberOfOutputs );

  std::string componentTypeOut = "float";
  parser->GetCommandLineArgument( "-opct", componentTypeOut );

  /** Check that numberOfOutputs <= 8. */
  if ( numberOfOutputs > 8 )
  {
    std::cerr << "ERROR: The maximum number of outputs is 8. You requested "
      << numberOfOutputs << "." << std::endl;
    return 1;
  }

  /** Threads. */
  unsigned int maximumNumberOfThreads
    = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(
    maximumNumberOfThreads );

  /** Determine image properties. */
  std::string componentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    componentTypeIn,
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

  /** Input images are read in as float, always. The default output is float,
   * but can be overridden by specifying -opct in the command line.
   */
  componentTypeIn = "float";

  /** Run the program. */
  bool supported = false;
  try
  {
    run( PerformTextureAnalysis, float, float, 2 );
    run( PerformTextureAnalysis, float, double, 2 );

    run( PerformTextureAnalysis, float, float, 3 );
    run( PerformTextureAnalysis, float, double, 3 );
  }
  catch ( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << componentTypeIn
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

template< class InputImageType, class OutputImageType >
void PerformTextureAnalysis(
  const std::string & inputFileName,
  const std::string & outputDirectory,
  unsigned int neighborhoodRadius,
  const std::vector< unsigned int > & offsetScales,
  unsigned int numberOfBins,
  unsigned int numberOfOutputs )
{

  /** Typedefs. */
  typedef itk::TextureImageToImageFilter<
    InputImageType, OutputImageType >                   TextureFilterType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Read the input. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Setup the texture filter. */
  typename TextureFilterType::Pointer textureFilter = TextureFilterType::New();
  textureFilter->SetInput( reader->GetOutput() );
  textureFilter->SetNeighborhoodRadius( neighborhoodRadius );
  textureFilter->SetOffsetScales( offsetScales );
  textureFilter->SetNumberOfHistogramBins( numberOfBins );
  textureFilter->SetNormalizeHistogram( false );
  textureFilter->SetNumberOfRequestedOutputs( numberOfOutputs );

  /** Create and attach a progress observer. */
  ShowProgressObject progressWatch( textureFilter );
  itk::SimpleMemberCommand<ShowProgressObject>::Pointer command
    = itk::SimpleMemberCommand<ShowProgressObject>::New();
  command->SetCallbackFunction( &progressWatch, &ShowProgressObject::ShowProgress );
  textureFilter->AddObserver( itk::ProgressEvent(), command );

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
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFileNames[ i ].c_str() );
    writer->SetInput( textureFilter->GetOutput( i ) );
    writer->Update();
  }

} // end PerformTextureAnalysis()


/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText = "Usage: \
  pxtexture \
  This program computes texture features based on the gray-level co-occurrence matrix (GLCM).\n \
    -in      inputFilename\n \
    [-out]   outputDirectory, default equal to the inputFilename directory\n \
    [-r]     the radius of the neighborhood on which to construct the GLCM, default 3\n \
    [-os]    the desired offset scales to compute the GLCM, default 1, but can be e.g. 1 2 4\n \
    [-b]     the number of bins of the GLCM, default 128\n \
    [-noo]   the number of texture feature outputs, default all 8\n \
    [-opct]  output pixel component type, default float\n \
  Supported: 2D, 3D, any input image type, float or double output type.";

  return helpText;

} // end PrintHelp()

