/**
 * castconvert
 *
 * This program converts and possibly casts images.
 *
 * This is done by reading in an image, possibly casting of the image,
 * and subsequently writing the image to some format.
 * With converting we mean changing the extension of the image,
 * such as bmp, mhd, etc. With casting we mean changing the component
 * type of a voxel, such as short, unsigned long, float.
 *
 * Casting is currently done using the ShiftScaleImageFilter,
 * where values are mapped to itself, leaving the intensity range
 * the same. NOTE that when casting to a component type with a
 * smaller dynamic range, information might get lost. In this case
 * we might use the RescaleIntensityImageFilter to linearly
 * rescale the image values.
 *
 * Currently only supported are the SCALAR pixel types.
 * Input images can be in all file formats ITK supports and for which
 * the ImageFileReader works, and additionally single 3D dicom series
 * using the ImageSeriesReader. The pixel component type should
 * of course be a component type supported by the file format.
 * Output images can be in all file formats ITK supports and for which
 * the ImageFileReader works, so no dicom output is currently supported.
 *
 */

/** Basic Image support. */
#include "itkImage.h"
#include "itkImageIORegion.h"

/** For the support of RGB voxels. */
//#include "itkRGBPixel.h"

/** Reading and writing images. */
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

/** DICOM headers. We had some problems with the GDCM version
 * from ITK 2.0, and decided to use DICOMImageIO2.
 * Later we can easily change this to GDCM again.
 */
//#include "itkGDCMImageIO.h"
//#include "itkGDCMSeriesFileNames.h"
#include "itkDICOMImageIO2.h"
#include "itkDICOMSeriesFileNames.h"

/** One of these is used to cast the image. */
#include "itkShiftScaleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

/** In order to determine if argv[1] is a directory or a file,
 * so that we can distinguish between dicom and other files.
 */
#include <itksys/SystemTools.hxx>

/** Declare a function to do the actual conversion.
 * ReadCastWriteImage() is for non-dicom images.
 */
template<	class	InputImageType,	class	OutputImageType	>
void ReadCastWriteImage( std::string inputFileName,	std::string	outputFileName );

/** Declare a function to do the actual conversion.
 * ReadDicomSeriesCastWriteImage() is for dicom images. */
template<	class	InputImageType,	class	OutputImageType	>
void ReadDicomSeriesCastWriteImage( std::string inputDirectoryName,	std::string	outputFileName );

/** Declare a function to print image information. */
template<	class	ReaderType,	class	WriterType >
void PrintInfo(	ReaderType reader, WriterType	writer );

/** Macros are used in order to make the code in main() look cleaner. */

/** callCorrectReadWriterMacro:
 * A macro to call the conversion function.
 */

