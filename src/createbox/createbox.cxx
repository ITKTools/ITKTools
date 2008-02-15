#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageDuplicator.h"
#include "itkBoxSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vnl/vnl_math.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run1(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( outputFileName, size, spacing, input1, input2, input, orientation ); \
  supported = true; \
}

#define run2(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( inputFileName, outputFileName, input1, input2, input, orientation ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CreateBox. */
template< class ImageType >
void CreateBox(
  std::string outputFileName,
  std::vector<unsigned int> size,
  std::vector<double> spacing,
  std::vector<double> input1,
	std::vector<double> input2,
  std::string input,
  std::vector<double> orientation );

template< class ImageType >
void CreateBox(
  std::string inputFileName,
  std::string outputFileName,
  std::vector<double> input1,
	std::vector<double> input2,
  std::string input,
  std::vector<double> orientation );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------


int main( int argc, char** argv )
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
  std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

 	std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

 	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

  std::vector<double> spacing( Dimension, 1.0 );
	bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<unsigned int> size( Dimension );
	bool retsz = parser->GetCommandLineArgument( "-sz", size );
  
  std::vector<double> center( Dimension );
	bool retc = parser->GetCommandLineArgument( "-c", center );

	std::vector<double> radius( Dimension );
	bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::vector<double> corner1( Dimension );
	bool retcp1 = parser->GetCommandLineArgument( "-cp1", corner1 );

  std::vector<double> corner2( Dimension );
	bool retcp2 = parser->GetCommandLineArgument( "-cp2", corner2 );

  std::vector<double> cornerindex1( Dimension );
	bool retci1 = parser->GetCommandLineArgument( "-ci1", cornerindex1 );

  std::vector<double> cornerindex2( Dimension );
	bool retci2 = parser->GetCommandLineArgument( "-ci2", cornerindex2 );

  std::vector<double> orientation( Dimension, 0.0 );
	bool reto = parser->GetCommandLineArgument( "-o", orientation );

	/** Check if the required arguments are given. */
	if ( !retout )
	{
		std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
		return 1;
	}
  if ( ( retin && retsz ) || ( !retin && !retsz ) )
	{
		std::cerr << "ERROR: You should specify either \"-in\" or \"-sz\"." << std::endl;
		return 1;
	}
  if ( ( !retc | !retr | retcp1 | retcp2 | retci1 | retci2 )
    && ( retc | retr | !retcp1 | !retcp2 | retci1 | retci2 )
    && ( retc | retr | retcp1 | retcp2 | !retci1 | !retci2 ) )
	{
		std::cerr << "ERROR: Either you should specify \"-c\" and \"-r\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-cp1\" and \"-cp2\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-ci1\" and \"-ci2\"." << std::endl;
		return 1;
	}
  
  /** Determine input image properties. */
  std::string	ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int NumberOfComponents = 1;
  if ( retin )
  {
    int retgip = GetImageProperties(
      inputFileName, PixelType, ComponentType, Dimension, NumberOfComponents, size );
    if ( retgip != 0 )
    {
      return 1;
    }
  }

  /** Let the user overrule this. */
	bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );

  /** Get rid of the possible "_" in ComponentType. */
	ReplaceUnderscoreWithSpace( ComponentType );

  /** How was the input supplied by the user? */
  std::vector<double> input1, input2;
  std::string input = "";
  if ( retc )
  {
    input = "CenterRadius";
    input1 = center;
    input2 = radius;
  }
  else if ( retcp1 )
  {
    input = "CornersAsPoints";
    input1 = corner1;
    input2 = corner2;
  }
  else if ( retci1 )
  {
    input = "CornersAsIndices";
    input1 = cornerindex1;
    input2 = cornerindex2;
  }

  /** Run the program. */
  bool supported = false;
	try
	{
    if ( retin )
    {
      run2( CreateBox, unsigned char, 2 );
      run2( CreateBox, char, 2 );
      run2( CreateBox, unsigned short, 2 );
      run2( CreateBox, short, 2 );
      run2( CreateBox, float, 2 );
      run2( CreateBox, double, 2 );

      run2( CreateBox, unsigned char, 3 );
      run2( CreateBox, char, 3 );
      run2( CreateBox, unsigned short, 3 );
      run2( CreateBox, short, 3 );
      run2( CreateBox, float, 3 );
      run2( CreateBox, double, 3 );
    }
    else if ( retsz )
    {
      run1( CreateBox, unsigned char, 2 );
      run1( CreateBox, char, 2 );
      run1( CreateBox, unsigned short, 2 );
      run1( CreateBox, short, 2 );
      run1( CreateBox, float, 2 );
      run1( CreateBox, double, 2 );

      run1( CreateBox, unsigned char, 3 );
      run1( CreateBox, char, 3 );
      run1( CreateBox, unsigned short, 3 );
      run1( CreateBox, short, 3 );
      run1( CreateBox, float, 3 );
      run1( CreateBox, double, 3 );
    }
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
	 * ******************* CreateBox *******************
	 */

template< class ImageType >
void CreateBox( std::string filename,
  std::vector<unsigned int> size,
  std::vector<double> spacing,
  std::vector<double> input1,
	std::vector<double> input2,
  std::string input,
  std::vector<double> orientation )
{
  /** Typedefs. */
  typedef itk::ImageFileReader< ImageType > 				  ImageReaderType;
	typedef itk::ImageFileWriter< ImageType > 				  ImageWriterType;

  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::BoxSpatialFunction< Dimension >        BoxSpatialFunctionType;
  typedef typename BoxSpatialFunctionType::InputType  InputType;
  typedef itk::ImageRegionIterator< ImageType >		    IteratorType;

  typedef typename ImageType::RegionType			        RegionType;
	typedef typename RegionType::SizeType			          SizeType;
  typedef typename RegionType::SizeValueType	        SizeValueType;
	typedef typename ImageType::PointType			          PointType;
	typedef typename ImageType::IndexType			          IndexType;
	typedef typename ImageType::SpacingType		          SpacingType;

  /** Parse the arguments. */
  SizeType    Size;
  SpacingType Spacing;
  InputType   Orientation;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Size[ i ] = static_cast<SizeValueType>( size[ i ] );
    Spacing[ i ] = spacing[ i ];
    Orientation[ i ] = orientation[ i ];
  }

	/** Create image. */
	RegionType region;
	region.SetSize( Size );
	typename ImageType::Pointer image = ImageType::New();
	image->SetRegions( region );
	image->SetSpacing( Spacing );
	image->Allocate();

  /** Translate input of two opposite corners to center + radius input. */
  InputType Center, Radius;
  PointType point1, point2;
	IndexType index1, index2;
  if ( input == "CornersAsPoints" )
  {
    /** The input is points, copy it. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      point1[ i ] = input1[ i ];
      point2[ i ] = input2[ i ];
    }
  }
  else if ( input == "CornersAsIndices" )
  {
    /** The input is indices, copy and transform to the point. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      index1[ i ] = static_cast<unsigned int>( input1[ i ] );
      index2[ i ] = static_cast<unsigned int>( input2[ i ] );
    }
    image->TransformIndexToPhysicalPoint( index1, point1 );
    image->TransformIndexToPhysicalPoint( index2, point2 );
  }

  /** Compute the center and radius. */
  if ( input != "CenterRadius" )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = ( point1[ i ] + point2[ i ] ) / 2.0;
      Radius[ i ] = Spacing[ i ] + vcl_abs( point1[ i ] - Center[ i ] );
    }
  }
  else
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = point1[ i ];
      Radius[ i ] = point2[ i ];
    }
  }

	/** Create and initialize box. */
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

} // end CreateBox()


  /**
	 * ******************* CreateBox *******************
	 */

