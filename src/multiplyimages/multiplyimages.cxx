#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkMultiplyImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName1, inputFileName2, outputFileName ); \
}

//-------------------------------------------------------------------------------------

/** Declare IntensityWindowing. */
template< class InputImageType >
void MultiplyImages( std::string inputFileName1, std::string inputFileName2,
	std::string outputFileName );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 4 || argc > 10 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::vector< std::string >	inputFileNames;
	bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

	std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

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

	/** Check input. */
	if( inputFileNames.size() != 2 )
	{
		std::cout << "ERROR: You should specify two input images." << std::endl;
		return 1;
	}

	std::string inputFileName1 = inputFileNames[ 0 ];
	std::string inputFileName2 = inputFileNames[ 1 ];

	/** outputFileName */
	if ( outputFileName == "" )
	{
		std::string	part1 = inputFileNames[ 0 ].substr( 0, inputFileNames[ 0 ].rfind( "." ) );
		part1 += "TIMES";
		std::basic_string<char>::size_type dot = inputFileNames[ 1 ].rfind( "." );
		std::basic_string<char>::size_type slash = inputFileNames[ 1 ].rfind( "/" );
		const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
		std::string part2 = "";
		if ( dot != npos && slash != npos )
		{
			part2 = inputFileNames[ 1 ].substr( slash, dot );
		}
		else if ( dot != npos && slash == npos )
		{
			part2 = inputFileNames[ 1 ].substr( 0, dot );
		}
		else if ( dot == npos && slash != npos )
		{
			part2 = inputFileNames[ 1 ].substr( slash, inputFileNames[ 1 ].size() );
		}
		else part2 = inputFileNames[ 1 ];
		outputFileName = part1 + part2 + ".mhd";
	}

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Run the program. */
	try
	{
		run(MultiplyImages,unsigned char,2);
		run(MultiplyImages,unsigned char,3);
		run(MultiplyImages,char,2);
		run(MultiplyImages,char,3);
		run(MultiplyImages,unsigned short,2);
		run(MultiplyImages,unsigned short,3);
		run(MultiplyImages,short,2);
		run(MultiplyImages,short,3);
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
	 * ******************* MultiplyImages *******************
	 */

template< class InputImageType >
void MultiplyImages( std::string inputFileName1, std::string inputFileName2,
	std::string outputFileName )
{
	/** Typedefs. */
	typedef itk::MultiplyImageFilter<
		InputImageType, InputImageType, InputImageType >	MultiplyImageFilterType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;

	/** Declarations. */
	typename MultiplyImageFilterType::Pointer multiplier = MultiplyImageFilterType::New();
	typename ReaderType::Pointer reader1 = ReaderType::New();
	typename ReaderType::Pointer reader2 = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Setup the pipeline. */
	reader1->SetFileName( inputFileName1.c_str() );
	reader2->SetFileName( inputFileName2.c_str() );
	writer->SetFileName( outputFileName.c_str() );

	/** Connect and execute the pipeline. */
	multiplier->SetInput( 0, reader1->GetOutput() );
	multiplier->SetInput( 1, reader2->GetOutput() );
	writer->SetInput( multiplier->GetOutput() );
	writer->Update();

} // end MultiplyImages


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxmultiplyimages" << std::endl;
	std::cout << "  -in      inputFilename1 inputFilename2" << std::endl;
	std::cout << "  [-out]   outputFilename, default in1 + TIMES + in2 + .mhd" << std::endl;
	std::cout << "  [-dim]   dimension, default 3" << std::endl;
	std::cout << "  [-pt]    pixelType, default short" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp

