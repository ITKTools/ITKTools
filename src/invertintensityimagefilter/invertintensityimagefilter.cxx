#include "itkImageFileReader.h"
#include "itkStatisticsImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkImageFileWriter.h"

/** This program inverts the intensity of an image:
 * new = max - old,
 * where max is the maximum of an image.
 */

int main( int argc, char ** argv )
{
	/** Check arguments. */
	if( ( argc != 2 && argc != 3 ) || argv[ 1 ] == "--help" )
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "pxinvertintensityimagefilter image [outputname]" << std::endl;
		std::cerr << "This program only accepts 2D short images." << std::endl;
		return 1;
	}

	/** Define image type. */
	const unsigned int Dimension = 2;
	typedef short PixelType;

	/** Some typedef's. */
	typedef itk::Image<PixelType, Dimension>						ImageType;
	typedef itk::ImageFileReader< ImageType >						ReaderType;
	typedef itk::ImageFileWriter< ImageType >						WriterType;
	typedef itk::StatisticsImageFilter< ImageType >			StatisticsFilterType;
	typedef StatisticsFilterType::RealType							RealType;
	typedef itk::InvertIntensityImageFilter<ImageType>	InvertIntensityFilterType;
	
	/** Create reader. */
	std::string imageFileName = argv[ 1 ];
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( imageFileName.c_str() );

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
	PixelType max = statistics->GetMaximum();

	/** Create invert filter. */
	InvertIntensityFilterType::Pointer invertFilter = InvertIntensityFilterType::New();
	invertFilter->SetInput( reader->GetOutput() );
	invertFilter->SetMaximum( max );

	/** Create writer. */
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( invertFilter->GetOutput() );

	/** Create and set filename. */
	if ( argc == 3 )
	{
		writer->SetFileName( argv[ 2 ] );
	}
	else
	{
		std::ostringstream outputFileNameSS("");
    outputFileNameSS << imageFileName.substr( 0, imageFileName.rfind( "." ) )
			<< "INVERTED.mhd";
		writer->SetFileName( outputFileNameSS.str().c_str() );
	}

	/** Write output image. */
	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Return a value. */
	return 0;

} // end main


