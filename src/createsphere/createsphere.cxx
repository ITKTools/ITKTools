#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkSphereSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( outputFileName, size, spacing, center, radius ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CreateSphere. */
template< class ImageType >
void CreateSphere(
  const std::string & outputFileName,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & center,
	const double & radius );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
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

  std::vector<unsigned int> size;
	bool retsz = parser->GetCommandLineArgument( "-sz", size );
  
  std::vector<double> center;
	bool retc = parser->GetCommandLineArgument( "-c", center );

	double radius;
	bool retr = parser->GetCommandLineArgument( "-r", radius );

	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

	std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

  std::vector<double> spacing( Dimension, 1.0 );
	bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

	/** Check if the required arguments are given. */
	if ( !retout )
	{
		std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
		return 1;
	}
  if ( !retsz )
	{
		std::cerr << "ERROR: You should specify \"-sz\"." << std::endl;
		return 1;
	}
  if ( !retc )
	{
		std::cerr << "ERROR: You should specify \"-c\"." << std::endl;
		return 1;
	}
	if ( !retr )
	{
		std::cerr << "ERROR: You should specify \"-r\"." << std::endl;
		return 1;
	}

  /** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

  /** Run the program. */
  bool supported = false;
	try
	{
    run( CreateSphere, unsigned char, 2 );
		run( CreateSphere, char, 2 );
		run( CreateSphere, unsigned short, 2 );
		run( CreateSphere, short, 2 );
    run( CreateSphere, float, 2 );
    run( CreateSphere, double, 2 );

    run( CreateSphere, unsigned char, 3 );
		run( CreateSphere, char, 3 );
		run( CreateSphere, unsigned short, 3 );
		run( CreateSphere, short, 3 );
    run( CreateSphere, float, 3 );
    run( CreateSphere, double, 3 );
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

 	/** End program. Return a value. */
	return 0;

} // end main 

/**
	 * ******************* CreateSphere *******************
	 */

template< class ImageType >
void CreateSphere(
  const std::string & filename,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & center,
	const double & radius )
{
  /** Typedefs. */
  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::ImageRegionIterator< ImageType >		IteratorType;
  typedef itk::SphereSpatialFunction< Dimension >	SphereSpatialFunctionType;
  typedef typename SphereSpatialFunctionType::InputType    InputType;
	typedef itk::ImageFileWriter< ImageType >				ImageWriterType;

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
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Size[ i ] = static_cast<SizeValueType>( size[ i ] );
    Spacing[ i ] = spacing[ i ];
    Center[ i ] = static_cast<double>( center[ i ] );
  }

	/** Create image. */
	RegionType region;
	region.SetSize( Size );
	typename ImageType::Pointer image = ImageType::New();
	image->SetRegions( region );
	image->SetSpacing( Spacing );
	image->Allocate();

	/** Create and initialize ellipsoid. */
	typename SphereSpatialFunctionType::Pointer sphere = SphereSpatialFunctionType::New();
	sphere->SetCenter( Center );
	sphere->SetRadius( radius );

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
		it.Set( sphere->Evaluate( point ) );
		++it;
  }

	/** Write image. */
	typename ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( filename.c_str() );
	writer->SetInput( image );
	writer->Update();

} // end CreateEllipsoid


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp( void )
{
	std::cout << "Usage:" << std::endl << "pxcreatesphere" << std::endl;
	std::cout << "  -out     outputFilename" << std::endl;
  std::cout << "  -sz      image size (voxels)" << std::endl;
  std::cout << "  [-sp]    image spacing (mm)" << std::endl;
  std::cout << "  -c       center (mm)" << std::endl;
	std::cout << "  -r       radii (mm)" << std::endl;
	std::cout << "  [-dim]   dimension, default 3" << std::endl;
	std::cout << "  [-pt]    pixelType, default short" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;
} // end PrintHelp

