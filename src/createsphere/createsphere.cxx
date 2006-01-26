#ifndef __CreateSphere_CXX__
#define __CreateSphere_CXX__

#include "itkSphereSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	/** Check number of arguments. */
	if ( argc < 6 )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "CreateSphere imagename -s imagesize -r radius [-sp spacing]" << std::endl;
		std::cout << "NOTE: only 3D short are created and arguments should be in above order." << std::endl;
		return 1;
	}

	// Some consts.
	const unsigned int	Dimension = 3;
	typedef short	PixelType;

	// TYPEDEF's
	typedef itk::Image< PixelType, Dimension >					ImageType;
	typedef itk::ImageRegionIterator< ImageType >				IteratorType;
	typedef itk::SphereSpatialFunction< Dimension >			SphereSpatialFunctionType;
	typedef itk::ImageFileWriter< ImageType >						ImageWriterType;

	typedef ImageType::RegionType			RegionType;
	typedef RegionType::SizeType			SizeType;
	typedef ImageType::PointType			PointType;
	typedef ImageType::IndexType			IndexType;
	typedef ImageType::SpacingType		SpacingType;

	/** Get arguments. */
	std::string filename = argv[ 1 ];
	unsigned int pos_s, pos_r, pos_sp;
	pos_s = pos_r = pos_sp = 0;
	
	/** Get positions of -?. */
	for ( unsigned int i = 2; i < argc; i++ )
	{
		std::string temp = argv[ i ];
		//std::cout << "argv[ " << i << " ] = " << argv[ i ] << std::endl;
		if ( temp == "-s" ) pos_s = i;
		if ( temp == "-r" ) pos_r = i;
		if ( temp == "-sp" ) pos_sp = i;
	}

	/** Get size. */
	SizeType size;
	size.Fill( 0 );
	if ( pos_r - pos_s == 2 )
	{
		size.Fill( atoi( argv[ pos_s + 1 ] ) );
	}
	else if ( pos_r - pos_s == 4 )
	{
		size[ 0 ] = atoi( argv[ pos_s + 1 ] );
		size[ 1 ] = atoi( argv[ pos_s + 2 ] );
		size[ 2 ] = atoi( argv[ pos_s + 3 ] );
	}
	else
	{
		std::cerr << "ERROR: wrong commandline arguments." << std::endl;
		return 1;
	}

	/** Get radius. */
	double radius = 0.0;
	if ( pos_sp == 0 && pos_r == argc - 2 )
	{
		radius = atof( argv[ pos_r + 1 ] );
	}
	else if ( pos_sp != 0 && pos_sp -pos_r == 2 )
	{
		radius = atof( argv[ pos_r + 1 ] );
	}
	else
	{
		std::cerr << "ERROR: wrong commandline arguments." << std::endl;
		return 1;
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
	else if ( argc - pos_sp == 4 )
	{
		spacing[ 0 ] = atof( argv[ pos_sp + 1 ] );
		spacing[ 1 ] = atof( argv[ pos_sp + 2 ] );
		spacing[ 2 ] = atof( argv[ pos_sp + 3 ] );
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

	/** Get the center of the image. */
	IndexType centerIndex;
	PointType centerPoint;
	centerIndex.Fill( 0 );
	centerPoint.Fill( 0.0 );
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		centerIndex[ i ] = size[ i ] / 2;
	}
	image->TransformIndexToPhysicalPoint( centerIndex, centerPoint );

	/** Create and initialize sphereSpatialFunction. */
	SphereSpatialFunctionType::Pointer sphereFunction = SphereSpatialFunctionType::New();
	sphereFunction->SetCenter( centerPoint );
	sphereFunction->SetRadius( radius );

	/** Create iterator, index and point. */
	IteratorType it( image, region );
	it.GoToBegin();
	PointType point;
	IndexType index;

	/** Walk over the image. */
	while ( !it.IsAtEnd() )
	{
		index = it.GetIndex();
		image->TransformIndexToPhysicalPoint( index, point );
		it.Set( sphereFunction->Evaluate( point ) );
		/** Increase iterator. */		
		++it;
	} // end while

	/** Write image. */
	ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( filename.c_str() );
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


#endif // #ifndef __CreateSphere_CXX__
