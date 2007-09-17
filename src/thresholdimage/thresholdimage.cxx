#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, threshold1, threshold2 ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ThresholdImage. */
template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
	const std::string & outputFileName,
	const double & threshold1,
	const double & threshold2 );

/** Declare PrintHelp. */
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
	std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "THRESHOLDED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	double threshold1 = itk::NumericTraits<double>::NonpositiveMin();
	bool rett1 = parser->GetCommandLineArgument( "-t1", threshold1 );

	double threshold2 = itk::NumericTraits<double>::One;
	bool rett2 = parser->GetCommandLineArgument( "-t2", threshold2 );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string	PixelType; //we don't use this
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
    run( ThresholdImage, char, 2 );
    run( ThresholdImage, unsigned char, 2 );
		run( ThresholdImage, short, 2 );
    run( ThresholdImage, unsigned short, 2 );
    run( ThresholdImage, float, 2 );
    run( ThresholdImage, double, 2 );

    /** 3D. */
    run( ThresholdImage, char, 3 );
    run( ThresholdImage, unsigned char, 3 );
		run( ThresholdImage, short, 3 );
    run( ThresholdImage, unsigned short, 3 );
    run( ThresholdImage, float, 3 );
    run( ThresholdImage, double, 3 );

	} // end run
	catch ( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr <<
      "pixel (component) type = " << ComponentTypeIn <<
      " ; dimension = " << Dimension 
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ******************* ThresholdImage *******************
	 *
	 * The threshold function templated over the input pixel type.
	 */

template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
	const std::string & outputFileName,
	const double & threshold1,
	const double & threshold2 )
{
	/** Typedef's. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef InputImageType                              OutputImageType;
  typedef InputPixelType                              OutputPixelType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::BinaryThresholdImageFilter<
		InputImageType, OutputImageType>									ThresholderType;
	typedef itk::ImageFileWriter< OutputImageType >			WriterType;
	
	/** Declarations. */
	InputPixelType lowerthreshold;
	typename ReaderType::Pointer reader = ReaderType::New();
	typename ThresholderType::Pointer thresholder = ThresholderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Read in the inputImage. */
	reader->SetFileName( inputFileName.c_str() );
	
	/** Apply the threshold. */
	lowerthreshold = static_cast<InputPixelType>( vnl_math_max(
		static_cast<double>( itk::NumericTraits<InputPixelType>::NonpositiveMin() ),
		threshold1 ) );
	thresholder->SetLowerThreshold( lowerthreshold );
	thresholder->SetUpperThreshold( static_cast<InputPixelType>( threshold2 ) );
  thresholder->SetInsideValue( itk::NumericTraits<OutputPixelType>::Zero );
	thresholder->SetOutsideValue( itk::NumericTraits<OutputPixelType>::One );
	thresholder->SetInput( reader->GetOutput() );

	/** Write the output image. */
	writer->SetInput( thresholder->GetOutput() );
	writer->SetFileName( outputFileName.c_str() );
	writer->Update();

} // end ThresholdImage()


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "This program thresholds an image." << std::endl;
	std::cout << "Usage:" << std::endl << "pxthresholdimage" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
	std::cout << "  [-out]   outputFilename; default: in + THRESHOLDED.mhd" << std::endl;
	std::cout << "  [-t1]    lower threshold; default: -infinity" << std::endl;
	std::cout << "  [-t2]    upper threshold; everything >t2 will get a value 1 default: 1.0" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;
} // end PrintHelp()

