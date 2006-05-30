#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
//#include "itkNumericTraits.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == #dim ) \
{ \
    typedef  itk::Image< type, dim >   InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, minimum, maximum ); \
}

//-------------------------------------------------------------------------------------

/** Declare resizeImage. */
template< class InputImageType >
void rescaleIntensity( std::string inputFileName, std::string outputFileName, double minimum, double maximum );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check number of arguments. */
	if ( argc < 3 || argc > 12 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "\tpxrescaleintensityimagefilter -in inputfilename [-out outputfilename] [-mm minimum maximum] [-dim Dimension] [-pt PixelType]" << std::endl;
		std::cout << "Defaults: Dimension = 2, PixelType = short, out = in + INTENSITYRESCALED." << std::endl;
		std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::string	inputFileName = "";
	bool ret1 = parser->GetCommandLineArgument( "-in", inputFileName );

	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "INTENSITYRESCALED.mhd";
	bool ret2 = parser->GetCommandLineArgument( "-out", outputFileName );

	std::vector<double> extrema(2);
	extrema[ 0 ] = 0.0; extrema[ 1 ] = 0.0;
	bool ret3 = parser->GetCommandLineArgument( "-mm", extrema );
	double minimum = extrema[ 0 ];
	double maximum = extrema[ 1 ];

	std::string	Dimension = "2";
	bool ret4 = parser->GetCommandLineArgument( "-dim", Dimension );

	std::string	PixelType = "short";
	bool ret5 = parser->GetCommandLineArgument( "-pt", PixelType );

	/** Check if the required arguments are given. */
	if ( !ret1 )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

	/** Check if the extrema are given (correctly). */
	if ( ret3 )
	{
		if ( extrema.size() != 2 )
		{
			std::cerr << "ERROR: You should specify \"-mm\" with two values." << std::endl;
			return 1;
		}
		if ( extrema[ 1 ] <= extrema[ 0 ] )
		{
			std::cerr << "ERROR: You should specify \"-mm\" with two values:" << std::endl;
			std::cerr << "minimum maximum, where it should hold that maximum > minimum." << std::endl;
			return 1;
		}
	}

	/** Get rid of the possible "_" in PixelType. */
	std::basic_string<char>::size_type pos = PixelType.find( "_" );
	static const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
	if ( pos != npos )
	{
		PixelType.replace( pos, 1, " " );
	}

	/** Run the program. */
	try
	{
		run(rescaleIntensity,unsigned char,2);
		run(rescaleIntensity,unsigned char,3);
		run(rescaleIntensity,char,2);
		run(rescaleIntensity,char,3);
		run(rescaleIntensity,unsigned short,2);
		run(rescaleIntensity,unsigned short,3);
		run(rescaleIntensity,short,2);
		run(rescaleIntensity,short,3);
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
	 * ******************* resizeImage *******************
	 *
	 * The rescaleIntensity function templated over the input pixel type.
	 */

template< class InputImageType >
void rescaleIntensity( std::string inputFileName, std::string outputFileName, double minimum, double maximum )
{
	/** TYPEDEF's. */
	typedef itk::RescaleIntensityImageFilter< InputImageType, InputImageType >	RescalerType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;
	typedef typename InputImageType::PixelType					PixelType;

	/** DECLARATION'S. */
	typename RescalerType::Pointer rescaler = RescalerType::New();
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Define the extrema. */
	PixelType min, max;
	if ( minimum == 0.0 && maximum == 0.0 )
	{
		min = itk::NumericTraits<PixelType>::NonpositiveMin();
		max = itk::NumericTraits<PixelType>::max();
	}
	else
	{
		min = static_cast<PixelType>( minimum );
		max = static_cast<PixelType>( maximum );
	}

	/** Read in the inputImage. */
	reader->SetFileName( inputFileName.c_str() );

	/** Setup the rescaler. */
	rescaler->SetInput( reader->GetOutput() );
	rescaler->SetOutputMinimum( min );
	rescaler->SetOutputMaximum( max );

	/** Write the output image. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( rescaler->GetOutput() );
	writer->Update();

} // end rescaleIntensity

