

/** Include the right headers. */

#include "itkImageFileReader.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageFileWriter.h"

#include "itkImage.h"
#include "itkVector.h"

/** \todo explain rationale.
 *
 *
 */
int main( int argc, char ** argv )
{
	/** Warning. */
	//std::cout << "Warning: this program assumes 3d images with a pixelType convertible to shorts!\n" << std::endl;

	if ( argc != 4 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "pxextractindexfromvectorimage inputimage outputimage index" << std::endl;
		std::cout << "WARNING: this program expects 3D float vector images." << std::endl;
		return 1;
	}

	/** Define image type. */
	const unsigned int Dimension = 3;
	typedef float PixelValueType;

	/** Some typedef's. */
	typedef itk::Vector< PixelValueType, Dimension >		VectorPixelType;
	typedef itk::Image< VectorPixelType, Dimension >		VectorImageType;
	typedef itk::Image< PixelValueType, Dimension >			ImageType;
	typedef itk::ImageFileReader< VectorImageType >			ImageReaderType;
	typedef itk::VectorIndexSelectionCastImageFilter<
		VectorImageType, ImageType >											IndexExtractorType;
	typedef itk::ImageFileWriter< ImageType >						ImageWriterType;

	/**	Task 1:
	 * **********************************************************************
	 * Read in the input image.
	 */

	/** Create reader. */
	ImageReaderType::Pointer reader = ImageReaderType::New();
	reader->SetFileName( argv[ 1 ] );

	/** Update the reader. */
	try
	{
		reader->Update();
	}
	catch ( itk::ExceptionObject & err )
	{
		std::cerr << "ERROR occured while reading the image." << std::endl;
		std::cerr << err << std::endl;
		return 1;
	}

	/** Create index extractor and writer. */
	IndexExtractorType::Pointer extractor = IndexExtractorType::New();
	extractor->SetInput( reader->GetOutput() );
	extractor->SetIndex( atoi( argv[ 3 ] ) );
	ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( argv[ 2 ] );
	writer->SetInput( extractor->GetOutput() );

	/** Sanity check. */
	if ( atoi( argv[ 3 ] ) > Dimension - 1 )
	{
		std::cerr << "ERROR: You selected index "
			<< argv[ 3 ] << ", where the input image only has "
			<< Dimension << " indices." << std::endl;
		return 1;
	}

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


