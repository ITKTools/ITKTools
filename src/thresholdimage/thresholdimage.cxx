#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, threshold1, threshold2 ); \
}

//-------------------------------------------------------------------------------------

/** Declare ThresholdImage. */
template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
	const std::string & outputFileName,
	double threshold1,
	double threshold2);

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 3 || argc > 9 )
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

	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "THRESHOLDED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	double threshold1 = itk::NumericTraits<double>::NonpositiveMin();
	bool rett1 = parser->GetCommandLineArgument( "-t1", threshold1 );

	double threshold2 = itk::NumericTraits<double>::One;
	bool rett2 = parser->GetCommandLineArgument( "-t2", threshold2 );
		
	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );
	
	std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
	
	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace(PixelType);
	
	/** Run the program. */
	try
	{
		run(ThresholdImage,short,2);
		run(ThresholdImage,short,3);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ******************* ThresholdImage *******************
	 *
	 * The resize function templated over the input pixel type.
	 */

template< class InputImageType >
void ThresholdImage( const std::string & inputFileName, const std::string & outputFileName, 
											 double threshold1, double threshold2 )
{
	
	/** constants */
	const unsigned int Dimension = InputImageType::ImageDimension;
	
	/** TYPEDEF's. */
	typedef typename InputImageType::PixelType					InputPixelType;
	typedef InputPixelType															OutputPixelType;
	
	typedef itk::Image<OutputPixelType, Dimension>			OutputImageType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::BinaryThresholdImageFilter<
		InputImageType, OutputImageType>									ThresholderType;
	typedef itk::ImageFileWriter< OutputImageType >			WriterType;
	
	/** DECLARATION'S. */
	InputPixelType lowerthreshold;
	typename ReaderType::Pointer reader = ReaderType::New();
	typename ThresholderType::Pointer thresholder = ThresholderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Read in the inputImage. */
	
	reader->SetFileName( inputFileName.c_str() );

	std::cout << "Reading input image..." << std::endl;		
	reader->Update();
	std::cout << "Input image read." << std::endl;
	
	/** Apply the threshold. */

	lowerthreshold = static_cast<InputPixelType>( vnl_math_max(
		static_cast<double>( itk::NumericTraits<InputPixelType>::NonpositiveMin() ),
		threshold1 ) );
	thresholder->SetLowerThreshold( 
		lowerthreshold );
	thresholder->SetUpperThreshold( static_cast<InputPixelType>(threshold2) );
	thresholder->SetInsideValue( itk::NumericTraits<OutputPixelType>::Zero );
	thresholder->SetOutsideValue( itk::NumericTraits<OutputPixelType>::One );
	thresholder->SetInput( reader->GetOutput() );
	std::cout << "Applying threshold..." << std::endl;
	thresholder->Update();
	std::cout << "Threshold applied." << std::endl;

	/** Write the output image. */
	writer->SetInput( thresholder->GetOutput() );
	writer->SetFileName( outputFileName.c_str() );
	std::cout << "Saving the resulting mask to disk..." << std::endl;
	writer->Update();
	std::cout << "Done." << std::endl;

} // end ThresholdImage


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "This program thresholds an image, using the ITK-BinaryThresholdImageFilter. See the ITK documentation for more information." << std::endl;
	
	std::cout << "Usage:" << std::endl << "pxThresholdImage" << std::endl;
	std::cout << "\t-in\tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename; default: in + THRESHOLDED.mhd" << std::endl;
	std::cout << "\t[-t1]\tlower threshold; default: -infinity" << std::endl;
	std::cout << "\t[-t2]\tupper threshold; default: 1.0" << std::endl;
	std::cout << "\t[-dim]\tdimension; default: 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType; default: short" << std::endl;
	std::cout << "Supported: 2D, 3D, short." << std::endl;
} // end PrintHelp

