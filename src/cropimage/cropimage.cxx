#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkCropImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, input1, input2, option ); \
}

//-------------------------------------------------------------------------------------

/** Declare CropImage. */
template< class InputImageType >
void CropImage( std::string inputFileName, std::string outputFileName,
	std::vector<unsigned int> input1, std::vector<unsigned int> input2, unsigned int option );

/** Declare other functions. */
void PrintHelp(void);

bool CheckWhichInputOption( bool pAGiven, bool pBGiven, bool szGiven,
	bool lbGiven, bool ubGiven, unsigned int & arg );

bool ProcessArgument( std::vector<unsigned int> & arg, unsigned int dimension );

void GetBox( std::vector<unsigned int> & pA, std::vector<unsigned int> & pB, unsigned int dimension );

std::vector<unsigned int> GetUpperBoundary( const std::vector<unsigned int> & input1,
	 const std::vector<unsigned int> & input2, const std::vector<unsigned int> & imageSize,
	 unsigned int dimension, unsigned int option );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 5 || argc > 17 )
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
	outputFileName += "CROPPED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	std::vector<unsigned int> pA;
	bool retpA = parser->GetCommandLineArgument( "-pA", pA );

	std::vector<unsigned int> pB;
	bool retpB = parser->GetCommandLineArgument( "-pB", pB );

	std::vector<unsigned int> sz;
	bool retsz = parser->GetCommandLineArgument( "-sz", sz );

	std::vector<unsigned int> lowBound;
	bool retlb = parser->GetCommandLineArgument( "-lb", lowBound );

	std::vector<unsigned int> upBound;
	bool retub = parser->GetCommandLineArgument( "-ub", upBound );

	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

	std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

	/** Check which input option is used:
	 * 1: supply two points with -pA and -pB
	 * 2: supply a points and a size with -pA and -sz
	 * 3: supply a lower and an upper bound with -lb and -ub
	 */
	unsigned int option = 0;
	if ( !CheckWhichInputOption( retpA, retpB, retsz, retlb, retub, option ) )
	{
		std::cerr << "ERROR: Check your commandline arguments." << std::endl;
		return 1;
	}
	
	/** Check argument pA. */
	if ( retpA )
	{
		if ( !ProcessArgument( pA, Dimension ) )
		{
			std::cout << "ERROR: Point A should consist of 1 or Dimension positive values." << std::endl;
			return 1;
		}
	}

	/** Check argument pB. */
	if ( retpB )
	{
		if ( !ProcessArgument( pB, Dimension ) )
		{
			std::cout << "ERROR: Point B should consist of 1 or Dimension positive values." << std::endl;
			return 1;
		}
	}

	/** Check argument sz. */
	if ( retsz )
	{
		if ( !ProcessArgument( sz, Dimension ) )
		{
			std::cout << "ERROR: The size sz should consist of 1 or Dimension positive values." << std::endl;
			return 1;
		}
	}
	
	/** Check argument lb. */
	if ( retlb )
	{
		if ( !ProcessArgument( lowBound, Dimension ) )
		{
			std::cout << "ERROR: The lowerbound lb should consist of 1 or Dimension positive values." << std::endl;
			return 1;
		}
	}

	/** Check argument ub. */
	if ( retub )
	{
		if ( !ProcessArgument( upBound, Dimension ) )
		{
			std::cout << "ERROR: The upperbound ub should consist of 1 or Dimension positive values." << std::endl;
			return 1;
		}
	}

	/** Get inputs. */
	std::vector<unsigned int> input1, input2;
	if ( option == 1 )
	{
		GetBox( pA, pB, Dimension );
		input1 = pA;
		input2 = pB;
	}
	else if ( option == 2 )
	{
		input1 = pA;
		input2 = sz;
	}
	else if ( option == 3 )
	{
		input1 = lowBound;
		input2 = upBound;
	}

	/** Run the program. */
	try
	{
		run(CropImage,unsigned char,2);
		run(CropImage,unsigned char,3);
		run(CropImage,char,2);
		run(CropImage,char,3);
		run(CropImage,unsigned short,2);
		run(CropImage,unsigned short,3);
		run(CropImage,short,2);
		run(CropImage,short,3);
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
	 * ******************* CropImage *******************
	 */

template< class InputImageType >
void CropImage( std::string inputFileName, std::string outputFileName,
	std::vector<unsigned int> input1, std::vector<unsigned int> input2, unsigned int option )
{
	/** Typedefs. */
	typedef itk::CropImageFilter< InputImageType, InputImageType >	CropImageFilterType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;
	typedef typename InputImageType::SizeType					SizeType;

	const unsigned int Dimension = InputImageType::ImageDimension;

	/** Declarations. */
	typename CropImageFilterType::Pointer cropFilter = CropImageFilterType::New();
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();

	/** Prepare stuff. */
	SizeType input1Size, input2Size;
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		input1Size[ i ] = input1[ i ];
		input2Size[ i ] = input2[ i ];
	}

	/** Read the image. */
	reader->SetFileName( inputFileName.c_str() );
	reader->Update();

	/** Get the size of input image. */
	SizeType imageSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
	std::vector<unsigned int> imSize( Dimension );
	for ( unsigned int i = 0; i < Dimension; i++ ) imSize[ i ] = imageSize[ i ];

	/** Get the upper boundary. */
	std::vector<unsigned int> up = GetUpperBoundary( input1, input2, imSize, Dimension, option );
	SizeType upSize;
	for ( unsigned int i = 0; i < Dimension; i++ ) upSize[ i ] = up[ i ];

	/** Set the boundaries. */
	cropFilter->SetLowerBoundaryCropSize( input1Size );
	cropFilter->SetUpperBoundaryCropSize( upSize );

	/** Setup and process the pipeline. */
	cropFilter->SetInput( reader->GetOutput() );
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( cropFilter->GetOutput() );
	writer->Update();

} // end CropImage


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxcropimage" << std::endl;
	std::cout << "\t-in\tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in + CROPPED.mhd" << std::endl;
	std::cout << "\t[-pA]\ta point A" << std::endl;
	std::cout << "\t[-pB]\ta point B" << std::endl;
	std::cout << "\t[-sz]\tsize" << std::endl;
	std::cout << "\t[-lb]\tlower bound" << std::endl;
	std::cout << "\t[-ub]\tupper bound" << std::endl;
	std::cout << "\t[-dim]\tdimension, default 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType, default short" << std::endl;
	std::cout << "pxcropimage can be called in different ways:" << std::endl;
	std::cout << "\t1: supply two points with \"-pA\" and \"-pB\"." << std::endl;
	std::cout << "\t2: supply a points and a size with \"-pA\" and \"-sz\"." << std::endl;
	std::cout << "\t3: supply a lower and an upper bound with \"-lb\" and \"-ub\"." << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp


	/**
	 * ******************* CheckWhichInputOption *******************
	 *
	 * 1: supply two points with -pA and -pB
	 * 2: supply a points and a size with -pA and -sz
	 * 3: supply a lower and an upper bound with -lb and -ub
	 */
bool CheckWhichInputOption( bool pAGiven, bool pBGiven, bool szGiven,
	bool lbGiven, bool ubGiven, unsigned int & arg )
{
	if ( pAGiven && pBGiven && !szGiven && !lbGiven && !ubGiven )
	{
		/** Two points given. */
		arg = 1;
		return true;
	}
	else if ( pAGiven && !pBGiven && szGiven && !lbGiven && !ubGiven )
	{
		/** A point and a size given. */
		arg = 2;
		return true;
	}
	else if ( !pAGiven && !pBGiven && !szGiven && lbGiven && ubGiven )
	{
		/** A lower and an upper bound given. */
		arg = 3;
		return true;
	}

	/** Return a value. */
	return false;

} // end CheckWhichInputOption


	/**
	 * ******************* ProcessArgument *******************
	 */

bool ProcessArgument( std::vector<unsigned int> & arg, unsigned int dimension )
{
	/** Check if arg is of the right size. */
	if( arg.size() != dimension && arg.size() != 1 )
	{	
		return false;
	}

	/** Create a vector arg2 of size dimension, with values:
	 * - ( arg[0], ..., arg[0] ) if arg.size() == 1
	 * - ( arg[0], ..., arg[dimension-1] ) if arg.size() == dimension
	 */
	std::vector<unsigned int> arg2( dimension, arg[ 0 ] );
	if ( arg.size() == dimension )
	{
		for ( unsigned int i = 1; i < dimension; i++ )
		{
			arg2[ i ] = arg[ i ];
		}
	}

	/** Substitute arg2 for arg. */
	arg = arg2;

	/** Check for positive numbers. */
	for ( unsigned int i = 1; i < dimension; i++ )
	{
		if ( arg[ i ] < 0 ) return false;
	}

	/** Return a value. */
	return true;
  
} // end ProcessArgument


	/**
	 * ******************* GetBox *******************
	 */

void GetBox( std::vector<unsigned int> & pA, std::vector<unsigned int> & pB, unsigned int dimension )
{
	/** Get the outer points of the box. */
	std::vector<unsigned int> pa( dimension, 0 );
	std::vector<unsigned int> pb( dimension, 0 );
	for ( unsigned int i = 0; i < dimension; i++ )
	{
		pa[ i ] = vnl_math_min( pA[ i ], pB[ i ] );
		pb[ i ] = vnl_math_max( pA[ i ], pB[ i ] );
	}

	/** Copy to the input variables. */
	pA = pa; pB = pb;

} // end GetBox

	/**
	 * ******************* GetUpperBoundary *******************
	 */

 std::vector<unsigned int> GetUpperBoundary( const std::vector<unsigned int> & input1,
	 const std::vector<unsigned int> & input2, const std::vector<unsigned int> & imageSize,
	 unsigned int dimension, unsigned int option )
{
	/** Create output vector. */
	std::vector<unsigned int > output( dimension, 0 );
	
	/** Fill output vector. */
	if ( option == 1 )
	{
		for ( unsigned int i = 0; i < dimension; i++ )
		{
			if ( imageSize[ i ] < input2[ i ] )
			{
				itkGenericExceptionMacro( << "out of bounds." );
			}
			if ( input1[ i ] == input2[ i ] )
			{
				itkGenericExceptionMacro( << "size[" << i << "] = 0" );
			}
			output[ i ] = imageSize[ i ] - input2[ i ];
		}
	}
	else if ( option == 2 )
	{
		for ( unsigned int i = 0; i < dimension; i++ )
		{
			if ( imageSize[ i ] < input1[ i ] + input2[ i ] )
			{
				itkGenericExceptionMacro( << "out of bounds." );
			}

			if ( input2[ i ] == 0 )
			{
				itkGenericExceptionMacro( << "size[" << i << "] = 0" );
			}
			output[ i ] = imageSize[ i ] - input1[ i ] - input2[ i ];
		}
	}
	else if ( option == 3 )
	{
		for ( unsigned int i = 0; i < dimension; i++ )
		{
			if ( imageSize[ i ] < input1[ i ] + input2[ i ] )
			{
				itkGenericExceptionMacro( << "out of bounds." );
			}
			if ( input1[ i ] + input2[ i ] == imageSize[ i ] )
			{
				itkGenericExceptionMacro( << "size[" << i << "] = 0" );
			}
			output[ i ] = input2[ i ];
		}
	} // end if

	/** Return output. */
	return output;

} // end GetUpperBoundary

