
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
    typedef  itk::Image< type, dim >   InputImageType; \
    function< InputImageType >( inputFileName, outputFileName, minimum, maximum ); \
}

//-------------------------------------------------------------------------------------

/** Declare RescaleIntensity. */
template< class InputImageType >
void RescaleIntensity( std::string inputFileName, std::string outputFileName, double minimum, double maximum );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check number of arguments. */
	if ( argc < 3 || argc > 12 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get input file name. */
	std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  /** Determine input image properties. */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Let the user overrule this. */
	bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  
  /** Error checking. */
  if ( NumberOfComponents > 1 )
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get the output file name. */
	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "INTENSITYRESCALED.mhd";
	bool ret2 = parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get the extrema. */
	std::vector<double> extrema(2);
	extrema[ 0 ] = 0.0; extrema[ 1 ] = 0.0;
	bool ret3 = parser->GetCommandLineArgument( "-mm", extrema );
	double minimum = extrema[ 0 ];
	double maximum = extrema[ 1 ];

	//std::string	Dimension = "2";
	//bool ret4 = parser->GetCommandLineArgument( "-dim", Dimension );

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

	/** Get rid of the possible "_" in ComponentType. */
	ReplaceUnderscoreWithSpace( ComponentType );

	/** Run the program. */
	try
	{
		run(RescaleIntensity,unsigned char,2);
		run(RescaleIntensity,unsigned char,3);
		run(RescaleIntensity,char,2);
		run(RescaleIntensity,char,3);
		run(RescaleIntensity,unsigned short,2);
		run(RescaleIntensity,unsigned short,3);
		run(RescaleIntensity,short,2);
		run(RescaleIntensity,short,3);
    run(RescaleIntensity,float,2);
		run(RescaleIntensity,float,3);
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
 * ******************* RescaleIntensity *******************
 */

template< class InputImageType >
void RescaleIntensity( std::string inputFileName, std::string outputFileName, double minimum, double maximum )
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

} // end RescaleIntensity

/**
 * ******************* PrintHelp *******************
 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxrescaleintensityimagefilter" << std::endl;
	std::cout << "\t-in   \tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in + INTENSITYRESCALED.mhd" << std::endl;
	std::cout << "\t[-mm] \tminimum maximum, default range of pixeltype" << std::endl;
  std::cout << "\t[-pt] \tpixel type of input and output images;" << std::endl;
  std::cout << "\t      \tdefault: automatically determined from the first input image." << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char, float." << std::endl;

} // end PrintHelp
