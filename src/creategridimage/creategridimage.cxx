#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileWriter.h"

#include <map>
#include <string>

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	/** Check number of arguments. */
	if ( argc < 2 || argc > 8 || argv[ 1 ] == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "\tpxreategridimage filename [-s imagesize] [-sp spacing] [-d distance]" << std::endl;
		std::cout << "\tDefaults: -s = 256, -sp = 1.0, -d = 16" << std::endl;
		std::cout << "\tThis program only creates 2D short grid images." << std::endl;
		return 1;
	}

	/** Some consts. */
	const unsigned int	Dimension = 2;
	typedef short				PixelType;

	/** Typedef's. */
	typedef itk::Image< PixelType, Dimension >							ImageType;
	typedef itk::ImageRegionIteratorWithIndex< ImageType >	IteratorType;
	typedef itk::ImageFileWriter< ImageType >								WriterType;

	typedef ImageType::RegionType		RegionType;
	typedef ImageType::SizeType			SizeType;
	typedef ImageType::IndexType		IndexType;
	typedef ImageType::SpacingType	SpacingType;
	typedef ImageType::PointType		OriginType;

	typedef std::map<std::string, std::string>					ArgMapType;

	/*
	 * Get command line arguments.
	 */
	ArgMapType argmap;
	for ( unsigned int i = 2; i < argc; i += 2 )
	{
		std::string key = argv[ i ];
		std::string value = "";
		if ( ( i + 1 ) < argc )
		{
		  value = argv[ i + 1 ];
		}
		argmap[ key ] = value;
	}

	/** Defaults. */
	std::string filename = argv[ 1 ];
	unsigned int imagesize = 256;
	unsigned int distance = 16;
	float spacing1D = 1.0;
	/** User specified. */
	if ( argmap.count("-s") ) imagesize = atoi( argmap["-s"].c_str() );
	if ( argmap.count("-d") ) distance = atoi( argmap["-d"].c_str() );
	if ( argmap.count("-sp") ) spacing1D = atof( argmap["-sp"].c_str() );

	/*
	 * Create an image.
	 */
	ImageType::Pointer image = ImageType::New();
	SizeType	size;
	IndexType index;
	RegionType region;
	size.Fill( imagesize );
	index.Fill(0);
	region.SetIndex(index);
	region.SetSize(size);

	OriginType origin;
	origin.Fill( 0.0 );
	SpacingType spacing;
	spacing.Fill( spacing1D );

  image->SetRegions( region );
	image->SetOrigin( origin );
	image->SetSpacing( spacing );
	image->Allocate();

	/*
	 * ********************** Fill the image ********************
	 */

	/** Declare and initialize iterators. */
	IteratorType	it( image, region );

	/** Loop over the image and fill it. */
	it.GoToBegin();
	IndexType ind;
	while ( !it.IsAtEnd() )
	{
		ind = it.GetIndex();
		/** The if() defines a grid. */
		if ( ind[ 0 ] % distance == 0 || ind[ 1 ] % distance == 0 )
		{
			it.Set( 1 );
		}
		else
		{
			it. Set( 0 );
		}
		/** Increase iterator. */
		++it;
	} // end while

	/*
	 * Write result to file.
	 */
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( filename.c_str() );
	writer->SetInput( image );
	try
	{
		writer->Update();
	}
	catch ( itk::ExceptionObject & excp )
	{
		std::cerr << excp << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main

