#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkBoxSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "vnl/vnl_math.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > ImageType; \
    function< ImageType >( outputFileName, size, spacing, center, radius, orientation ); \
}

//-------------------------------------------------------------------------------------

/** Declare CreateBox. */
template< class ImageType >
void CreateBox( std::string outputFileName,
  std::vector<unsigned int> size,
  std::vector<double> spacing,
  std::vector<double> center,
	std::vector<double> radius,
  std::vector<double> orientation );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------


int main(int argc, char** argv)
{
  /** Check arguments for help. */
	if ( argc < 5 )
	{
		PrintHelp();
		return 1;
	}

  /** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
 	std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

 	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

  std::vector<double> spacing( Dimension, 1.0 );
	bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<unsigned int> size( Dimension );
	bool rets = parser->GetCommandLineArgument( "-s", size );
  
  std::vector<double> center( Dimension );
	bool retc = parser->GetCommandLineArgument( "-c", center );

	std::vector<double> radius( Dimension );
	bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::vector<double> corner1( Dimension );
	bool retcp1 = parser->GetCommandLineArgument( "-cp1", corner1 );

  std::vector<double> corner2( Dimension );
	bool retcp2 = parser->GetCommandLineArgument( "-cp2", corner2 );

  std::vector<double> orientation( Dimension * Dimension, 0.0 );
	bool reto = parser->GetCommandLineArgument( "-o", orientation );

	/** Check if the required arguments are given. */
	if ( !retout )
	{
		std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
		return 1;
	}
  if ( !rets )
	{
		std::cerr << "ERROR: You should specify \"-s\"." << std::endl;
		return 1;
	}
  if ( ( !retc | !retr | retcp1 | retcp2 ) && ( retc | retr | !retcp1 | !retcp2 ) )
	{
		std::cerr << "ERROR: Either you should specify \"-c\" and \"-r\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-cp1\" and \"-cp2\"." << std::endl;
		return 1;
	}
  if ( !reto )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      orientation[ i * ( Dimension + 1 ) ] = 1.0;
    }
  }

  /** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

  /** Translate input of two opposite corners to center + radius input. */
  if ( retcp1 )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      center[ i ] = ( corner1[ i ] + corner2[ i ] ) / 2.0;
      radius[ i ] = vcl_abs( corner1[ i ] - center[ i ] );
    }
  }

  /** Run the program. */
	try
	{
		run(CreateBox,unsigned char,2);
		run(CreateBox,unsigned char,3);
		run(CreateBox,char,2);
		run(CreateBox,char,3);
		run(CreateBox,unsigned short,2);
		run(CreateBox,unsigned short,3);
		run(CreateBox,short,2);
		run(CreateBox,short,3);
    run(CreateBox,float,2);
		run(CreateBox,float,3);
    run(CreateBox,double,2);
		run(CreateBox,double,3);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

 	/** End program. Return a value. */
	return 0;

} // end main 


  /**
	 * ******************* CreateBox *******************
	 */

template< class ImageType >
void CreateBox( std::string filename,
  std::vector<unsigned int> size,
  std::vector<double> spacing,
  std::vector<double> center,
	std::vector<double> radius,
  std::vector<double> orientation )
{
  /** Typedefs. */
  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::ImageRegionIterator<ImageType>		  IteratorType;
	typedef itk::BoxSpatialFunction<Dimension>      BoxSpatialFunctionType;
  typedef typename BoxSpatialFunctionType::InputType       InputType;
	typedef itk::ImageFileWriter<ImageType> 				ImageWriterType;

	typedef typename ImageType::RegionType			RegionType;
	typedef typename RegionType::SizeType			SizeType;
  typedef typename RegionType::SizeValueType	SizeValueType;
	typedef typename ImageType::PointType			PointType;
	typedef typename ImageType::IndexType			IndexType;
	typedef typename ImageType::SpacingType		SpacingType;

  /** Parse the arguments. */
  SizeType    Size;
  SpacingType Spacing;
  InputType   Center;
  InputType   Radius;
  InputType   Orientation;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Size[ i ] = static_cast<SizeValueType>( size[ i ] );
    Spacing[ i ] = spacing[ i ];
    Center[ i ] = center[ i ];
    Radius[ i ] = radius[ i ];
    Orientation[ i ] = orientation[ i ];
  }

	/** Create image. */
	RegionType region;
	region.SetSize( Size );
	typename ImageType::Pointer image = ImageType::New();
	image->SetRegions( region );
	image->SetSpacing( Spacing );
	image->Allocate();

	/** Create and initialize ellipsoid. */
	typename BoxSpatialFunctionType::Pointer box = BoxSpatialFunctionType::New();
	box->SetCenter( Center );
	box->SetRadius( Radius );
  box->SetOrientation( Orientation );

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
		it.Set( box->Evaluate( point ) );
		++it;
	} // end while

	/** Write image. */
	typename ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( filename.c_str() );
	writer->SetInput( image );
	writer->Update();

} // end CreateBox


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxcreatebox" << std::endl;
	std::cout << "\t-out\toutputFilename" << std::endl;
  std::cout << "\t-s\timage size (voxels)" << std::endl;
  std::cout << "\t[-sp]\timage spacing (mm)" << std::endl;
  std::cout << "\t[-c]\tcenter (mm)" << std::endl;
	std::cout << "\t[-r]\tradii (mm)" << std::endl;
  std::cout << "\t[-cp1]\tcornerpoint 1 (mm)" << std::endl;
	std::cout << "\t[-cp2]\tcornerpoint 2 (mm)" << std::endl;
  std::cout << "\t[-o]\torientation, default xyz" << std::endl;
	std::cout << "\t[-dim]\tdimension, default 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType, default short" << std::endl;
  std::cout << "The user should EITHER specify the center and the radius," << std::endl;
  std::cout << "OR the positions of two opposite corner points." << std::endl;
  std::cout << "The orientation is a vector with the Euler angles (rad)." << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;
} // end PrintHelp

