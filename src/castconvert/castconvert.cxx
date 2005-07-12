// This process not only involves casting but also rescaling the image
//  intensity since the dynamic range of the input and output pixel types can
//  be quite different.  The \doxygen{RescaleIntensityImageFilter} is used
//  here to linearly rescale the image values.

#include "itkImage.h"
#include "itkImageIORegion.h"

#include "itkRGBPixel.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// One of these is used
#include "itkShiftScaleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

/**	Declare	some functions.	*/
template<	class	InputImageType,	class	OutputImageType	>
void ReadCastWriteImage( std::string inputFileName,	std::string	outputFileName );

template<	class	ReaderType,	class	WriterType >
void PrintInfo(	ReaderType reader, WriterType	writer );

//-------------------------------------------------------------------------------------

int	main(	int	argc,	char *argv[] )
{
	/**	Check	number of	arguments. */
	if ( argc	< 3 || argv[ 1 ] == "--help" )
	{
		std::cout	<< "Usage:"	<< std::endl;
		std::cout	<< "\tconvert inputfilename outputfilename [outputPixelType]" << std::endl;
		std::cout	<< "\twhere outputPixelType is one of:" << std::endl;
		std::cout	<< "\t\t- unsigned_char" << std::endl;
		std::cout	<< "\t\t- char" << std::endl;
		std::cout	<< "\t\t- unsigned_short" << std::endl;
		std::cout	<< "\t\t- short" << std::endl;
		std::cout	<< "\t\t- unsigned_long" << std::endl;
		std::cout	<< "\t\t- long" << std::endl;
		std::cout	<< "\t\t- float" << std::endl;
		std::cout	<< "\t\t- double" << std::endl;
		std::cout	<< "\tBy default the outputPixelType is set to the inputPixelTtype." << std::endl;
		return 1;
	}

	/**	Get	the	filenames. */
	std::string	inputFileName	=	argv[	1	];
	std::string	outputFileName = argv[ 2 ];

	/** Get the output pixeltype. */
	std::string outputPixelType = "";
	if ( argc == 4 ) outputPixelType = argv[ 3 ];

	/**	Some consts. */
	const	unsigned int	Dimension	=	3;

	/** Some typedef's. */
	typedef	short		PixelType;

	/**	Some typedef's.	*/
	typedef	itk::Image<	PixelType, Dimension >			ImageType;
	typedef	itk::ImageFileReader<	ImageType	>				ReaderType;
	typedef	itk::ImageIOBase												ImageIOBaseType;

	/**	Create a test	reader and IOBase	in order to	extract	the	correct	image	type.	*/
	ReaderType::Pointer	testReader = ReaderType::New();
	testReader->SetFileName( inputFileName.c_str() );
	testReader->GenerateOutputInformation();
	ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();

	/**	Get	the	component	type,	number of	components,	dimension	and	pixel	type.	*/
	unsigned int dimension = testImageIOBase->GetNumberOfDimensions();
	unsigned int numberOfComponents	=	testImageIOBase->GetNumberOfComponents();
	std::string	iOComponent	=	testImageIOBase->GetComponentTypeAsString(
		testImageIOBase->GetComponentType()	);
	std::string	pixelType	=	testImageIOBase->GetPixelTypeAsString(
		testImageIOBase->GetPixelType()	);

	/** Get the size of the image. */
	std::vector<unsigned int> size(dimension);
	for ( unsigned int i = 0; i < dimension; i++ )
	{
		size[ i ] = testImageIOBase->GetDimensions( i );
	}

	/** Check outputPixelType. */
	if ( outputPixelType == "" )
	{
		/** In this case this option is not given, and by default
		 * we set it to the inputPixelType.
		 */
		outputPixelType = iOComponent;
	}
	else if ( outputPixelType != "unsigned_char"
		&& outputPixelType != "char"
		&& outputPixelType != "unsigned_short"
		&& outputPixelType != "short"
		&& outputPixelType != "unsigned_long"
		&& outputPixelType != "long"
		&& outputPixelType != "float"
		&& outputPixelType != "double" )
	{
		/** In this case an illegal outputPixelType is given. */
		std::cerr << "The given outputPixelType is \"" << outputPixelType
			<< "\", which is not supported." << std::endl;
		return 1;
	}

	/**	Call the correct ReadWriteImage(). */
	if ( dimension ==	2	)
	{
		/**
		 * ****************** Support for SCALAR pixel types. **********************************
		 */
		if ( strcmp( pixelType.c_str(),	"scalar" ) == 0	&& numberOfComponents	== 1 )
		{
			/** From unsigned char to something else. */
			if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned char, 2 >	InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From char to something else. */
			else if ( iOComponent == "char" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "char" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "short" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "long" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "float" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "double" )
			{
				typedef	itk::Image<	char, 2 >						InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From unsigned short to something else. */
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned short, 2 >	InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From short to something else. */
			else if ( iOComponent == "short" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "char" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "short" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "long" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "float" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "double" )
			{
				typedef	itk::Image<	short, 2 >					InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From unsigned long to something else. */
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned long, 2 >	InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From long to something else. */
			else if ( iOComponent == "long" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "char" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "short" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "long" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "float" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "double" )
			{
				typedef	itk::Image<	long, 2 >						InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
						/** From float to something else. */
			else if ( iOComponent == "float" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "char" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "short" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "long" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "float" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "double" )
			{
				typedef	itk::Image<	float, 2 >					InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From double to something else. */
			else if ( iOComponent == "double" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	unsigned char, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "char" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	char, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	unsigned short, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "short" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	short, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	unsigned long, 2 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "long" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	long, 2 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "float" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	float, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "double" )
			{
				typedef	itk::Image<	double, 2 >					InputImageType;
				typedef	itk::Image<	double, 2 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** Error checking. */
			else if ( iOComponent == "unknown" )
			{
				std::cerr	<< "ComponentType of input image unknown."	<< std::endl;
				return -1;
			}
			else
			{
				std::cerr	<< "ComponentType not supported."	<< std::endl;
				return -1;
			}
		} // end support for SCALAR pixel type
		/**
		 * ****************** Support for RGB pixel types. **********************************
		 *
		else if ( strcmp( pixelType.c_str(),	"rgb" ) == 0	&& numberOfComponents	== 3 )
		{
			if ( strcmp( iOComponent.c_str(),	"unsigned_char"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned char >		PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "char"	)	== 0 )
			{
				typedef itk::RGBPixel< char >							PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_short"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned short >		PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "short" ) ==	0	)
			{
				typedef itk::RGBPixel< short >						PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_int"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned int >			PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "int" ) ==	0	)
			{
				typedef itk::RGBPixel< int >							PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_long" ) ==	0	)
			{
				typedef itk::RGBPixel< unsigned long >		PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "long"	)	== 0 )
			{
				typedef itk::RGBPixel< long >							PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "float" ) ==	0	)
			{
				typedef itk::RGBPixel< float >						PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "double"	)	== 0 )
			{
				typedef itk::RGBPixel< double >						PixelType;
				typedef	itk::Image<	PixelType, 2 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unknown" ) ==	0	)
			{
				std::cerr	<< "ComponentType unknown."	<< std::endl;
				return -1;
			}
			else
			{
				std::cerr	<< "ComponentType not supported."	<< std::endl;
				return -1;
			}
		} // end Support for RGB pixel type */
		else
		{
			std::cerr	<< "Pixel type is "	<< pixelType
				<< ", component type is " << iOComponent
				<< " and number of components equals " <<	numberOfComponents <<	"."	<< std::endl;
			std::cerr	<< "ERROR: This image type is not supported."	<< std::endl;
			return -1;
		}
	} // end 2D case
	else if	(	dimension	== 3 )
	{
		/**
		 * ****************** Support for SCALAR pixel types. **********************************
		 */
		if ( strcmp( pixelType.c_str(),	"scalar" ) == 0	&& numberOfComponents	== 1 )
		{
			/** From unsigned char to something else. */
			if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_char" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_char" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned char, 3 >	InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From char to something else. */
			else if ( iOComponent == "char" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "char" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "short" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "long" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "char" && outputPixelType == "float" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "char" && outputPixelType == "double" )
			{
				typedef	itk::Image<	char, 3 >						InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From unsigned short to something else. */
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_short" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_short" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned short, 3 >	InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From short to something else. */
			else if ( iOComponent == "short" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "char" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "short" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "long" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "short" && outputPixelType == "float" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "short" && outputPixelType == "double" )
			{
				typedef	itk::Image<	short, 3 >					InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From unsigned long to something else. */
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "char" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "short" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "long" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "unsigned_long" && outputPixelType == "float" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "unsigned_long" && outputPixelType == "double" )
			{
				typedef	itk::Image<	unsigned long, 3 >	InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From long to something else. */
			else if ( iOComponent == "long" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "char" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "short" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "long" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "long" && outputPixelType == "float" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "long" && outputPixelType == "double" )
			{
				typedef	itk::Image<	long, 3 >						InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
						/** From float to something else. */
			else if ( iOComponent == "float" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "char" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "short" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "long" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "float" && outputPixelType == "float" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "float" && outputPixelType == "double" )
			{
				typedef	itk::Image<	float, 3 >					InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** From double to something else. */
			else if ( iOComponent == "double" && outputPixelType == "unsigned_char" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	unsigned char, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "char" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	char, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "unsigned_short" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	unsigned short, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "short" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	short, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "unsigned_long" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	unsigned long, 3 >	OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "long" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	long, 3 >						OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if ( iOComponent == "double" && outputPixelType == "float" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	float, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			else if	( iOComponent == "double" && outputPixelType == "double" )
			{
				typedef	itk::Image<	double, 3 >					InputImageType;
				typedef	itk::Image<	double, 3 >					OutputImageType;
				ReadCastWriteImage<	InputImageType, OutputImageType >(	inputFileName, outputFileName	);
			}
			/** Error checking. */
			else if ( iOComponent == "unknown" )
			{
				std::cerr	<< "ComponentType of input image unknown."	<< std::endl;
				return -1;
			}
			else
			{
				std::cerr	<< "ComponentType not supported."	<< std::endl;
				return -1;
			}
		} // end support for SCALAR pixel type
		/**
		 * ****************** Support for RGB pixel types. **********************************
		 *
		else if ( strcmp( pixelType.c_str(),	"rgb" ) == 0	&& numberOfComponents	== 3 )
		{
			if ( strcmp( iOComponent.c_str(),	"unsigned_char"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned char >		PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "char"	)	== 0 )
			{
				typedef itk::RGBPixel< char >							PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_short"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned short >		PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "short" ) ==	0	)
			{
				typedef itk::RGBPixel< short >						PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_int"	)	== 0 )
			{
				typedef itk::RGBPixel< unsigned int >			PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "int" ) ==	0	)
			{
				typedef itk::RGBPixel< int >							PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unsigned_long" ) ==	0	)
			{
				typedef itk::RGBPixel< unsigned long >		PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "long"	)	== 0 )
			{
				typedef itk::RGBPixel< long >							PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "float" ) ==	0	)
			{
				typedef itk::RGBPixel< float >						PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "double"	)	== 0 )
			{
				typedef itk::RGBPixel< double >						PixelType;
				typedef	itk::Image<	PixelType, 3 >				ImageType;
				ReadWriteImage<	ImageType, ImageType >(	inputFileName, outputFileName	);
			}
			else if	(	strcmp(	iOComponent.c_str(), "unknown" ) ==	0	)
			{
				std::cerr	<< "ComponentType unknown."	<< std::endl;
				return -1;
			}
			else
			{
				std::cerr	<< "ComponentType not supported."	<< std::endl;
				return -1;
			}
		} // end Support for RGB pixel type */
		else
		{
			std::cerr	<< "Pixel type is "	<< pixelType
				<< ", component type is " << iOComponent
				<< " and number of components equals " <<	numberOfComponents <<	"."	<< std::endl;
			std::cerr	<< "ERROR: This image type is not supported."	<< std::endl;
			return -1;
		}
	} // end 3D case
	else
	{
		std::cerr	<< "Dimension equals " << dimension << " ( ";
		for ( unsigned int i = 0; i < dimension; i++ )
		{
			std::cerr << size[ i ] << " ";
		}
		std::cerr << "), which is not supported."	<< std::endl;
		std::cerr	<< "Only 2D and 3D images are supported."	<< std::endl;
		return -1;
	}

	/**	End	program. */
	return 0;

}	// end main


/**	The	function that	reads	the	input	image	and	writes the output	image.
* This	function is	templated	over the image types.	In the main	function
* we	have to	make sure	to call	the	right	instantiation.
*/
template<	class	InputImageType,	class	OutputImageType	>
void ReadCastWriteImage( std::string inputFileName,	std::string	outputFileName )
{
	/**	Create the correct reader	and	writer.	*/
	typedef	typename itk::ImageFileReader< InputImageType	>			ImageReaderType;
	typedef typename itk::RescaleIntensityImageFilter<
		InputImageType, OutputImageType >													RescaleFilterType;
	typedef typename itk::ShiftScaleImageFilter<
		InputImageType, OutputImageType >													ShiftScaleFilterType;
	typedef	typename itk::ImageFileWriter< OutputImageType >		ImageWriterType;
	
	typename ImageReaderType::Pointer	reader = ImageReaderType::New();
	//typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
	typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
	typename ImageWriterType::Pointer	writer = ImageWriterType::New();

	/**	Set	the	filenames	and	connect	the	pipeline.	*/
	reader->SetFileName( inputFileName.c_str() );
	writer->SetFileName( outputFileName.c_str()	);
	caster->SetShift( 0.0 );
	caster->SetScale( 1.0 );

	/** Connect the pipeline. */
	caster->SetInput(	reader->GetOutput()	);
	writer->SetInput(	caster->GetOutput()	);

	/**	Do the actual	conversion.	*/
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject	&	err	)
	{
		std::cerr	<< "ExceptionObject caught !"	<< std::endl;
		std::cerr	<< err <<	std::endl;
	}

	/**	Print	information. */
	PrintInfo( reader, writer	);

}	// end ReadWriteImage


/**
*
*/
template<	class	ReaderType,	class	WriterType >
void PrintInfo(	ReaderType reader, WriterType	writer )
{
	/**	Typedef's. */
	typedef	itk::ImageIOBase												ImageIOBaseType;
	typedef	itk::ImageIORegion											ImageIORegionType;
	typedef	typename ImageIORegionType::SizeType		SizeType;

	/**	Get	IOBase of	the	reader and extract information.	*/
	ImageIOBaseType::Pointer imageIOBaseIn = reader->GetImageIO();
	ImageIORegionType	iORegionIn = imageIOBaseIn->GetIORegion();

	const	char * fileNameIn	=	imageIOBaseIn->GetFileName();
	std::string	pixelTypeIn	=	imageIOBaseIn->GetPixelTypeAsString( imageIOBaseIn->GetPixelType() );
	unsigned int nocIn = imageIOBaseIn->GetNumberOfComponents();
	std::string	componentTypeIn	=	imageIOBaseIn->GetComponentTypeAsString( imageIOBaseIn->GetComponentType() );
	unsigned int dimensionIn = imageIOBaseIn->GetNumberOfDimensions();
	SizeType sizeIn	=	iORegionIn.GetSize();

	/**	Get	IOBase of	the	writer and extract information.	*/
	ImageIOBaseType::Pointer imageIOBaseOut	=	writer->GetImageIO();
	ImageIORegionType	iORegionOut	=	imageIOBaseOut->GetIORegion();

	const	char * fileNameOut = imageIOBaseOut->GetFileName();
	std::string	pixelTypeOut = imageIOBaseOut->GetPixelTypeAsString( imageIOBaseOut->GetPixelType()	);
	unsigned int nocOut	=	imageIOBaseOut->GetNumberOfComponents();
	std::string	componentTypeOut = imageIOBaseOut->GetComponentTypeAsString( imageIOBaseOut->GetComponentType()	);
	unsigned int dimensionOut	=	imageIOBaseOut->GetNumberOfDimensions();
	SizeType sizeOut = iORegionOut.GetSize();

	/**	Print	information. */
	std::cout	<< "Information about the input image \""	<< fileNameIn	<< "\":" <<	std::endl;
	std::cout	<< "\tdimension:\t\t"	<< dimensionIn <<	std::endl;
	std::cout	<< "\tpixel type:\t\t" <<	pixelTypeIn	<< std::endl;
	std::cout	<< "\tnumber of components:\t" <<	nocIn	<< std::endl;
	std::cout	<< "\tcomponent type:\t\t" <<	componentTypeIn	<< std::endl;
	std::cout	<< "\tsize:\t\t\t";
	for	(	unsigned int i = 0;	i	<	dimensionIn; i++ ) std::cout <<	sizeIn[	i	]	<< " ";
	std::cout	<< std::endl;

	/**	Print	information. */
	std::cout	<< std::endl;
	std::cout	<< "Information about the output image \"" <<	fileNameOut	<< "\":" <<	std::endl;
	std::cout	<< "\tdimension:\t\t"	<< dimensionOut	<< std::endl;
	std::cout	<< "\tpixel type:\t\t" <<	pixelTypeOut <<	std::endl;
	std::cout	<< "\tnumber of components:\t" <<	nocOut <<	std::endl;
	std::cout	<< "\tcomponent type:\t\t" <<	componentTypeOut <<	std::endl;
	std::cout	<< "\tsize:\t\t\t";
	for	(	unsigned int i = 0;	i	<	dimensionOut;	i++	)	std::cout	<< sizeOut[	i	]	<< " ";
	std::cout	<< std::endl;

}	// end PrintInfo
