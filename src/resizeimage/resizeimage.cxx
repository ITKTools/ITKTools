#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkResampleImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == #dim ) \
{ \
    typedef  itk::Image< type, dim >   InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, factor ); \
}

//-------------------------------------------------------------------------------------

/** Declare resizeImage. */
template< class InputImageType >
void resizeImage( std::string inputFileName, std::string outputFileName, double factor );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check number of arguments. */
	if ( argc < 5 || argc > 11 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "\tpxresizeimage -in inputfilename [-out outputfilename] -f factor [-dim Dimension] [-pt PixelType]" << std::endl;
		std::cout << "Defaults: Dimension = 2, PixelType = short, out = in + RESIZED." << std::endl;
		std::cout << "Supported: 2D, 3D, (unsigned) short." << std::endl;
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::string	inputFileName = "";
	bool ret1 = parser->GetCommandLineArgument( "-in", inputFileName );

	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "RESIZED.mhd";
	bool ret2 = parser->GetCommandLineArgument( "-out", outputFileName );

	double factor;
	bool ret3 = parser->GetCommandLineArgument( "-f", factor );

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
	if ( !ret3 )
	{
		std::cerr << "ERROR: You should specify \"-f\"." << std::endl;
		return 1;
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
		run(resizeImage,unsigned short,2);
		run(resizeImage,unsigned short,3);
		run(resizeImage,short,2);
		run(resizeImage,short,3);
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
	 * The resize function templated over the input pixel type.
	 */

template< class InputImageType >
void resizeImage( std::string inputFileName, std::string outputFileName, double factor )
{
	/** TYPEDEF's. */
	typedef itk::ResampleImageFilter< InputImageType, InputImageType >	ResamplerType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;

	typedef typename InputImageType::SizeType				SizeType;
	typedef typename InputImageType::SpacingType			SpacingType;

	const unsigned int Dimension = InputImageType::ImageDimension;

	/** DECLARATION'S. */
	typename InputImageType::Pointer inputImage = InputImageType::New();
	typename ResamplerType::Pointer resampler = ResamplerType::New();
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Read in the inputImage. */
	reader->SetFileName( inputFileName.c_str() );
	inputImage = reader->GetOutput();
	inputImage->Update();

	/** Prepare stuff. */
	SpacingType outputSpacing = inputImage->GetSpacing();
	SizeType outputSize = inputImage->GetLargestPossibleRegion().GetSize();
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		outputSpacing[ i ] = outputSpacing[ i ] / factor;
		outputSize[ i ] = static_cast<unsigned int>( outputSize[ i ] * factor );
	}

	/** Setup the pipeline.
	 * The resampler has by default an IdentityTransform as transform, and
	 * a LinearInterpolateImageFunction as interpolator.
	 */
	resampler->SetInput( inputImage );
	resampler->SetSize( outputSize );
	resampler->SetDefaultPixelValue( 0 );
	resampler->SetOutputStartIndex( inputImage->GetLargestPossibleRegion().GetIndex() );
	resampler->SetOutputSpacing( outputSpacing );
	resampler->SetOutputOrigin( inputImage->GetOrigin() );

	/** Write the output image. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( resampler->GetOutput() );
	writer->Update();

} // end resize