#define callCorrectReadWriterMacro(typeIn,typeOut,dim) \
	if ( inputPixelComponentType == #typeIn && outputPixelComponentType == #typeOut && inputDimension == dim) \
		{ \
			typedef	itk::Image< typeIn, dim >		InputImageType; \
			typedef	itk::Image< typeOut, dim >	OutputImageType; \
			ReadCastWriteImage< InputImageType, OutputImageType >( inputFileName, outputFileName ); \
		}

/** callCorrectReadDicomWriterMacro:
 * A macro to call the dicom-conversion function.
 */

#define callCorrectReadDicomWriterMacro(typeIn,typeOut) \
	if ( inputPixelComponentType == #typeIn && outputPixelComponentType == #typeOut ) \
		{ \
			typedef	itk::Image< typeIn, 3 >		InputImageType; \
			typedef	itk::Image< typeOut, 3 >	OutputImageType; \
			ReadDicomSeriesCastWriteImage< InputImageType, OutputImageType >( inputDirectoryName, outputFileName ); \
		}

//-------------------------------------------------------------------------------------

int	main(	int	argc,	char *argv[] )
{
	/** TASK 1:
	 * Check arguments.
	 * *******************************************************************
	 */
	if ( argc	< 3 || argv[ 1 ] == "--help" )
	{
		std::cout	<< "Usage:"	<< std::endl;
		std::cout	<< "\tpxcastconvert inputfilename outputfilename [outputPixelComponentType]" << std::endl;
		std::cout	<< "\tpxcastconvert dicomDirectory outputfilename [outputPixelComponentType]" << std::endl;
		std::cout	<< "\twhere outputPixelComponentType is one of:" << std::endl;
		std::cout	<< "\t\t- unsigned_char" << std::endl;
		std::cout	<< "\t\t- char" << std::endl;
		std::cout	<< "\t\t- unsigned_short" << std::endl;
		std::cout	<< "\t\t- short" << std::endl;
		std::cout	<< "\t\t- unsigned_int" << std::endl;
		std::cout	<< "\t\t- int" << std::endl;
		std::cout	<< "\t\t- unsigned_long" << std::endl;
		std::cout	<< "\t\t- long" << std::endl;
		std::cout	<< "\t\t- float" << std::endl;
		std::cout	<< "\t\t- double" << std::endl;
		std::cout	<< "\tprovided that the outputPixelComponentType is supported by the output file format." << std::endl;
		std::cout	<< "\tBy default the outputPixelComponentType is set to the inputPixelComponentType." << std::endl;
		return 1;
	}

	/**	Get	the	inputs. */
	std::string	input = argv[ 1 ];
	std::string outputFileName = argv[ 2 ];
	std::string outputPixelComponentType = "";
	if ( argc == 4 ) outputPixelComponentType = argv[ 3 ];

	/** Make sure last character of input != "/".
	 * Otherwise FileIsDirectory() won't work.
	 */
	if ( input.rfind( "/" ) == input.size() - 1 )
	{
		input.erase( input.size() - 1, 1 );
	}

	/** Check if input is a file or a directory. */
	bool exists = itksys::SystemTools::FileExists( input.c_str() );
	bool isDir = itksys::SystemTools::FileIsDirectory( input.c_str() );
	bool isDICOM = false;
	std::string inputFileName, inputDirectoryName;

	if ( exists && !isDir )
	{
		/** Input is a file, and we use the ImageFileReader. */
		inputFileName = input;
	}
	else if ( exists && isDir )
	{
		/** Input is a directory, and we use the ImageSeriesReader. */
		inputDirectoryName = input;
		isDICOM = true;
	}
	else
	{
		/** Something is wrong. */
		std::cerr << "ERROR: first input argument does not exist!" << std::endl;
		return 1;
	}

	/** Check outputPixelType. */
	if ( outputPixelComponentType != ""
		&& outputPixelComponentType != "unsigned_char"
		&& outputPixelComponentType != "char"
		&& outputPixelComponentType != "unsigned_short"
		&& outputPixelComponentType != "short"
		&& outputPixelComponentType != "unsigned_int"
		&& outputPixelComponentType != "int"
		&& outputPixelComponentType != "unsigned_long"
		&& outputPixelComponentType != "long"
		&& outputPixelComponentType != "float"
		&& outputPixelComponentType != "double" )
	{
		/** In this case an illegal outputPixelComponentType is given. */
		std::cerr << "The given outputPixelComponentType is \"" << outputPixelComponentType
			<< "\", which is not supported." << std::endl;
		return 1;
	}

	/** TASK 2:
	 * Typedefs and test reading to determine correct image types.
	 * *******************************************************************
	 */

	/**	Initial image type. */
	const	unsigned int	Dimension	=	3;
	typedef	short				PixelType;

	/**	Some typedef's.	*/
	typedef	itk::Image<	PixelType, Dimension >			ImageType;
	typedef	itk::ImageFileReader<	ImageType	>				ReaderType;
	typedef	itk::ImageIOBase												ImageIOBaseType;
	//typedef itk::GDCMImageIO                        GDCMImageIOType;
	//typedef itk::GDCMSeriesFileNames								GDCMNamesGeneratorType;
	typedef itk::DICOMImageIO2											DICOMImageIOType;
	typedef itk::DICOMSeriesFileNames								DICOMNamesGeneratorType;
	typedef std::vector< std::string >							FileNamesContainerType;

	/** Create a testReader. */
	ReaderType::Pointer	testReader = ReaderType::New();
	
	/** Setup the testReader. */
	if ( !isDICOM )
	{
		/** Set the inputFileName in the testReader. */
		testReader->SetFileName( inputFileName.c_str() );
	}
	else
	{
		/** Get a name of a 2D image. *
		GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
		nameGenerator->SetInputDirectory( inputDirName.c_str() );
		FileNamesContainerType fileNames = nameGenerator->GetInputFileNames();
		std::string fileName = fileNames[ 0 ];

		/** Get a name of a 2D dicom image. */
		DICOMNamesGeneratorType::Pointer nameGenerator = DICOMNamesGeneratorType::New();
		nameGenerator->SetDirectory( inputDirectoryName.c_str() );
		FileNamesContainerType fileNames = nameGenerator->GetFileNames();
		std::string fileName = fileNames[ 0 ];

		/** Create a dicom ImageIO and set it in the testReader. */
		//GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();
		DICOMImageIOType::Pointer dicomIO = DICOMImageIOType::New();
		testReader->SetImageIO( dicomIO );

		/** Set the name of the 2D dicom image in the testReader. */
		testReader->SetFileName( fileName.c_str() );

	} // end isDICOM
	
	/** Generate all information. */
	testReader->GenerateOutputInformation();

	/** Extract the ImageIO from the testReader. */
	ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();

	/**	Get	the	component	type,	number of	components,	dimension	and	pixel	type.	*/
	unsigned int inputDimension = testImageIOBase->GetNumberOfDimensions();
	unsigned int numberOfComponents	=	testImageIOBase->GetNumberOfComponents();
	std::string	inputPixelComponentType	=	testImageIOBase->GetComponentTypeAsString(
		testImageIOBase->GetComponentType()	);
	std::string	pixelType	=	testImageIOBase->GetPixelTypeAsString(
		testImageIOBase->GetPixelType()	);
	
	/** TASK 3:
	 * Do some preparations.
	 * *******************************************************************
	 */

	/** Check outputPixelType. */
	if ( outputPixelComponentType == "" )
	{
		/** In this case this option is not given, and by default
		 * we set it to the inputPixelComponentType.
		 */
		outputPixelComponentType = inputPixelComponentType;
	}

	/** Get rid of the "_" in inputPixelComponentType and outputPixelComponentType. */
	std::basic_string<char>::size_type pos = inputPixelComponentType.find( "_" );
	static const std::basic_string<char>::size_type npos = -1;
	if ( pos != npos )
	{
		inputPixelComponentType.replace( pos, 1, " " );
	}
	pos = outputPixelComponentType.find( "_" );
	if ( pos != npos )
	{
		outputPixelComponentType.replace( pos, 1, " " );
	}

	/** TASK 4:
	 * Now we are ready to check on image type and subsequently call the
	 * correct ReadCastWrite-function.
	 * *******************************************************************
	 */

	if ( !isDICOM )
	{
		/**
		 * ****************** Support for SCALAR pixel types. **********************************
		 */
		if ( strcmp( pixelType.c_str(),	"scalar" ) == 0	&& numberOfComponents	== 1 )
		{
			/** Support for 2D images. */
			if ( inputDimension == 2 )
			{
				/** From unsigned char to something else. */
				callCorrectReadWriterMacro( unsigned char, unsigned char, 2 );
				callCorrectReadWriterMacro( unsigned char, char, 2 );
				callCorrectReadWriterMacro( unsigned char, unsigned short, 2 );
				callCorrectReadWriterMacro( unsigned char, short, 2 );
				callCorrectReadWriterMacro( unsigned char, unsigned int, 2 );
				callCorrectReadWriterMacro( unsigned char, int, 2 );
				callCorrectReadWriterMacro( unsigned char, unsigned long, 2 );
				callCorrectReadWriterMacro( unsigned char, long, 2 );
				callCorrectReadWriterMacro( unsigned char, float, 2 );
				callCorrectReadWriterMacro( unsigned char, double, 2 );

				/** From char to something else. */
				callCorrectReadWriterMacro( char, unsigned char, 2 );
				callCorrectReadWriterMacro( char, char, 2 );
				callCorrectReadWriterMacro( char, unsigned short, 2 );
				callCorrectReadWriterMacro( char, short, 2 );
				callCorrectReadWriterMacro( char, unsigned int, 2 );
				callCorrectReadWriterMacro( char, int, 2 );
				callCorrectReadWriterMacro( char, unsigned long, 2 );
				callCorrectReadWriterMacro( char, long, 2 );
				callCorrectReadWriterMacro( char, float, 2 );
				callCorrectReadWriterMacro( char, double, 2 );

				/** From unsigned short to something else. */
				callCorrectReadWriterMacro( unsigned short, unsigned char, 2 );
				callCorrectReadWriterMacro( unsigned short, char, 2 );
				callCorrectReadWriterMacro( unsigned short, unsigned short, 2 );
				callCorrectReadWriterMacro( unsigned short, short, 2 );
				callCorrectReadWriterMacro( unsigned short, unsigned int, 2 );
				callCorrectReadWriterMacro( unsigned short, int, 2 );
				callCorrectReadWriterMacro( unsigned short, unsigned long, 2 );
				callCorrectReadWriterMacro( unsigned short, long, 2 );
				callCorrectReadWriterMacro( unsigned short, float, 2 );
				callCorrectReadWriterMacro( unsigned short, double, 2 );

				/** From short to something else. */
				callCorrectReadWriterMacro( short, unsigned char, 2 );
				callCorrectReadWriterMacro( short, char, 2 );
				callCorrectReadWriterMacro( short, unsigned short, 2 );
				callCorrectReadWriterMacro( short, short, 2 );
				callCorrectReadWriterMacro( short, unsigned int, 2 );
				callCorrectReadWriterMacro( short, int, 2 );
				callCorrectReadWriterMacro( short, unsigned long, 2 );
				callCorrectReadWriterMacro( short, long, 2 );
				callCorrectReadWriterMacro( short, float, 2 );
				callCorrectReadWriterMacro( short, double, 2 );

				/** From unsigned int to something else. */
				callCorrectReadWriterMacro( unsigned int, unsigned char, 2 );
				callCorrectReadWriterMacro( unsigned int, char, 2 );
				callCorrectReadWriterMacro( unsigned int, unsigned short, 2 );
				callCorrectReadWriterMacro( unsigned int, short, 2 );
				callCorrectReadWriterMacro( unsigned int, unsigned int, 2 );
				callCorrectReadWriterMacro( unsigned int, int, 2 );
				callCorrectReadWriterMacro( unsigned int, unsigned long, 2 );
				callCorrectReadWriterMacro( unsigned int, long, 2 );
				callCorrectReadWriterMacro( unsigned int, float, 2 );
				callCorrectReadWriterMacro( unsigned int, double, 2 );

				/** From int to something else. */
				callCorrectReadWriterMacro( int, unsigned char, 2 );
				callCorrectReadWriterMacro( int, char, 2 );
				callCorrectReadWriterMacro( int, unsigned short, 2 );
				callCorrectReadWriterMacro( int, short, 2 );
				callCorrectReadWriterMacro( int, unsigned int, 2 );
				callCorrectReadWriterMacro( int, int, 2 );
				callCorrectReadWriterMacro( int, unsigned long, 2 );
				callCorrectReadWriterMacro( int, long, 2 );
				callCorrectReadWriterMacro( int, float, 2 );
				callCorrectReadWriterMacro( int, double, 2 );

				/** From unsigned long to something else. */
				callCorrectReadWriterMacro( unsigned long, unsigned char, 2 );
				callCorrectReadWriterMacro( unsigned long, char, 2 );
				callCorrectReadWriterMacro( unsigned long, unsigned short, 2 );
				callCorrectReadWriterMacro( unsigned long, short, 2 );
				callCorrectReadWriterMacro( unsigned long, unsigned int, 2 );
				callCorrectReadWriterMacro( unsigned long, int, 2 );
				callCorrectReadWriterMacro( unsigned long, unsigned long, 2 );
				callCorrectReadWriterMacro( unsigned long, long, 2 );
				callCorrectReadWriterMacro( unsigned long, float, 2 );
				callCorrectReadWriterMacro( unsigned long, double, 2 );

				/** From long to something else. */
				callCorrectReadWriterMacro( long, unsigned char, 2 );
				callCorrectReadWriterMacro( long, char, 2 );
				callCorrectReadWriterMacro( long, unsigned short, 2 );
				callCorrectReadWriterMacro( long, short, 2 );
				callCorrectReadWriterMacro( long, unsigned int, 2 );
				callCorrectReadWriterMacro( long, int, 2 );
				callCorrectReadWriterMacro( long, unsigned long, 2 );
				callCorrectReadWriterMacro( long, long, 2 );
				callCorrectReadWriterMacro( long, float, 2 );
				callCorrectReadWriterMacro( long, double, 2 );

				/** From float to something else. */
				callCorrectReadWriterMacro( float, unsigned char, 2 );
				callCorrectReadWriterMacro( float, char, 2 );
				callCorrectReadWriterMacro( float, unsigned short, 2 );
				callCorrectReadWriterMacro( float, short, 2 );
				callCorrectReadWriterMacro( float, unsigned int, 2 );
				callCorrectReadWriterMacro( float, int, 2 );
				callCorrectReadWriterMacro( float, unsigned long, 2 );
				callCorrectReadWriterMacro( float, long, 2 );
				callCorrectReadWriterMacro( float, float, 2 );
				callCorrectReadWriterMacro( float, double, 2 );

				/** From double to something else. */
				callCorrectReadWriterMacro( double, unsigned char, 2 );
				callCorrectReadWriterMacro( double, char, 2 );
				callCorrectReadWriterMacro( double, unsigned short, 2 );
				callCorrectReadWriterMacro( double, short, 2 );
				callCorrectReadWriterMacro( double, unsigned int, 2 );
				callCorrectReadWriterMacro( double, int, 2 );
				callCorrectReadWriterMacro( double, unsigned long, 2 );
				callCorrectReadWriterMacro( double, long, 2 );
				callCorrectReadWriterMacro( double, float, 2 );
				callCorrectReadWriterMacro( double, double, 2 );

			} // end support for 2D images
			/** Support for 3D images. */
			else if ( inputDimension == 3 )
			{
				/** From unsigned char to something else. */
				callCorrectReadWriterMacro( unsigned char, unsigned char, 3 );
				callCorrectReadWriterMacro( unsigned char, char, 3 );
				callCorrectReadWriterMacro( unsigned char, unsigned short, 3 );
				callCorrectReadWriterMacro( unsigned char, short, 3 );
				callCorrectReadWriterMacro( unsigned char, unsigned int, 3 );
				callCorrectReadWriterMacro( unsigned char, int, 3 );
				callCorrectReadWriterMacro( unsigned char, unsigned long, 3 );
				callCorrectReadWriterMacro( unsigned char, long, 3 );
				callCorrectReadWriterMacro( unsigned char, float, 3 );
				callCorrectReadWriterMacro( unsigned char, double, 3 );

				/** From char to something else. */
				callCorrectReadWriterMacro( char, unsigned char, 3 );
				callCorrectReadWriterMacro( char, char, 3 );
				callCorrectReadWriterMacro( char, unsigned short, 3 );
				callCorrectReadWriterMacro( char, short, 3 );
				callCorrectReadWriterMacro( char, unsigned int, 3 );
				callCorrectReadWriterMacro( char, int, 3 );
				callCorrectReadWriterMacro( char, unsigned long, 3 );
				callCorrectReadWriterMacro( char, long, 3 );
				callCorrectReadWriterMacro( char, float, 3 );
				callCorrectReadWriterMacro( char, double, 3 );

				/** From unsigned short to something else. */
				callCorrectReadWriterMacro( unsigned short, unsigned char, 3 );
				callCorrectReadWriterMacro( unsigned short, char, 3 );
				callCorrectReadWriterMacro( unsigned short, unsigned short, 3 );
				callCorrectReadWriterMacro( unsigned short, short, 3 );
				callCorrectReadWriterMacro( unsigned short, unsigned int, 3 );
				callCorrectReadWriterMacro( unsigned short, int, 3 );
				callCorrectReadWriterMacro( unsigned short, unsigned long, 3 );
				callCorrectReadWriterMacro( unsigned short, long, 3 );
				callCorrectReadWriterMacro( unsigned short, float, 3 );
				callCorrectReadWriterMacro( unsigned short, double, 3 );

				/** From short to something else. */
				callCorrectReadWriterMacro( short, unsigned char, 3 );
				callCorrectReadWriterMacro( short, char, 3 );
				callCorrectReadWriterMacro( short, unsigned short, 3 );
				callCorrectReadWriterMacro( short, short, 3 );
				callCorrectReadWriterMacro( short, unsigned int, 3 );
				callCorrectReadWriterMacro( short, int, 3 );
				callCorrectReadWriterMacro( short, unsigned long, 3 );
				callCorrectReadWriterMacro( short, long, 3 );
				callCorrectReadWriterMacro( short, float, 3 );
				callCorrectReadWriterMacro( short, double, 3 );

				/** From unsigned int to something else. */
				callCorrectReadWriterMacro( unsigned int, unsigned char, 3 );
				callCorrectReadWriterMacro( unsigned int, char, 3 );
				callCorrectReadWriterMacro( unsigned int, unsigned short, 3 );
				callCorrectReadWriterMacro( unsigned int, short, 3 );
				callCorrectReadWriterMacro( unsigned int, unsigned int, 3 );
				callCorrectReadWriterMacro( unsigned int, int, 3 );
				callCorrectReadWriterMacro( unsigned int, unsigned long, 3 );
				callCorrectReadWriterMacro( unsigned int, long, 3 );
				callCorrectReadWriterMacro( unsigned int, float, 3 );
				callCorrectReadWriterMacro( unsigned int, double, 3 );

				/** From int to something else. */
				callCorrectReadWriterMacro( int, unsigned char, 3 );
				callCorrectReadWriterMacro( int, char, 3 );
				callCorrectReadWriterMacro( int, unsigned short, 3 );
				callCorrectReadWriterMacro( int, short, 3 );
				callCorrectReadWriterMacro( int, unsigned int, 3 );
				callCorrectReadWriterMacro( int, int, 3 );
				callCorrectReadWriterMacro( int, unsigned long, 3 );
				callCorrectReadWriterMacro( int, long, 3 );
				callCorrectReadWriterMacro( int, float, 3 );
				callCorrectReadWriterMacro( int, double, 3 );

				/** From unsigned long to something else. */
				callCorrectReadWriterMacro( unsigned long, unsigned char, 3 );
				callCorrectReadWriterMacro( unsigned long, char, 3 );
				callCorrectReadWriterMacro( unsigned long, unsigned short, 3 );
				callCorrectReadWriterMacro( unsigned long, short, 3 );
				callCorrectReadWriterMacro( unsigned long, unsigned int, 3 );
				callCorrectReadWriterMacro( unsigned long, int, 3 );
				callCorrectReadWriterMacro( unsigned long, unsigned long, 3 );
				callCorrectReadWriterMacro( unsigned long, long, 3 );
				callCorrectReadWriterMacro( unsigned long, float, 3 );
				callCorrectReadWriterMacro( unsigned long, double, 3 );

				/** From long to something else. */
				callCorrectReadWriterMacro( long, unsigned char, 3 );
				callCorrectReadWriterMacro( long, char, 3 );
				callCorrectReadWriterMacro( long, unsigned short, 3 );
				callCorrectReadWriterMacro( long, short, 3 );
				callCorrectReadWriterMacro( long, unsigned int, 3 );
				callCorrectReadWriterMacro( long, int, 3 );
				callCorrectReadWriterMacro( long, unsigned long, 3 );
				callCorrectReadWriterMacro( long, long, 3 );
				callCorrectReadWriterMacro( long, float, 3 );
				callCorrectReadWriterMacro( long, double, 3 );

				/** From float to something else. */
				callCorrectReadWriterMacro( float, unsigned char, 3 );
				callCorrectReadWriterMacro( float, char, 3 );
				callCorrectReadWriterMacro( float, unsigned short, 3 );
				callCorrectReadWriterMacro( float, short, 3 );
				callCorrectReadWriterMacro( float, unsigned int, 3 );
				callCorrectReadWriterMacro( float, int, 3 );
				callCorrectReadWriterMacro( float, unsigned long, 3 );
				callCorrectReadWriterMacro( float, long, 3 );
				callCorrectReadWriterMacro( float, float, 3 );
				callCorrectReadWriterMacro( float, double, 3 );

				/** From double to something else. */
				callCorrectReadWriterMacro( double, unsigned char, 3 );
				callCorrectReadWriterMacro( double, char, 3 );
				callCorrectReadWriterMacro( double, unsigned short, 3 );
				callCorrectReadWriterMacro( double, short, 3 );
				callCorrectReadWriterMacro( double, unsigned int, 3 );
				callCorrectReadWriterMacro( double, int, 3 );
				callCorrectReadWriterMacro( double, unsigned long, 3 );
				callCorrectReadWriterMacro( double, long, 3 );
				callCorrectReadWriterMacro( double, float, 3 );
				callCorrectReadWriterMacro( double, double, 3 );

			} // end support for 3D images
			else
			{
				std::cerr	<< "Dimension equals " << inputDimension << ", which is not supported."	<< std::endl;
				std::cerr	<< "Only 2D and 3D images are supported."	<< std::endl;
				return 1;
			} // end if over inputDimension

		} // end support for SCALAR pixel type
		else
		{
			std::cerr	<< "Pixel type is "	<< pixelType
				<< ", component type is " << inputPixelComponentType
				<< " and number of components equals " <<	numberOfComponents <<	"."	<< std::endl;
			std::cerr	<< "ERROR: This image type is not supported."	<< std::endl;
			return 1;
		}
	}
	else
	{
		/** In this case input is a DICOM series, from which we only support
		 * SCALAR pixel types, with component type:
		 * DICOMImageIO2: (unsigned) char, (unsigned) short, float
		 * GDCMImageIO: (unsigned) char, (unsigned) short, (unsigned) int, double
		 * It is also assumed that the dicom series consist of multiple
		 * 2D images forming a 3D image.
		 */

		if ( strcmp( pixelType.c_str(),	"scalar" ) == 0	&& numberOfComponents	== 1 )
		{
			/** From unsigned char to something else. */
			callCorrectReadDicomWriterMacro( unsigned char, unsigned char );
			callCorrectReadDicomWriterMacro( unsigned char, char );
			callCorrectReadDicomWriterMacro( unsigned char, unsigned short );
			callCorrectReadDicomWriterMacro( unsigned char, short );
			callCorrectReadDicomWriterMacro( unsigned char, unsigned int );
			callCorrectReadDicomWriterMacro( unsigned char, int );
			callCorrectReadDicomWriterMacro( unsigned char, unsigned long );
			callCorrectReadDicomWriterMacro( unsigned char, long );
			callCorrectReadDicomWriterMacro( unsigned char, float );
			callCorrectReadDicomWriterMacro( unsigned char, double );

			/** From char to something else. */
			callCorrectReadDicomWriterMacro( char, unsigned char );
			callCorrectReadDicomWriterMacro( char, char );
			callCorrectReadDicomWriterMacro( char, unsigned short );
			callCorrectReadDicomWriterMacro( char, short );
			callCorrectReadDicomWriterMacro( char, unsigned int );
			callCorrectReadDicomWriterMacro( char, int );
			callCorrectReadDicomWriterMacro( char, unsigned long );
			callCorrectReadDicomWriterMacro( char, long );
			callCorrectReadDicomWriterMacro( char, float );
			callCorrectReadDicomWriterMacro( char, double );

			/** From unsigned short to something else. */
			callCorrectReadDicomWriterMacro( unsigned short, unsigned char );
			callCorrectReadDicomWriterMacro( unsigned short, char );
			callCorrectReadDicomWriterMacro( unsigned short, unsigned short );
			callCorrectReadDicomWriterMacro( unsigned short, short );
			callCorrectReadDicomWriterMacro( unsigned short, unsigned int );
			callCorrectReadDicomWriterMacro( unsigned short, int );
			callCorrectReadDicomWriterMacro( unsigned short, unsigned long );
			callCorrectReadDicomWriterMacro( unsigned short, long );
			callCorrectReadDicomWriterMacro( unsigned short, float );
			callCorrectReadDicomWriterMacro( unsigned short, double );

			/** From short to something else. */
			callCorrectReadDicomWriterMacro( short, unsigned char );
			callCorrectReadDicomWriterMacro( short, char );
			callCorrectReadDicomWriterMacro( short, unsigned short );
			callCorrectReadDicomWriterMacro( short, short );
			callCorrectReadDicomWriterMacro( short, unsigned int );
			callCorrectReadDicomWriterMacro( short, int );
			callCorrectReadDicomWriterMacro( short, unsigned long );
			callCorrectReadDicomWriterMacro( short, long );
			callCorrectReadDicomWriterMacro( short, float );
			callCorrectReadDicomWriterMacro( short, double );

			/** From unsigned int to something else. *
			callCorrectReadDicomWriterMacro( unsigned int, unsigned char );
			callCorrectReadDicomWriterMacro( unsigned int, char );
			callCorrectReadDicomWriterMacro( unsigned int, unsigned short );
			callCorrectReadDicomWriterMacro( unsigned int, short );
			callCorrectReadDicomWriterMacro( unsigned int, unsigned int );
			callCorrectReadDicomWriterMacro( unsigned int, int );
			callCorrectReadDicomWriterMacro( unsigned int, unsigned long );
			callCorrectReadDicomWriterMacro( unsigned int, long );
			callCorrectReadDicomWriterMacro( unsigned int, float );
			callCorrectReadDicomWriterMacro( unsigned int, double );

			/** From int to something else. *
			callCorrectReadDicomWriterMacro( int, unsigned char );
			callCorrectReadDicomWriterMacro( int, char );
			callCorrectReadDicomWriterMacro( int, unsigned short );
			callCorrectReadDicomWriterMacro( int, short );
			callCorrectReadDicomWriterMacro( int, unsigned int );
			callCorrectReadDicomWriterMacro( int, int );
			callCorrectReadDicomWriterMacro( int, unsigned long );
			callCorrectReadDicomWriterMacro( int, long );
			callCorrectReadDicomWriterMacro( int, float );
			callCorrectReadDicomWriterMacro( int, double );

			/** From float to something else. */
			callCorrectReadDicomWriterMacro( float, unsigned char );
			callCorrectReadDicomWriterMacro( float, char );
			callCorrectReadDicomWriterMacro( float, unsigned short );
			callCorrectReadDicomWriterMacro( float, short );
			callCorrectReadDicomWriterMacro( float, unsigned int );
			callCorrectReadDicomWriterMacro( float, int );
			callCorrectReadDicomWriterMacro( float, unsigned long );
			callCorrectReadDicomWriterMacro( float, long );
			callCorrectReadDicomWriterMacro( float, float );
			callCorrectReadDicomWriterMacro( float, double );

			/** From double to something else. *
			callCorrectReadDicomWriterMacro( double, unsigned char );
			callCorrectReadDicomWriterMacro( double, char );
			callCorrectReadDicomWriterMacro( double, unsigned short );
			callCorrectReadDicomWriterMacro( double, short );
			callCorrectReadDicomWriterMacro( double, unsigned int );
			callCorrectReadDicomWriterMacro( double, int );
			callCorrectReadDicomWriterMacro( double, unsigned long );
			callCorrectReadDicomWriterMacro( double, long );
			callCorrectReadDicomWriterMacro( double, float );
			callCorrectReadDicomWriterMacro( double, double );*/

		} // end support for SCALAR pixel type
		else
		{
			std::cerr	<< "Pixel type is "	<< pixelType
				<< ", component type is " << inputPixelComponentType
				<< " and number of components equals " <<	numberOfComponents <<	"."	<< std::endl;
			std::cerr	<< "ERROR: This image type is not supported."	<< std::endl;
			return 1;
		}

	} // end isDICOM


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
		} // end Support for RGB pixel type */


	/**	End	program. */
	return 0;

}	// end main


/**	The	function that	reads	the	input	dicom image	and	writes the output	image.
 * This	function is	templated	over the image types.	In the main	function
 * we	have to	make sure	to call	the	right	instantiation.
 */
template<	class	InputImageType,	class	OutputImageType	>
void ReadDicomSeriesCastWriteImage( std::string inputDirectoryName,	std::string	outputFileName )
{
	/**	Typedef the correct reader, caster and writer. */
	typedef	typename itk::ImageSeriesReader< InputImageType	>		SeriesReaderType;
	typedef typename itk::RescaleIntensityImageFilter<
		InputImageType, OutputImageType >													RescaleFilterType;
	typedef typename itk::ShiftScaleImageFilter<
		InputImageType, OutputImageType >													ShiftScaleFilterType;
	typedef	typename itk::ImageFileWriter< OutputImageType >		ImageWriterType;

	/** Typedef dicom stuff. */
	//typedef itk::GDCMImageIO                        GDCMImageIOType;
	//typedef itk::GDCMSeriesFileNames								GDCMNamesGeneratorType;
	typedef itk::DICOMImageIO2											DICOMImageIOType;
	typedef itk::DICOMSeriesFileNames								DICOMNamesGeneratorType;
	typedef std::vector< std::string >							FileNamesContainerType;

	/** Create the dicom ImageIO. */
	//typename GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();
	typename DICOMImageIOType::Pointer dicomIO = DICOMImageIOType::New();
	
	/** Get a list of the filenames of the 2D input dicom images. */
	//GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
  //nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );
	//FileNamesContainerType fileNames = nameGenerator->GetInputFileNames();
	DICOMNamesGeneratorType::Pointer nameGenerator = DICOMNamesGeneratorType::New();
	nameGenerator->SetDirectory( inputDirectoryName.c_str() );
	FileNamesContainerType fileNames = nameGenerator->GetFileNames();

	/** Create and setup the seriesReader. */
	typename SeriesReaderType::Pointer seriesReader = SeriesReaderType::New();
	seriesReader->SetFileNames( fileNames );
	seriesReader->SetImageIO( dicomIO );

	/** Create and setup caster and writer. */
	//typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
	typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
	typename ImageWriterType::Pointer	writer = ImageWriterType::New();
	caster->SetShift( 0.0 );
	caster->SetScale( 1.0 );
	writer->SetFileName( outputFileName.c_str()	);

	/** Connect the pipeline. */
	caster->SetInput(	seriesReader->GetOutput()	);
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
	PrintInfo( seriesReader, writer	);

}	// end ReadDicomSeriesCastWriteImage


/**	The	function that	reads	the	input	image	and	writes the output	image.
 * This	function is	templated	over the image types.	In the main	function
 * we	have to	make sure	to call	the	right	instantiation.
 */
template<	class	InputImageType,	class	OutputImageType	>
void ReadCastWriteImage( std::string inputFileName,	std::string	outputFileName )
{
	/**	Typedef the correct reader, caster and writer. */
	typedef	typename itk::ImageFileReader< InputImageType	>			ImageReaderType;
	typedef typename itk::RescaleIntensityImageFilter<
		InputImageType, OutputImageType >													RescaleFilterType;
	typedef typename itk::ShiftScaleImageFilter<
		InputImageType, OutputImageType >													ShiftScaleFilterType;
	typedef	typename itk::ImageFileWriter< OutputImageType >		ImageWriterType;
	
	/** Create and setup the reader. */
	typename ImageReaderType::Pointer	reader = ImageReaderType::New();
	reader->SetFileName( inputFileName.c_str() );

	/** Create and setup caster and writer. */
	//typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
	typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
	typename ImageWriterType::Pointer	writer = ImageWriterType::New();
	caster->SetShift( 0.0 );
	caster->SetScale( 1.0 );
	writer->SetFileName( outputFileName.c_str()	);

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


/** Print image information from the reader and the writer. */
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

