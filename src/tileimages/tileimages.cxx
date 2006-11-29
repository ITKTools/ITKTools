
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkTileImageFilter.h"
#include "itkImageFileWriter.h"
#include <vector>
#include <string>

/** This program tiles a stacks of 2D images into a 3D image.
 * This is done by employing an itk::SeriesFileReader.
 *
 */

/** run: A macro to call a function. */
#define runA(function,type) \
if ( ComponentType == #type ) \
{ \
  function< type >( inputFileNames, outputFileName, zspacing ); \
}

#define runB(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileNames, outputFileName, layout, defaultvalue ); \
}

//-------------------------------------------------------------------------------------

/** Declare TileImages2D3D. */
template< class PixelType >
void TileImages2D3D(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  double zspacing );

/** Declare TileImages. */
template< class PixelType, unsigned int Dimension >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::vector<unsigned int> & layout,
  double defaultvalue );

/** Declare PrintHelp function. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
	if ( argc < 6 )
	{
    PrintHelp();
		return 1;
	}

  /** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  /** Get the input file names. */
	std::vector< std::string >	inputFileNames;
	bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if ( inputFileNames.size() < 2 )
	{
		std::cout << "ERROR: You should specify at least two input images." << std::endl;
    return 1;
	}

  /** Get the outputFileName. */
  std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
  if ( !retout )
  {
    std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
    return 1;
  }

  /** Read the z-spacing. */
  double zspacing = -1.0;
	bool retsp = parser->GetCommandLineArgument( "-sp", zspacing );

  /** Get the layout. */
	std::vector< unsigned int >	layout;
	bool retly = parser->GetCommandLineArgument( "-ly", layout );

  /** Get the layout. */
	double defaultvalue = 0.0;
	bool retd = parser->GetCommandLineArgument( "-d", defaultvalue );

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileNames[ 0 ],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );

  if ( retgip != 0 )
  {
    return 1;
  }

  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  
  /** Let the user overrule this. */
	bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if ( retpt )
  {
    std::cout << "The user has overruled this by specifying -pt:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if ( NumberOfComponents > 1 )
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

	/** Get rid of the possible "_" in ComponentType. */
	ReplaceUnderscoreWithSpace( ComponentType );
	
	/** Run the program. */	
  try
	{
    if ( !retly )
    {
      runA( TileImages2D3D, unsigned char );
      runA( TileImages2D3D, char );
      runA( TileImages2D3D, unsigned short );
      runA( TileImages2D3D, short );
      runA( TileImages2D3D, float );
    }
    else
    {
      runB( TileImages, unsigned char, 2 );
      runB( TileImages, char, 2 );
      runB( TileImages, unsigned short, 2 );
      runB( TileImages, short, 2 );
      runB( TileImages, float, 2 );

      runB( TileImages, unsigned char, 3 );
      runB( TileImages, char, 3 );
      runB( TileImages, unsigned short, 3 );
      runB( TileImages, short, 3 );
      runB( TileImages, short, 3 );
      runB( TileImages, float, 3 );
    }
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Return a value. */
	return 0;

} // end main

//-------------------------------------------------------------------------------------

/** Define TileImages2D3D. */
template< class PixelType >
void TileImages2D3D(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  double zspacing )
{
 	/** Define image type. */
	const unsigned int Dimension = 3;

	/** Some typedef's. */
	typedef itk::Image<PixelType, Dimension>						ImageType;
	typedef typename ImageType::SpacingType					  	SpacingType;
	typedef itk::ImageSeriesReader<ImageType>	  				ImageSeriesReaderType;
	typedef itk::ImageFileWriter<ImageType>		  				ImageWriterType;

  /** Create reader. */
	typename ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
	reader->SetFileNames( inputFileNames );

	/** Update the reader. */
  std::cout << "Input images are read..." << std::endl;
	reader->Update();
  std::cout << "Reading done." << std::endl;
  typename ImageType::Pointer tiledImage = reader->GetOutput();
	
	/** Get and set the spacing, if it was set by the user. */
  if ( zspacing > 0.0 )
  {
    /** Make sure that changes are not undone */
    tiledImage->DisconnectPipeline();
    /** Set the zspacing */
	  SpacingType spacing = tiledImage->GetSpacing();
	  spacing[ 2 ] = zspacing;
	  tiledImage->SetSpacing( spacing );
  }

	/** Write to disk. */
	typename ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( tiledImage );
  std::cout << "Writing tiled image..." << std::endl;
	writer->Update();
  std::cout << "Ready." << std::endl;
		
} // end TileImages2D3D

//-------------------------------------------------------------------------------------

/** Define TileImages. */
template< class PixelType, unsigned int Dimension >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::vector<unsigned int> & layout,
  double defaultvalue )
{
	/** Some typedef's. */
  typedef itk::Image<PixelType, Dimension>						ImageType;
  typedef itk::ImageFileReader<ImageType>					    ImageReaderType;
	typedef itk::TileImageFilter<ImageType, ImageType>  TilerType;
	typedef itk::ImageFileWriter<ImageType>		  				ImageWriterType;
  //typedef typename ImageType::SpacingType					    SpacingType;

  /** Copy layout into a fixed array. */
  itk::FixedArray< unsigned int, Dimension > Layout;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Layout[ i ] = layout[ i ];
  }

  /** Cast the defaultvalue. */
  PixelType defaultValue = static_cast<PixelType>( defaultvalue );

  /** Create tiler. */
  typename TilerType::Pointer tiler = TilerType::New();
  tiler->SetLayout( Layout );
  tiler->SetDefaultPixelValue( defaultValue );

  /** Read input images and set it into the tiler. */
  for ( unsigned int i = 0; i < inputFileNames.size(); i++ )
  {
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( inputFileNames[ i ].c_str() );
    reader->Update();
    tiler->SetInput( i, reader->GetOutput() );
  }

  /** Do the tiling. */
  tiler->Update();

	/** Write to disk. */
	typename ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( tiler->GetOutput() );
	writer->Update();
		
} // end TileImages

