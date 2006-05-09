
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkAddToPixelImageFilter.h"


int main( int argc, char **argv )
{
	/** typedefs. */
	typedef short				PixelType;
	const unsigned int	Dimension = 2;

	/** Check arguments. */
	if( ( argc != 3 && argc != 4 ) || argv[ 1 ] == "--help" )
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "pxaddtopixelimagefilter image addition [outputname]" << std::endl;
		std::cerr << "This program only accepts 2D short images." << std::endl;
		return 1;
	}
	
	/** Get the image name, the addition, and the output name. */
	std::string imageFileName = argv[ 1 ];
	PixelType addition = atoi( argv[ 2 ] );
	std::string outputFileName = "";
	if ( argc == 4 )
	{
		outputFileName = argv[ 3 ];
	}

  /** typedefs. */
  typedef itk::Image< PixelType, Dimension >						ImageType;
  typedef itk::ImageFileReader< ImageType >							ReaderType;
	typedef itk::ImageFileWriter< ImageType >							WriterType;
	typedef itk::AddToPixelImageFilter<
		ImageType, ImageType >															AddToPixelImageFilterType;

  /** Read image. */
  ReaderType::Pointer imageReader = ReaderType::New();
	imageReader->SetFileName( imageFileName.c_str() );
  try
	{
		imageReader->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cerr << "Exception detected while reading "
			<< imageFileName
			<< " : "  << e.GetDescription() << std::endl;
		return 1;
	}

  /** Now minimum the two images. */
	AddToPixelImageFilterType::Pointer addToPixelFilter = AddToPixelImageFilterType::New();
	addToPixelFilter->SetInput( imageReader->GetOutput() );
	addToPixelFilter->SetAddition( addition );

	/** Create writer. */
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( addToPixelFilter->GetOutput() );

	/** Create and set filename. */
	if ( argc == 3 )
	{
		std::ostringstream outputFileNameSS("");
    outputFileNameSS << imageFileName.substr( 0, imageFileName.rfind( "." ) )
			<< "PLUS"
			<< addition
			<< ".mhd";
		writer->SetFileName( outputFileNameSS.str().c_str() );
	}
	else
	{
		writer->SetFileName( outputFileName.c_str() );
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