template< class ImageType >
void CreateBox(
  std::string inputFileName,
  std::string outputFileName,
  std::vector<double> input1,
	std::vector<double> input2,
  std::string input,
  std::vector<double> orientation )
{
  /** Typedefs. */
  typedef itk::ImageFileReader< ImageType > 				  ImageReaderType;
  typedef itk::ImageDuplicator< ImageType >           DuplicatorType;
	typedef itk::ImageFileWriter< ImageType > 				  ImageWriterType;

  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::BoxSpatialFunction< Dimension >        BoxSpatialFunctionType;
  typedef typename BoxSpatialFunctionType::InputType  InputType;
  typedef itk::ImageRegionIterator< ImageType >		    IteratorType;

	typedef typename ImageType::PointType			          PointType;
	typedef typename ImageType::IndexType			          IndexType;
  typedef typename ImageType::SpacingType		          SpacingType;

  /** Parse the arguments. */
  InputType   Orientation;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Orientation[ i ] = orientation[ i ];
  }

  /** Read input image. */
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();

  /** Copy input image. */
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage( reader->GetOutput() );
  duplicator->Update();
  typename ImageType::Pointer image = duplicator->GetOutput();

  /** Translate input of two opposite corners to center + radius input. */
  SpacingType Spacing = image->GetSpacing();
  InputType Center, Radius;
  PointType point1, point2;
	IndexType index1, index2;
  if ( input == "CornersAsPoints" )
  {
    /** The input is points, copy it. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      point1[ i ] = input1[ i ];
      point2[ i ] = input2[ i ];
    }
  }
  else if ( input == "CornersAsIndices" )
  {
    /** The input is indices, copy and transform to the point. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      index1[ i ] = static_cast<unsigned int>( input1[ i ] );
      index2[ i ] = static_cast<unsigned int>( input2[ i ] );
    }
    image->TransformIndexToPhysicalPoint( index1, point1 );
    image->TransformIndexToPhysicalPoint( index2, point2 );
  }

  /** Compute the center and radius. */
  if ( input != "CenterRadius" )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = ( point1[ i ] + point2[ i ] ) / 2.0;
      Radius[ i ] = Spacing[ i ] + vcl_abs( point1[ i ] - Center[ i ] );
    }
  }
  else
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = point1[ i ];
      Radius[ i ] = point2[ i ];
    }
  }

	/** Create and initialize box. */
	typename BoxSpatialFunctionType::Pointer box = BoxSpatialFunctionType::New();
	box->SetCenter( Center );
	box->SetRadius( Radius );
  box->SetOrientation( Orientation );

	/** Create iterator, index and point. */
	IteratorType it( image, image->GetLargestPossibleRegion() );
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
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( image );
	writer->Update();

} // end CreateBox()


	/**
	 * ******************* PrintHelp *******************
	 */

