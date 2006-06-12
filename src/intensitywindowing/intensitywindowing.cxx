#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkIntensityWindowingImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, window ); \
}

//-------------------------------------------------------------------------------------

/** Declare IntensityWindowing. */
template< class InputImageType >
void IntensityWindowing( std::string inputFileName, std::string outputFileName,
	std::vector<double> window );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 5 || argc > 7 )
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
	outputFileName += "WINDOWED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	std::vector<double> window;
	bool retw = parser->GetCommandLineArgument( "-w", window );

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
	if ( !retw )
	{
		std::cerr << "ERROR: You should specify \"-w\"." << std::endl;
		return 1;
	}

	/** Check window. */
	if( window.size() != 2 )
	{
		std::cout << "ERROR: The window should consist of two numbers." << std::endl;
		return 1;
	}
	if ( window[ 1 ] < window[ 0 ] )
	{
		double temp = window[ 0 ];
		window[ 0 ] = window[ 1 ];
		window[ 1 ] = temp;
	}
	if ( window[ 0 ] == window[ 1 ] )
	{
		std::cerr << "ERROR: The window should be larger." << std::endl;
		return 1;
	}

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Run the program. */
	try
	{
		run(IntensityWindowing,unsigned char,2);
		run(IntensityWindowing,unsigned char,3);
		run(IntensityWindowing,char,2);
		run(IntensityWindowing,char,3);
		run(IntensityWindowing,unsigned short,2);
		run(IntensityWindowing,unsigned short,3);
		run(IntensityWindowing,short,2);
		run(IntensityWindowing,short,3);
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
	 * ******************* IntensityWindowing *******************
	 */

template< class InputImageType >
void IntensityWindowing( std::string inputFileName, std::string outputFileName,
	std::vector<double> window )
{
	/** Typedefs. */
	typedef itk::IntensityWindowingImageFilter<
		InputImageType, InputImageType >									WindowingType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;
	typedef typename InputImageType::PixelType					InputPixelType;

	/** Declarations. */
	typename WindowingType::Pointer windowfilter = WindowingType::New();
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Setup the pipeline. */
	reader->SetFileName( inputFileName.c_str() );
	writer->SetFileName( outputFileName.c_str() );
	InputPixelType min = static_cast<InputPixelType>( window[ 0 ] );
	InputPixelType max = static_cast<InputPixelType>( window[ 1 ] );
	windowfilter->SetWindowMinimum( min );
	windowfilter->SetWindowMaximum( max );
	windowfilter->SetOutputMinimum( min );
	windowfilter->SetOutputMaximum( max );

	/** Connect and execute the pipeline. */
	windowfilter->SetInput( reader->GetOutput() );
	writer->SetInput( windowfilter->GetOutput() );
	writer->Update();

} // end IntensityWindowing


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxintensitywindowing" << std::endl;
	std::cout << "\t-in\tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in + WINDOWED.mhd" << std::endl;
	std::cout << "\t-w\twindowMinimum windowMaximum" << std::endl;
	std::cout << "\t[-dim]\tdimension, default 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType, default short" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp

