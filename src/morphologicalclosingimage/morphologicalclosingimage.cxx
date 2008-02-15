#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, Radius ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare GrayscaleMorphologicalClosing. */
template< class InputImageType >
void GrayscaleMorphologicalClosing(
  const std::string & inputFileName,
  const std::string & outputFileName,
	const std::vector<unsigned int> & radius );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	/** Check arguments for help. */
	if ( argc < 5 || argc > 13 )
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
	outputFileName += "CLOSED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	std::vector<unsigned int> radius;
	bool retr = parser->GetCommandLineArgument( "-r", radius );

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
	if ( !retr )
	{
		std::cerr << "ERROR: You should specify \"-r\"." << std::endl;
		return 1;
	}

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Check radius. */
	if ( retr )
	{
		if( radius.size() != Dimension && radius.size() != 1 )
		{
			std::cout << "ERROR: The number of radii should be 1 or Dimension." << std::endl;
			return 1;
		}
	}

	/** Get the radius. */
	std::vector<unsigned int> Radius( Dimension, radius[ 0 ] );
	if ( retr && radius.size() == Dimension )
	{
		for ( unsigned int i = 1; i < Dimension; i++ )
		{
			Radius[ i ] = radius[ i ];
		}
	}

	/** Check Radius for nonpositive numbers. */
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		if ( Radius[ i ] < 1 )
		{
			std::cout << "ERROR: No nonpositive numbers are allowed in radius." << std::endl;
			return 1;
		}
	}

	/** Run the program. */
  bool supported = false;
	try
	{
		run( GrayscaleMorphologicalClosing, unsigned char, 2 );
		run( GrayscaleMorphologicalClosing, unsigned char, 3 );
		run( GrayscaleMorphologicalClosing, char, 2 );
		run( GrayscaleMorphologicalClosing, char, 3 );
		run( GrayscaleMorphologicalClosing, unsigned short, 2 );
		run( GrayscaleMorphologicalClosing, unsigned short, 3 );
		run( GrayscaleMorphologicalClosing, short, 2 );
		run( GrayscaleMorphologicalClosing, short, 3 );
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
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ******************* GrayscaleMorphologicalClosing *******************
	 */

template< class InputImageType >
void GrayscaleMorphologicalClosing(
  const std::string & inputFileName,
  const std::string & outputFileName,
	const std::vector<unsigned int> & radius )
{
	/** Typedefs. */
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;

	typedef typename InputImageType::PixelType					PixelType;
	const unsigned int Dimension = InputImageType::ImageDimension;
	
	typedef itk::BinaryBallStructuringElement<
		PixelType, Dimension >														StructuringElementType;
	typedef typename StructuringElementType::RadiusType					RadiusType;
	typedef itk::GrayscaleMorphologicalClosingImageFilter<
		InputImageType, InputImageType,
		StructuringElementType >													ClosingFilterType;

	/** DECLARATION'S. */
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();
	typename ClosingFilterType::Pointer closing = ClosingFilterType::New();
	RadiusType	radiusarray;
	StructuringElementType	S_ball;

	/** Setup the reader. */
	reader->SetFileName( inputFileName.c_str() );

	/** Create and fill the radius. */
	radiusarray.Fill( 1 );
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		radiusarray.SetElement( i, radius[ i ] );
	}

	/** Create the structuring element and set it into the closing filter. */
	S_ball.SetRadius( radiusarray );
	S_ball.CreateStructuringElement();
	closing->SetKernel( S_ball );

	/** Connect the pipeline. */
	closing->SetInput( reader->GetOutput() );

	/** Write the output image. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( closing->GetOutput() );
	writer->Update();

} // end GrayscaleMorphologicalClosing


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp( void )
{
	std::cout << "Usage:" << std::endl << "pxmorphologicalclosingimage" << std::endl;
	std::cout << "\t-in\tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in + CLOSED.mhd" << std::endl;
	std::cout << "\t-r\tradius" << std::endl;
	std::cout << "\t[-dim]\tdimension, default 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType, default short" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp

