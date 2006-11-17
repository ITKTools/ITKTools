
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include <vector>
#include <string>

/** This program tiles a stacks of 2D images into a 3D image.
 * This is done by employing an itk::SeriesFileReader.
 *
 */

/** run: A macro to call a function. */
#define run(function,type) \
if ( ComponentType == #type ) \
{ \
    function< type >( inputFileNames, outputFileName, zspacing ); \
}

//-------------------------------------------------------------------------------------

/** Declare TileImages. */
template< class PixelType >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  double zspacing);

/** Declare PrintHelp function. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
	
	if ( argc < 6 || argv[ 1 ] == "--help" )
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
  if( (inputFileNames.size() < 2)  )
	{
		std::cout << "ERROR: You should at least specify two input images." << std::endl;
    return 1;
	}

  /** Get the outputFileName */
  std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
  if ( !retout )
  {
    std::cerr << "ERROR: no -out argument defined!" << std::endl;
    return 1;
  }

  /** read the z-spacing. */
  double zspacing = -1.0;
	bool rets = parser->GetCommandLineArgument( "-s", zspacing );

   /** Determine image properties */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 3;  
  unsigned int NumberOfComponents = 1;  
  int retgip = GetImageProperties(
    inputFileNames[0],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents);
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  
  /** Let the user overrule this */
	bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if (retpt)
  {
    std::cout << "The user has overruled this by specifying -pt:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if (NumberOfComponents > 1)
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1; 
  }

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace(ComponentType);
	
		
	try
	{
    run(TileImages, short);
		run(TileImages, float);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Return a value. */
	return 0;

} // end main


/** Define TileImages. */
template< class PixelType >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  double zspacing)
{
  	/** Define image type. */
	const unsigned int Dimension = 3;

	/** Some typedef's. */
	typedef itk::Image<PixelType, Dimension>						ImageType;
	typedef ImageType::SpacingType									  	SpacingType;
	typedef itk::ImageSeriesReader<ImageType>	  				ImageSeriesReaderType;
	typedef ImageSeriesReaderType::FileNamesContainer		FileNamesContainer;
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
		
} // end TileImages



void PrintHelp(void)
{
  std::cout << "pxtileimages tiles a stack of 2D images into a 3D image." << std::endl;
  std::cout << "This is done by employing an itk::SeriesFileReader.\n" << std::endl;

  std::cout << "Usage: \npxtileimages" << std::endl;
  std::cout << "\t-in   \tinput image filenames, at least 2." << std::endl;
  std::cout << "\t-out  \toutput image filename." << std::endl;
  std::cout << "\t[-pt] \tpixel type of input and output images; default: automatically determined from the first input image." << std::endl;
  std::cout << "\t[-s]  \tspacing in z-direction [double]; if omitted, the origins of the 2d images are used to find the spacing; if these are identical, a spacing of 1.0 is assumed." << std::endl;
  std::cout << "Supported pixel types: short, float.\n" << std::endl; 

}