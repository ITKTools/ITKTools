#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

/** This program tiles a stacks of 2D images into a 3D image.
 * This is done by employing an itk::SeriesFileReader.
 *
 */

int main( int argc, char ** argv )
{
	/** Warning. */
	//std::cout << "Warning: this program assumes images with a pixelType convertible to shorts!\n" << std::endl;

	if ( argc < 4 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "pxtileimages 2Dinputimagename1 ... 2DinputimagenameN outputimagename [-sp spacing in z-direction]" << std::endl;
		return 1;
	}

	/** Define image type. */
	typedef short PixelType;

	/** Some typedef's. */
	typedef itk::Image<PixelType, 3>										Image3DType;
	typedef Image3DType::SpacingType										SpacingType;
	typedef itk::ImageSeriesReader<Image3DType>					ImageSeriesReaderType;
	typedef ImageSeriesReaderType::FileNamesContainer		FileNamesContainer;
	typedef itk::ImageFileWriter<Image3DType>						ImageWriterType;
	
	/**	Check existence of -sp. */
	unsigned int pos_sp = 0;
	for ( unsigned int i = 1; i < argc - 1; i++ )
	{
		std::string arg = argv[ i ];
		if ( arg == "-sp" ) pos_sp = i;
	}
	/**	Get the arguments. */
	FileNamesContainer filenames;
	std::string outputfilename;
	float spacing = 1.0;
	unsigned int pos = argc - 1;
	if ( pos_sp != 0 )
	{
		pos = pos_sp - 1;
		spacing = atof( argv[ pos_sp + 1 ] );
	}
	for ( unsigned int i = 1; i < pos; i++ )
	{
		filenames.push_back( argv[ i ] );
	}
	outputfilename = argv[ pos ];

	/** Create reader. */
	ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
	reader->SetFileNames( filenames );

	/** Update the reader. */
	try
	{
		reader->Update();
	}
	catch ( itk::ExceptionObject & err )
	{
		std::cerr << "ERROR occured while reading the images." << std::endl;
		std::cerr << err << std::endl;
		return 1;
	}

	/** Get and set the spacing. */
	SpacingType space = reader->GetOutput()->GetSpacing();
	space[ 2 ] = spacing;
	reader->GetOutput()->SetSpacing( space );

	/** Create writer. */
	ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( outputfilename.c_str() );
	writer->SetInput( reader->GetOutput() );

	/** Update. */
	try
	{
		writer->Update();
	}
	catch ( itk::ExceptionObject & err )
	{
		std::cerr << "ERROR occured." << std::endl;
		std::cerr << err << std::endl;
		return 1;
	}

	/** Return a value. */
	return 0;

} // end main


