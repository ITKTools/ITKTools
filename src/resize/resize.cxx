
#include "itkImage.h"
#include "itkResampleImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	// Some consts.
	const unsigned int	Dimension = 3;
	typedef short	PixelType;

	std::cout << "NOTE: This program only resizes 3D short images!" << std::endl;
	/** Check number of arguments. */
	if ( argc != 4 )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "\tResizeImage inputfilename outputfilename factor" << std::endl;
		return 1;
	}
	float factor = atof( argv[ 3 ] );

	/** TYPEDEF's. */
	typedef itk::Image< PixelType, Dimension >					ImageType;
	typedef itk::ResampleImageFilter<
		ImageType, ImageType >														ResamplerType;
	typedef itk::ImageFileReader< ImageType >						ReaderType;
	typedef itk::ImageFileWriter< ImageType >						WriterType;

	typedef ImageType::SizeType				SizeType;
	typedef ImageType::SpacingType		SpacingType;

	/** DECLARATION'S. */
	ImageType::Pointer inputImage = ImageType::New();
	ResamplerType::Pointer resampler = ResamplerType::New();
	ReaderType::Pointer reader = ReaderType::New();
	WriterType::Pointer writer = WriterType::New();

	/** Read in the inputImage. */
	reader->SetFileName( argv[ 1 ] );
	inputImage = reader->GetOutput();
	inputImage->Update();

	/** Prepare stuff. */
	SpacingType outputSpacing = inputImage->GetSpacing();
	SizeType outputSize = inputImage->GetLargestPossibleRegion().GetSize();
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		outputSpacing[ i ] = outputSpacing[ i ] * factor;
		outputSize[ i ] = static_cast<unsigned int>( outputSize[ i ] / factor );
	}

	/** Setup the pipeline.
	 * The resampler has by default an IdentityTransform as transform, and
	 * a LinearInterpolateImageFunction as interpolator.
	 */
	resampler->SetInput( inputImage );
	resampler->SetSize( outputSize );
	resampler->SetDefaultPixelValue( 0 );
	resampler->SetOutputStartIndex( inputImage->GetLargestPossibleRegion().GetIndex() );
	resampler->SetOutputSpacing( outputSpacing );
	resampler->SetOutputOrigin( inputImage->GetOrigin() );

	/** Write the output image. */
	writer->SetFileName( argv[ 2 ] );
	writer->SetInput( resampler->GetOutput() );

	/** Execute the pipeline. */
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject	&	err	)
	{
		std::cerr	<< "ExceptionObject	caught !"	<< std::endl;
		std::cerr	<< err <<	std::endl;
	}

	/** End program. */
	return 0;

} // end main

