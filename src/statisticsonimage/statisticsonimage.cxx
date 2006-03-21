#include "itkImageFileReader.h"
#include "itkStatisticsImageFilter.h"

/** This program determines the minimum, maximum and
 * mean of an image.
 */

int main( int argc, char ** argv )
{
	/** Warning. */
	//std::cout << "Warning: this program assumes images with a pixelType convertible to shorts!\n" << std::endl;

	if ( argc > 2 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "pxtileimages 2Dinputimage" << std::endl;
		return 1;
	}

	/** Define image type. */
	const unsigned int Dimension = 2;
	typedef short PixelType;

	/** Some typedef's. */
	typedef itk::Image<PixelType, Dimension>						ImageType;
	typedef itk::ImageFileReader< ImageType >						ImageReaderType;
	typedef itk::StatisticsImageFilter< ImageType >			StatisticsFilterType;
	typedef StatisticsFilterType::RealType			RealType;
	
	/** Create reader. */
	ImageReaderType::Pointer reader = ImageReaderType::New();
	reader->SetFileName( argv[ 1 ] );

	/** Create statistics filter. */
	StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
	statistics->SetInput( reader->GetOutput() );

	/** Update. */
	try
	{
		statistics->Update();
	}
	catch ( itk::ExceptionObject & err )
	{
		std::cerr << "ERROR occured." << std::endl;
		std::cerr << err << std::endl;
		return 1;
	}

	/** Get all the output stuff. */
	PixelType min = statistics->GetMinimum();
	RealType mean = statistics->GetMean();
	PixelType max = statistics->GetMaximum();
	RealType std = statistics->GetSigma();
	RealType sum = statistics->GetSum();

	/** Print to screen. */
	std::cout << "min:\t" << min << std::endl;
	std::cout << "mean:\t" << mean << std::endl;
	std::cout << "max:\t" << max << std::endl;
	std::cout << "std:\t" << std << std::endl;
	std::cout << "sum:\t" << sum << std::endl;

	/** Return a value. */
	return 0;

} // end main


