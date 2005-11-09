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
 * authors:				Marius Staring and Stefan Klein
 *
 * Thanks to Hans J. Johnson for a modification to this program. This
 * modification breaks down the program into smaller compilation units,
 * so that the compiler does not overflow.
 *
 */

#include <iostream>
#include "itkImageFileReader.h"

/** DICOM headers. */
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

/** In order to determine if argv[1] is a directory or a file,
 * so that we can distinguish between dicom and other files.
 */
#include <itksys/SystemTools.hxx>

extern int      FileConverterScalar(const std::string &inputPixelComponentType,const std::string &outputPixelComponentType,const std::string &inputFileName, const std::string &outputFileName, int inputDimension);
extern int DicomFileConverterScalar(const std::string &inputPixelComponentType,const std::string &outputPixelComponentType,const std::string &inputFileName, const std::string &outputFileName, int inputDimension);

//-------------------------------------------------------------------------------------

int  main(  int  argc,  char *argv[] )
{
	/** TASK 1:
	 * Check arguments.
	 * *******************************************************************
	 */
	if ( argc  < 3 || argv[ 1 ] == "--help" )
	{
		std::cout  << "Usage:"  << std::endl;
		std::cout  << "\tpxcastconvert inputfilename outputfilename [outputPixelComponentType]" << std::endl;
		std::cout  << "\tpxcastconvert dicomDirectory outputfilename [outputPixelComponentType]" << std::endl;
		std::cout  << "\twhere outputPixelComponentType is one of:" << std::endl;
		std::cout  << "\t\t- unsigned_char" << std::endl;
		std::cout  << "\t\t- char" << std::endl;
		std::cout  << "\t\t- unsigned_short" << std::endl;
		std::cout  << "\t\t- short" << std::endl;
		std::cout  << "\t\t- unsigned_int" << std::endl;
		std::cout  << "\t\t- int" << std::endl;
		std::cout  << "\t\t- unsigned_long" << std::endl;
		std::cout  << "\t\t- long" << std::endl;
		std::cout  << "\t\t- float" << std::endl;
		std::cout  << "\t\t- double" << std::endl;
		std::cout  << "\tprovided that the outputPixelComponentType is supported by the output file format." << std::endl;
		std::cout  << "\tBy default the outputPixelComponentType is set to the inputPixelComponentType." << std::endl;
		return 1;
	}

	/**  Get  the  inputs. */
	std::string  input = argv[ 1 ];
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

	/** Initial image type. */
	const unsigned int		Dimension  =  3;
	typedef short					PixelType;

	/** Some typedef's. */
	typedef itk::Image< PixelType, Dimension >			ImageType;
	typedef itk::ImageFileReader< ImageType >				ReaderType;
	typedef itk::ImageIOBase												ImageIOBaseType;
	typedef itk::GDCMImageIO												GDCMImageIOType;
	typedef itk::GDCMSeriesFileNames								GDCMNamesGeneratorType;
	typedef std::vector< std::string >							FileNamesContainerType;

	/** Create a testReader. */
	ReaderType::Pointer testReader = ReaderType::New();

	/** Setup the testReader. */
	if ( !isDICOM )
	{
		/** Set the inputFileName in the testReader. */
		testReader->SetFileName( inputFileName.c_str() );
	}
	else
	{
		/** Get a name of a 2D image. */
		GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
		nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );
		FileNamesContainerType fileNames = nameGenerator->GetInputFileNames();
		std::string fileName = fileNames[ 0 ];

		/** Create a dicom ImageIO and set it in the testReader. */
		GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();
		testReader->SetImageIO( dicomIO );

		/** Set the name of the 2D dicom image in the testReader. */
		testReader->SetFileName( fileName.c_str() );

	} // end isDICOM

	/** Generate all information. */
	testReader->GenerateOutputInformation();

	/** Extract the ImageIO from the testReader. */
	ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();

	/** Get the component type, number of components, dimension and pixel type. */
	unsigned int inputDimension = testImageIOBase->GetNumberOfDimensions();
	unsigned int numberOfComponents = testImageIOBase->GetNumberOfComponents();
	std::string inputPixelComponentType = testImageIOBase->GetComponentTypeAsString(
		testImageIOBase->GetComponentType() );
	std::string pixelType = testImageIOBase->GetPixelTypeAsString(
		testImageIOBase->GetPixelType() );

	/** TASK 3:
	 * Do some preparations.
	 * *******************************************************************
	 */

	/** Check inputPixelType. */
	if ( inputPixelComponentType != "unsigned_char"
		&& inputPixelComponentType != "char"
		&& inputPixelComponentType != "unsigned_short"
		&& inputPixelComponentType != "short"
		&& inputPixelComponentType != "unsigned_int"
		&& inputPixelComponentType != "int"
		&& inputPixelComponentType != "unsigned_long"
		&& inputPixelComponentType != "long"
		&& inputPixelComponentType != "float"
		&& inputPixelComponentType != "double" )
	{
		/** In this case an illegal inputPixelComponentType is found. */
		std::cerr << "The found inputPixelComponentType is \"" << inputPixelComponentType
			<< "\", which is not supported." << std::endl;
		return 1;
	}

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
	static const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
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

	try
	{
		if ( !isDICOM )
		{
			/**
			* ****************** Support for SCALAR pixel types. **********************************
			*/
			if ( strcmp( pixelType.c_str(), "scalar" ) == 0 && numberOfComponents == 1 )
			{
				const int ret_value = FileConverterScalar(
					inputPixelComponentType, outputPixelComponentType, inputFileName,
					outputFileName, inputDimension );
				if ( ret_value != 0 )
				{
					return ret_value;
				}
			}
			else
			{
				std::cerr << "Pixel type is " << pixelType
					<< ", component type is " << inputPixelComponentType
					<< " and number of components equals " << numberOfComponents << "." << std::endl;
				std::cerr << "ERROR: This image type is not supported." << std::endl;
				return 1;
			}
		} // end NonDicom image
		else
		{
			/** In this case input is a DICOM series, from which we only support
			 * SCALAR pixel types, with component type:
			 * DICOMImageIO2: (unsigned) char, (unsigned) short, float
			 * GDCMImageIO: (unsigned) char, (unsigned) short, (unsigned) int, double
			 * It is also assumed that the dicom series consist of multiple
			 * 2D images forming a 3D image.
			 */

			if ( strcmp( pixelType.c_str(), "scalar" ) == 0 && numberOfComponents == 1 )
			{
				const int ret_value = DicomFileConverterScalar(
					inputPixelComponentType, outputPixelComponentType,
					inputDirectoryName, outputFileName, inputDimension );
				if ( ret_value != 0 )
				{
					return ret_value;
				}
			}
			else
			{
				std::cerr << "Pixel type is " << pixelType
					<< ", component type is " << inputPixelComponentType
					<< " and number of components equals " << numberOfComponents << "." << std::endl;
				std::cerr << "ERROR: This image type is not supported." << std::endl;
				return 1;
			}

		} // end isDICOM


		/**
		* ****************** Support for RGB pixel types. **********************************
		*
		else if ( strcmp( pixelType.c_str(),  "rgb" ) == 0  && numberOfComponents  == 3 )
		{
		if ( strcmp( iOComponent.c_str(),  "unsigned_char"  )  == 0 )
		{
		typedef itk::RGBPixel< unsigned char >    PixelType;
		typedef  itk::Image<  PixelType, 2 >        ImageType;
		ReadWriteImage<  ImageType, ImageType >(  inputFileName, outputFileName  );
		}
		} // end Support for RGB pixel type */

	} // end try
	/** If any errors have occurred, catch and print the exception and return false. */
	catch( itk::ExceptionObject  &  err  )
	{
		std::cerr  << "ExceptionObject caught !"  << std::endl;
		std::cerr  << err <<  std::endl;
		return 1;
	}

	/** End  program. Return succes. */
	return 0;

}  // end main
