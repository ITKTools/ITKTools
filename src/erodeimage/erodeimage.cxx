
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleErodeImageFilter.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	// Some consts.
	const unsigned int	Dimension = 2;
	typedef short	PixelType;

	std::cout << "NOTE: This program only erodes 2D short images!" << std::endl;
	/** Check number of arguments. */
	if ( argc != 4 )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "\tErodeImage inputfilename outputfilename radius" << std::endl;
		return 1;
	}
	unsigned int radius = atoi( argv[ 3 ] );

	/** TYPEDEF's. */
	typedef itk::Image< PixelType, Dimension >					ImageType;
	typedef itk::ImageFileReader< ImageType >						ReaderType;
	typedef itk::ImageFileWriter< ImageType >						WriterType;
	typedef itk::BinaryBallStructuringElement<
		PixelType, Dimension >														StructuringElementType;
	typedef StructuringElementType::RadiusType					RadiusType;
	typedef itk::GrayscaleErodeImageFilter<
		ImageType, ImageType, StructuringElementType >		ErodeFilterType;

	/** DECLARATION'S. */
	ReaderType::Pointer reader = ReaderType::New();
	WriterType::Pointer writer = WriterType::New();
	ErodeFilterType::Pointer erosion = ErodeFilterType::New();
	RadiusType	radiusarray;
	StructuringElementType	S_ball;

	/** Setup the reader. */
	reader->SetFileName( argv[ 1 ] );

	/** Create and fill the radius. */
	radiusarray.Fill( 0 );
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		radiusarray.SetElement( i, radius );
	}

	/** Create the structuring element and set it into the dilation filter. */
	S_ball.SetRadius( radiusarray );
	S_ball.CreateStructuringElement();
	erosion->SetKernel( S_ball );

	/** Connect the pipeline. */
	erosion->SetInput( reader->GetOutput() );

	/** Setup the writer and connect the pipeline. */
	writer->SetFileName( argv[ 2 ] );
	writer->SetInput( erosion->GetOutput() );

	/** Write the output image and execute the pipeline. */
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject & excp )
	{
		std::cerr << "ERROR: caught ITK exception while executing the pipeline." << std::endl;
		std::cerr << excp << std::endl;
		return 1;
	}

	/** End program. */
	return 0;

} // end main