void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxcreatebox" << std::endl;
  std::cout << "  [-in]    inputFilename, to copy image information from" << std::endl;
	std::cout << "  -out     outputFilename" << std::endl;
  std::cout << "  [-sz]    image size (voxels)" << std::endl;
  std::cout << "  [-sp]    image spacing (mm)" << std::endl;
  std::cout << "  [-c]     center (mm)" << std::endl;
	std::cout << "  [-r]     radii (mm)" << std::endl;
  std::cout << "  [-cp1]   cornerpoint 1 (mm)" << std::endl;
	std::cout << "  [-cp2]   cornerpoint 2 (mm)" << std::endl;
  std::cout << "  [-ci1]   cornerindex 1" << std::endl;
	std::cout << "  [-ci2]   cornerindex 2" << std::endl;
  std::cout << "  [-o]     orientation, default xyz" << std::endl;
	std::cout << "  [-dim]   dimension, default 3" << std::endl;
	std::cout << "  [-pt]    pixelType, default short" << std::endl;
  std::cout << "- The user should EITHER specify the input filename OR the output image size." << std::endl;
  std::cout << "- The user should EITHER specify the center and the radius," << std::endl;
  std::cout << "  OR the positions of two opposite corner points." << std::endl;
  std::cout << "  OR the positions of two opposite corner indices." << std::endl;
  std::cout << "- The orientation is a vector with Euler angles (rad)." << std::endl;
	std::cout << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;
} // end PrintHelp()

