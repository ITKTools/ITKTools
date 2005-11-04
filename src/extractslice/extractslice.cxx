

/** Include the right headers. */

#include "itkImageFileReader.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"

#include "itkImage.h"

/** \todo explain rationale.
 *
 *
 */
int main( int argc, char ** argv )
{
	/** Warning. */
	//std::cout << "Warning: this program assumes 3d images with a pixelType convertible to shorts!\n" << std::endl;

	if ( argc == 1 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "pxextractslice inputimage outputimage slicenumber" << std::endl;
		return 1;
	}

	/** Define image type. */
	typedef short PixelType;

	/** Some typedef's. */
	typedef itk::Image<PixelType, 3>										Image3DType;
	typedef Image3DType::Pointer												Image3DPointer;
	typedef itk::Image<PixelType, 2>										Image2DType;
	typedef Image2DType::Pointer												Image2DPointer;
	typedef itk::ImageFileReader<Image3DType>						ImageReaderType;
	typedef ImageReaderType::Pointer										ImageReaderPointer;
	typedef itk::ExtractImageFilter<
		Image3DType, Image2DType >												ExtractFilterType;
	typedef ExtractFilterType::Pointer									ExtractFilterPointer;
	typedef itk::ImageFileWriter<Image2DType>						ImageWriterType;
	typedef ImageWriterType::Pointer										ImageWriterPointer;

	typedef Image3DType::RegionType				RegionType;
	typedef Image3DType::SizeType					SizeType;
	typedef Image3DType::IndexType				IndexType;

	/**	Task 1:
	 * **********************************************************************
	 * Read in the input image.
	 */

	/** Create reader. */
	ImageReaderPointer reader = ImageReaderType::New();
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

	/** Create extractor and writer. */
	ExtractFilterPointer extractor = ExtractFilterType::New();
	extractor->SetInput( reader->GetOutput() );
	ImageWriterPointer writer = ImageWriterType::New();
	writer->SetFileName( argv[ 2 ] );
	writer->SetInput( extractor->GetOutput() );

	/** Get the slicenumber. */
	const unsigned int slicenumber = atoi( argv[ 3 ] );

	/** Get the size. */
  RegionType inputRegion = reader->GetOutput()->GetLargestPossibleRegion();
	SizeType size = inputRegion.GetSize();

	/** Sanity check. */
	if ( slicenumber > size[ 2 ] )
	{
		std::cerr << "ERROR: You selected slice number "
			<< slicenumber << ", where the input image only has "
			<< size[ 2 ] << " number of slices." << std::endl;
		return 1;
	}
	/** Set the third dimension to zero. */
  size[ 2 ] = 0;

	/** Get the index. Set the third dimension to the correct slice. */
	IndexType start = inputRegion.GetIndex();
  start[ 2 ] = slicenumber;

	/** Create a desired extraction region and set it into the extractor. */
	RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );

	extractor->SetExtractionRegion( desiredRegion );

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