//-------------------------------------------------------------------------------------

/** Define PrintHelp. */
void PrintHelp(void)
{
  std::cout << "pxtileimages EITHER tiles a stack of 2D images into a 3D image," << std::endl;
  std::cout << "OR tiles nD images to form another nD image." << std::endl;
  std::cout << "In the last case the way to tile is specified by a layout." << std::endl;
  std::cout << "To stack a pile of 2D images an itk::SeriesFileReader is employed." << std::endl;
  std::cout << "If no layout is specified with \"-ly\" 2D-3D tiling is done," << std::endl;
  std::cout << "otherwise 2D-2D or 3D-3D tiling is performed.\n" << std::endl;

  std::cout << "Usage:  \npxtileimages" << std::endl;
  std::cout << "\t-in   \tinput image filenames, at least 2" << std::endl;
  std::cout << "\t-out  \toutput image filename" << std::endl;
  std::cout << "\t[-pt] \tpixel type of input and output images" << std::endl;
  std::cout << "\t      \tdefault: automatically determined from the first input image" << std::endl;
  std::cout << "\t[-sp] \tspacing in z-direction for 2D-3D tiling [double];" << std::endl;
  std::cout << "\t      \tif omitted, the origins of the 2d images are used to find the spacing;" << std::endl;
  std::cout << "\t      \tif these are identical, a spacing of 1.0 is assumed" << std::endl;
  std::cout << "\t[-ly] \tlayout of the nD-nD tiling" << std::endl;
  std::cout << "\t      \texample: in 2D for 4 images \"-ly 2 2\" results in" << std::endl;
  std::cout << "\t      \t\t im1 im2" << std::endl;
  std::cout << "\t      \t\t im3 im4" << std::endl;
  std::cout << "\t      \texample: in 2D for 4 images \"-ly 4 1\" (or \"-ly 0 1\") results in" << std::endl;
  std::cout << "\t      \t\t im1 im2 im3 im4" << std::endl;
  std::cout << "\t[-d]  \tdefault value, by default 0." << std::endl;
  std::cout << "Supported pixel types: (unsigned) char, (unsigned) short, float.\n" << std::endl; 

} // end PrintHelp


