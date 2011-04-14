#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkStatisticsImageFilter.h"
#include "itkInvertIntensityImageFilter.h"

/** This program inverts the intensity of an image:
 * new = max - old,
 * where max is the maximum of an image.
 */

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare InvertIntensity. */
template< class InputImageType >
void InvertIntensity(
  const std::string & inputFileName,
  const std::string & outputFileName );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
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
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "INVERTED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
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

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Run the program. */
  bool supported = false;
  try
  {
    /** 2D. */
    run( InvertIntensity, char, 2 );
    run( InvertIntensity, unsigned char, 2 );
    run( InvertIntensity, short, 2 );
    run( InvertIntensity, unsigned short, 2 );
    run( InvertIntensity, float, 2 );
    run( InvertIntensity, double, 2 );

    /** 3D. */
    run( InvertIntensity, char, 3 );
    run( InvertIntensity, unsigned char, 3 );
    run( InvertIntensity, short, 3 );
    run( InvertIntensity, unsigned short, 3 );
    run( InvertIntensity, float, 3 );
    run( InvertIntensity, double, 3 );

  } // end run
    catch ( itk::ExceptionObject &e )
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


  /**
   * ******************* InvertIntensity *******************
   *
   * The resize function templated over the input pixel type.
   */

template< class InputImageType >
void InvertIntensity( const std::string & inputFileName,
  const std::string & outputFileName )
{
  /** Some typedef's. */
  typedef typename InputImageType::PixelType                InputPixelType;
  typedef itk::ImageFileReader< InputImageType >            ReaderType;
  typedef itk::ImageFileWriter< InputImageType >            WriterType;
  typedef itk::StatisticsImageFilter< InputImageType >      StatisticsFilterType;
  typedef typename StatisticsFilterType::RealType                   RealType;
  typedef itk::InvertIntensityImageFilter< InputImageType > InvertIntensityFilterType;

  /** Create reader. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Create statistics filter. */
  typename StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
  statistics->SetInput( reader->GetOutput() );
  statistics->Update();

  /** Get all the output stuff. */
  InputPixelType max = statistics->GetMaximum();

  /** Create invert filter. */
  typename InvertIntensityFilterType::Pointer invertFilter = InvertIntensityFilterType::New();
  invertFilter->SetInput( reader->GetOutput() );
  invertFilter->SetMaximum( max );

  /** Create writer. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( invertFilter->GetOutput() );
  writer->Update();

} // end InvertIntensity()

  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp()
{
  std::cout << "This program inverts the intensities of an image." << std::endl;
  std::cout << "Usage:" << std::endl << "pxinvertintensityimagefilter" << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-out]   outputFilename; default: in + INVERTED.mhd" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;
} // end PrintHelp()

