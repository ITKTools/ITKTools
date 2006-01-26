/**
 * computeDifferenceImage
 *
 * This program computes the difference between images.
 *
 * 
 * authors:				Marius Staring
 *
 */

#include <iostream>
#include "itkImageFileReader.h"

/** In order to determine if argv[1] is a directory or a file,
 * so that we can distinguish between dicom and other files.
 */
#include <itksys/SystemTools.hxx>

extern int ComputeScalarDifferenceImage( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension);

extern int ComputeVectorDifferenceImage( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension, int vectorDimension);

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
		std::cout  << "\tpxcomputedifferenceimage inputimage1filename inputimage2filename [outputimagefilename] [outputPixelComponentType]" << std::endl;
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
		std::cout  << "\tBy default the outputPixelComponentType is set to the inputPixelComponentType of image1." << std::endl;
		return 1;
	}

	/** Print arguments. */
	std::cout << "pxcomputedifferenceimage ";
	for ( unsigned int i = 1; i < argc; i++ )
	{
		std::cout << argv[ i ] << " ";
	}
	std::cout << std::endl;

	/** Get the image names. */
	std::string image1FileName = argv[ 1 ];
	std::string image2FileName = argv[ 2 ];
	std::string outputFileName = "";
	std::string outputPixelComponentType = "";
	if ( argc == 3 )
	{
		std::string::size_type slash = image2FileName.find_last_of( "/" ) + 1;
    outputFileName = image1FileName.substr( 0, image1FileName.rfind( "." ) );
		outputFileName += "MINUS";
		outputFileName += image2FileName.substr( slash, image2FileName.rfind( "." ) - slash );
		outputFileName += ".mhd";
	}
	else if ( argc == 4 )
	{
		outputFileName = argv[ 3 ];
	}
	else if ( argc == 5 )
	{
		outputFileName = argv[ 3 ];
		outputPixelComponentType = argv[ 4 ];
	}

	/** Check if image1FileName and image2FileName exist. */
	bool exists1 = itksys::SystemTools::FileExists( image1FileName.c_str() );
	bool exists2 = itksys::SystemTools::FileExists( image2FileName.c_str() );

	if ( !exists1 || !exists2 )
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

	/** Create testReaders. */
	ReaderType::Pointer testReader1 = ReaderType::New();
	ReaderType::Pointer testReader2 = ReaderType::New();

	/** Setup the testReaders. */
	testReader1->SetFileName( image1FileName.c_str() );
	testReader2->SetFileName( image2FileName.c_str() );
	
	/** Generate all information. */
	testReader1->GenerateOutputInformation();
	testReader2->GenerateOutputInformation();

	/** Extract the ImageIO from the testReaders. */
	ImageIOBaseType::Pointer testImageIOBase1 = testReader1->GetImageIO();
	ImageIOBaseType::Pointer testImageIOBase2 = testReader2->GetImageIO();

	/** Get the component type, number of components, dimension and pixel type of image1. */
	unsigned int inputDimension1 = testImageIOBase1->GetNumberOfDimensions();
	unsigned int numberOfComponents1 = testImageIOBase1->GetNumberOfComponents();
	std::string inputPixelComponentType1 = testImageIOBase1->GetComponentTypeAsString(
		testImageIOBase1->GetComponentType() );
	std::string pixelType1 = testImageIOBase1->GetPixelTypeAsString(
		testImageIOBase1->GetPixelType() );

	/** Get the component type, number of components, dimension and pixel type of image2. */
	unsigned int inputDimension2 = testImageIOBase2->GetNumberOfDimensions();
	unsigned int numberOfComponents2 = testImageIOBase2->GetNumberOfComponents();
	std::string inputPixelComponentType2 = testImageIOBase2->GetComponentTypeAsString(
		testImageIOBase2->GetComponentType() );
	std::string pixelType2 = testImageIOBase2->GetPixelTypeAsString(
		testImageIOBase2->GetPixelType() );

	/** TASK 3:
	 * Do some preparations and checks.
	 * *******************************************************************
	 */

	/** Check dimension equality. */
	if ( inputDimension1 != inputDimension2 )
	{
		std::cerr << "The dimensions of the input images are "
			<< inputDimension1 << " and "
			<< inputDimension2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

	/** Check equality of the pixel type. */
	if ( pixelType1 != pixelType2 )
	{
		std::cerr << "The pixel type of the input images are "
			<< pixelType1 << " and "
			<< pixelType2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

	/** Check equality of the number of components. */
	if ( numberOfComponents1 != numberOfComponents2 )
	{
		std::cerr << "The number of components of the input images are "
			<< numberOfComponents1 << " and "
			<< numberOfComponents2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

	/** Check inputPixelComponentType1. */
	if ( inputPixelComponentType1 != "unsigned_char"
		&& inputPixelComponentType1 != "char"
		&& inputPixelComponentType1 != "unsigned_short"
		&& inputPixelComponentType1 != "short"
		&& inputPixelComponentType1 != "unsigned_int"
		&& inputPixelComponentType1 != "int"
		&& inputPixelComponentType1 != "unsigned_long"
		&& inputPixelComponentType1 != "long"
		&& inputPixelComponentType1 != "float"
		&& inputPixelComponentType1 != "double" )
	{
		/** In this case an illegal inputPixelComponentType is found. */
		std::cerr << "The found inputPixelComponentType of image1 is \"" << inputPixelComponentType1
			<< "\", which is not supported." << std::endl;
		return 1;
	}

	/** Check inputPixelType2. */
	if ( inputPixelComponentType2 != "unsigned_char"
		&& inputPixelComponentType2 != "char"
		&& inputPixelComponentType2 != "unsigned_short"
		&& inputPixelComponentType2 != "short"
		&& inputPixelComponentType2 != "unsigned_int"
		&& inputPixelComponentType2 != "int"
		&& inputPixelComponentType2 != "unsigned_long"
		&& inputPixelComponentType2 != "long"
		&& inputPixelComponentType2 != "float"
		&& inputPixelComponentType2 != "double" )
	{
		/** In this case an illegal inputPixelComponentType is found. */
		std::cerr << "The found inputPixelComponentType of image2 is \"" << inputPixelComponentType2
			<< "\", which is not supported." << std::endl;
		return 1;
	}

	/** Check outputPixelType. */
	if ( outputPixelComponentType == "" )
	{
		/** In this case this option is not given, and by default
		* we set it to the inputPixelComponentType.
		*/
		outputPixelComponentType = inputPixelComponentType1;
	}

	/** Get rid of the "_" in inputPixelComponentTypes and outputPixelComponentType. */
	std::basic_string<char>::size_type pos = inputPixelComponentType1.find( "_" );
	static const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
	if ( pos != npos )
	{
		inputPixelComponentType1.replace( pos, 1, " " );
	}
	pos = inputPixelComponentType2.find( "_" );
	if ( pos != npos )
	{
		inputPixelComponentType2.replace( pos, 1, " " );
	}
	pos = outputPixelComponentType.find( "_" );
	if ( pos != npos )
	{
		outputPixelComponentType.replace( pos, 1, " " );
	}

	/** TASK 4:
	 * Now we are ready to check on image type and subsequently call the
	 * correct ComputeDifference-function.
	 * *******************************************************************
	 */

	try
	{
		/**
		 * ****************** Support for SCALAR pixel types. **********************************
		 */
		if ( strcmp( pixelType1.c_str(), "scalar" ) == 0 && numberOfComponents1 == 1 )
		{
			const int ret_value = ComputeScalarDifferenceImage(
				inputPixelComponentType1, inputPixelComponentType2,
				outputPixelComponentType, image1FileName, image2FileName,
				outputFileName, inputDimension1 );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		} // end scalar support
		/**
		 * ****************** Support for VECTOR pixel types. **********************************
		 */
		//else if ( strcmp( pixelType1.c_str(), "vector" ) == 0 )
		else if ( numberOfComponents1 > 1 )
		{
			const int ret_value = ComputeVectorDifferenceImage(
				inputPixelComponentType1, inputPixelComponentType2,
				outputPixelComponentType, image1FileName, image2FileName,
				outputFileName, inputDimension1, numberOfComponents1 );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		} // end vector support
		else
		{
			std::cerr << "Pixel types are " << pixelType1
				<< ", component types are " << inputPixelComponentType1
				<< " and number of components equals " << numberOfComponents1 << "." << std::endl;
			std::cerr << "ERROR: This image type is not supported." << std::endl;
			return 1;
		}
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
