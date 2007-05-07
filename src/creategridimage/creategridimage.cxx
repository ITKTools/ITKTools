#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, dim ) \
if ( imageDimension == dim ) \
{ \
    function< dim >( outputFileName, imageSize, imageSpacing, distance ); \
}

//-------------------------------------------------------------------------------------

/** Declare functions. */
void PrintHelp( void );

template<unsigned int Dimension>
void CreateGridImage( const std::string & outputFileName,
  const std::vector<unsigned int> & imageSize,
  const std::vector<float> & imageSpacing,
  const std::vector<unsigned int> & distance );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check arguments for help. */
	if ( argc < 7 || argc > 17 )
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

  unsigned int imageDimension = 2;
  bool retdim = parser->GetCommandLineArgument( "-dim", imageDimension );

  std::vector<unsigned int> imageSize( imageDimension );
	bool retsz = parser->GetCommandLineArgument( "-sz", imageSize );

  std::vector<float> imageSpacing( imageDimension, 1.0 );
	bool retsp = parser->GetCommandLineArgument( "-sp", imageSpacing );

  std::vector<unsigned int> distance( imageDimension, 1 );
	bool retd = parser->GetCommandLineArgument( "-d", distance );

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
  if ( !retd )
	{
		std::cerr << "ERROR: You should specify \"-d\"." << std::endl;
		return 1;
	}

  /** Check arguments. */
  if ( imageDimension < 2 || imageDimension > 3 )
  {
    std::cerr << "ERROR: Only image dimensions of 2 or 3 are supported." << std::endl;
		return 1;
  }
  for ( unsigned int i = 0; i < imageDimension; ++i )
  {
    if ( imageSize[ i ] == 0 )
    {
      std::cerr << "ERROR: image size[" << i << "] = 0." << std::endl;
      return 1;
    }
    if ( distance[ i ] == 0 ) distance[ i ] = 1;
  }

  /** Run the program. */
	try
	{
    run( CreateGridImage, 2 );
    run( CreateGridImage, 3 );
  }
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

  /** End program. */
	return 0;

} // end main

	
/**
 * ******************* CreateGridImage *******************
 */

template< unsigned int Dimension >
void CreateGridImage( const std::string & outputFileName,
  const std::vector<unsigned int> & imageSize,
  const std::vector<float> & imageSpacing,
  const std::vector<unsigned int> & distance )
{
	/** Typedef's. */
  typedef short				                                    PixelType;
	typedef itk::Image< PixelType, Dimension >							ImageType;
	typedef itk::ImageRegionIteratorWithIndex< ImageType >	IteratorType;
	typedef itk::ImageFileWriter< ImageType >								WriterType;

	typedef typename ImageType::SizeType			SizeType;
	typedef typename ImageType::IndexType		IndexType;
	typedef typename ImageType::SpacingType	SpacingType;

	/* Create image and writer. */
	typename ImageType::Pointer  image  = ImageType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Allocate image. */
	SizeType	  size;
  SpacingType spacing;
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    size[ i ] = imageSize[ i ];
    spacing[ i ] = imageSpacing[ i ];
  }
  image->SetRegions( size );
	image->SetSpacing( spacing );
	image->Allocate();

	/* Fill the image. */
  IteratorType	it( image, image->GetLargestPossibleRegion() );
	it.GoToBegin();
	IndexType ind;
	while ( !it.IsAtEnd() )
	{
    /** Check if on grid. */
		ind = it.GetIndex();
    bool onGrid = false;
    onGrid |= ind[ 0 ] % distance[ 0 ] == 0;
    onGrid |= ind[ 1 ] % distance[ 1 ] == 0;
    if ( Dimension == 3 )
    {
      if ( ind[ 2 ] % distance[ 2 ] != 0 )
      {
        onGrid = ind[ 0 ] % distance[ 0 ] == 0;
        onGrid &= ind[ 1 ] % distance[ 1 ] == 0;
      }
    }
    /** Set the value and continue. */
		if ( onGrid ) it.Set( 1 );
    else it.Set( 0 );
		++it;
	} // end while

	/* Write result to file. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( image );
  writer->Update();

} // end CreateGridImage()


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp( void )
{
	std::cout << "Usage:" << std::endl << "pxreategridimage" << std::endl;
	std::cout << "  -out     outputFilename" << std::endl;
  std::cout << "  [-dim]   image dimension, default 2" << std::endl;
  std::cout << "  -sz      image size" << std::endl;
  std::cout << "  [-sp]    image spacing, default 1.0" << std::endl;
  std::cout << "  -d       distance in pixels between two gridlines" << std::endl;
	std::cout << "Supported: 2D, 3D, short." << std::endl;
  std::cout << "In 3D simply a stack of 2D grid image is created." << std::endl;
} // end PrintHelp()

