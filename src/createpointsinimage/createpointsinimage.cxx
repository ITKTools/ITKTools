#ifndef __CreatePointsInImage_CXX__
#define __CreatePointsInImage_CXX__

#include "itkSphereSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"

#include <fstream>


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	/** Check number of arguments. */
	if ( argc < 5 )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "CreatePointsInImage pointsfilename imagename -s imagesize [-sp spacing]" << std::endl;
		std::cout << "NOTE: only 2D short are created and arguments should be in above order." << std::endl;
		return 1;
	}

	// Some consts.
	const unsigned int	Dimension = 2;
	typedef short	PixelType;

	// TYPEDEF's
	typedef itk::Image< PixelType, Dimension >					ImageType;
	typedef itk::ImageRegionIterator< ImageType >				IteratorType;
	typedef itk::ImageFileWriter< ImageType >						ImageWriterType;

	typedef ImageType::RegionType			RegionType;
	typedef RegionType::SizeType			SizeType;
	typedef ImageType::PointType			PointType;
	typedef ImageType::IndexType			IndexType;
	typedef ImageType::SpacingType		SpacingType;

	/** Get arguments. */
	std::string pointfilename = argv[ 1 ];
	std::string imagefilename = argv[ 2 ];
	unsigned int pos_s, pos_sp;
	pos_s = pos_sp = 0;
	
	/** Get positions of -?. */
	for ( unsigned int i = 2; i < argc; i++ )
	{
		std::string temp = argv[ i ];
		//std::cout << "argv[ " << i << " ] = " << argv[ i ] << std::endl;
		if ( temp == "-s" ) pos_s = i;
		if ( temp == "-sp" ) pos_sp = i;
	}

	/** Get spacing. */
	SpacingType spacing;
	if ( pos_sp == 0 )
	{
		spacing.Fill( 1.0 );
	}
	else if ( argc - pos_sp == 2 )
	{
		spacing.Fill( atof( argv[ pos_sp + 1 ] ) );
	}
	else if ( argc - pos_sp == 3 )
	{
		spacing[ 0 ] = atof( argv[ pos_sp + 1 ] );
		spacing[ 1 ] = atof( argv[ pos_sp + 2 ] );
		//spacing[ 2 ] = atof( argv[ pos_sp + 3 ] );
	}
	else
	{
		std::cerr << "ERROR: wrong commandline arguments." << std::endl;
		return 1;
	}

	/** Get size. */
	if ( pos_sp == 0 ) pos_sp = argc;
	SizeType size;
	size.Fill( 0 );
	if ( pos_sp - pos_s == 2 )
	{
		size.Fill( atoi( argv[ pos_s + 1 ] ) );
	}
	else if ( pos_sp - pos_s == 3 )
	{
		size[ 0 ] = atoi( argv[ pos_s + 1 ] );
		size[ 1 ] = atoi( argv[ pos_s + 2 ] );
		//size[ 2 ] = atoi( argv[ pos_s + 3 ] );
	}
	else
	{
		std::cerr << "ERROR: wrong commandline arguments." << std::endl;
		return 1;
	}

	/** Create size, region, image. */
	RegionType region;
	region.SetSize( size );
	ImageType::Pointer image = ImageType::New();
	image->SetRegions( region );
	image->SetSpacing( spacing );
	image->Allocate();

	/** Walk over the image and set everything to zero. */
	IteratorType it( image, region );
	it.GoToBegin();
	while ( !it.IsAtEnd() )
	{
		it.Set( 0 );
		/** Increase iterator. */		
		++it;
	} // end while

	/** Open the file containing the inputpoints.*/
	std::ifstream pointfile( pointfilename.c_str() );
	unsigned int nrofpoints;
	IndexType inputIndex;
	PixelType valueofpoint = 1;
	if ( pointfile.is_open() )
	{
		/** Read the inputpoints from a text file.*/
		pointfile >> nrofpoints;
		for ( unsigned int j = 0; j < nrofpoints; j++ )
		{
			/** Get the points from the file; they are given in iX coordinates. */
			pointfile >> inputIndex[ 0 ];
			pointfile >> inputIndex[ 1 ];
			inputIndex[ 1 ] = size[ 1 ] - inputIndex[ 1 ];
			/** Set this point to some number, ranging from 1 to nrofpoints. */
			image->SetPixel( inputIndex, valueofpoint );
			valueofpoint++;
		}
	}
	else
	{
		std::cerr << "WARNING: the file \"" << pointfilename
			<< "\" could not be opened!" << std::endl;
		return 1;
	} // end if
	
	/** Write image. */
	ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( imagefilename.c_str() );
	writer->SetInput( image );

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

	/** End program. Return a value. */
	return 0;

} // end main


#endif // #ifndef __CreatePointsInImage_CXX__
