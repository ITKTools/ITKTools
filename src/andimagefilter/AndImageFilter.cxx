
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkAndImageFilter.h"


int main( int argc, char **argv )
{
	/** Check arguments. */
	if( argc != 3 && argc != 4 )
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "pxandimagefilter image1 image2 [outputname]" << std::endl;
		std::cerr << "This program only accepts 2D short images." << std::endl;
		return 1;
	}
	
	/** Print arguments. */
	std::cout << "pxandimagefilter ";
	for ( unsigned int i = 1; i < argc; i++ )
	{
		std::cout << argv[ i ] << " ";
	}
	std::cout << std::endl;

	/** Get the image names. */
	std::string image1FileName = argv[ 1 ];
	std::string image2FileName = argv[ 2 ];
	std::string outputFileName = "";
	if ( argc == 4 )
	{
		outputFileName = argv[ 3 ];
	}

  /** typedefs. */
	typedef short		PixelType;
	const unsigned int Dimension = 2;
  typedef itk::Image< PixelType, Dimension >						ImageType;
  typedef itk::ImageFileReader< ImageType >							ReaderType;
	typedef itk::ImageFileWriter< ImageType >							WriterType;
	typedef itk::AndImageFilter<
		ImageType, ImageType, ImageType >										AndFilterType;

  /** Read image1. */
  ReaderType::Pointer image1Reader = ReaderType::New();
	image1Reader->SetFileName( image1FileName.c_str() );
  try
	{
		image1Reader->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cerr << "Exception detected while reading "
			<< image1FileName
			<< " : "  << e.GetDescription() << std::endl;
		return 1;
	}

	/** Read image2. */
  ReaderType::Pointer image2Reader = ReaderType::New();
	image2Reader->SetFileName( image2FileName.c_str() );
  try
	{
		image2Reader->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cerr << "Exception detected while reading "
			<< image2FileName
			<< " : "  << e.GetDescription() << std::endl;
		return 1;
	}

  /** The sizes of the image1 and image2 must match. */
	ImageType::SizeType size1 = image1Reader->GetOutput()->GetLargestPossibleRegion().GetSize();
	ImageType::SizeType size2 = image2Reader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( size1 != size2 )
	{
		std::cerr << "The size of the two images do not match!" << std::endl;
		std::cerr << "image1: " << image1FileName
			<< " has size " << size1 << std::endl;
		std::cerr << "image2: " << image2FileName
			<< " has size " << size2 << std::endl;
		return 1;
	}

	/** Create an output image. */
	ImageType::Pointer outputImage;

  /** Now AndNot the two images. */
	AndFilterType::Pointer andFilter = AndFilterType::New();
	andFilter->SetInput1( image1Reader->GetOutput() );
	andFilter->SetInput2( image2Reader->GetOutput() );

	/** Create writer. */
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( andFilter->GetOutput() );

	/** Create and set filename. */
	if ( argc == 3 )
	{
		std::string::size_type slash = image2FileName.find_last_of( "/" ) + 1;
    outputFileName = image1FileName.substr( 0, image1FileName.rfind( "." ) );
		outputFileName += "AND";
		outputFileName += image2FileName.substr( slash, image2FileName.rfind( "." ) - slash );
		outputFileName += ".mhd";
		writer->SetFileName( outputFileName.c_str() );
	}
	else
	{
		writer->SetFileName( outputFileName.c_str() );
	}

	/** Print output filename. */
	std::cout << "Output filename: " << outputFileName << std::endl;

	/** Write difference image. */
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

