
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkAndNotImageFilter.h"


int main( int argc, char **argv )
{
	/** Check arguments. */
	if( argc != 3 )
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "pxandnotimagefilter image1 image2" << std::endl;
		std::cerr << "This program only accepts 2D short images." << std::endl;
		return 1;
	}

	/** Get the image names. */
	std::string image1Filename = argv[ 1 ];
	std::string image2Filename = argv[ 2 ];

  /** typedefs. */
	typedef short		PixelType;
	const unsigned int Dimension = 2;
  typedef itk::Image< PixelType, Dimension >						ImageType;
  typedef itk::ImageFileReader< ImageType >							ReaderType;
	typedef itk::ImageFileWriter< ImageType >							WriterType;
	typedef itk::AndNotImageFilter< ImageType >						AndNotFilterType;

  /** Read image1. */
  ReaderType::Pointer image1Reader = ReaderType::New();
	image1Reader->SetFileName( image1Filename.c_str() );
  try
	{
		image1Reader->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cerr << "Exception detected while reading "
			<< image1Filename
			<< " : "  << e.GetDescription() << std::endl;
		return 1;
	}

	/** Read image2. */
  ReaderType::Pointer image2Reader = ReaderType::New();
	image2Reader->SetFileName( image2Filename.c_str() );
  try
	{
		image2Reader->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cerr << "Exception detected while reading "
			<< image2Filename
			<< " : "  << e.GetDescription() << std::endl;
		return 1;
	}

  /** The sizes of the image1 and image2 must match. */
	ImageType::SizeType size1 = image1Reader->GetOutput()->GetLargestPossibleRegion().GetSize();
	ImageType::SizeType size2 = image2Reader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( size1 != size2 )
	{
		std::cerr << "The size of the two images do not match!" << std::endl;
		std::cerr << "image1: " << image1Filename
			<< " has size " << size1 << std::endl;
		std::cerr << "image2: " << image2Filename
			<< " has size " << size2 << std::endl;
		return 1;
	}

	/** Create an output image. */
	ImageType::Pointer outputImage;

  /** Now AndNot the two images. */
	AndNotFilterType::Pointer andNotFilter = AndNotFilterType::New();
	andNotFilter->SetInput1( image1Reader->GetOutput() );
	andNotFilter->SetInput2( image2Reader->GetOutput() );

	/** Create writer and filename. */
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( andNotFilter->GetOutput() );
	std::string outputName = image1Filename.substr( 0, image1Filename.rfind( "." ) );
	outputName += "ANDNOT";
	outputName += image2Filename.substr( 0, image2Filename.rfind( "." ) );
	outputName += ".mhd";	
	writer->SetFileName( outputName.c_str() );

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

