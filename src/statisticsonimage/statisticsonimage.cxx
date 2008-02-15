
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "statisticsonimage.h"

/** This program determines the minimum, maximum, 
 * mean, sigma, variance, and sum of an image, or its magnitude/jacobian.
 */

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim,nrofcomp) \
if ( ComponentType == #type && Dimension == dim && NumberOfComponents == nrofcomp) \
{ \
  function< type, dim, nrofcomp >( inputFileName, maskFileName, histogramOutputFileName,\
    numberOfBins ); \
  supported = true; \
}

/** Declare PrintHelp, implemented at the bottom of this file. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------


int main( int argc, char ** argv )
{
	
	if ( argc < 3 )
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
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  std::string	maskFileName = "";
	bool retmask = parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string	histogramOutputFileName = "";
	bool rethist = parser->GetCommandLineArgument( "-out", histogramOutputFileName );

  unsigned int numberOfBins = 100;
  bool retb = parser->GetCommandLineArgument( "-b", numberOfBins );
  
  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tComponentType:      " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** force images to sneaky be converted to doubles */
	ComponentType = "float";
		
	/** Run the program. */
  bool supported = false;
	try
	{
    run( StatisticsOnImage, float, 2, 1 );
    run( StatisticsOnImage, float, 2, 2 );
    run( StatisticsOnImage, float, 2, 3 );
    run( StatisticsOnImage, float, 3, 1 );
    run( StatisticsOnImage, float, 3, 2 );
    run( StatisticsOnImage, float, 3, 3 );
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
      << "pixel (component) type = " << ComponentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main






	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "Compute statistics on an image. For vector images, the magnitude is used." << std::endl;
  std::cout << "Usage:" << std::endl << "pxstatisticonimage" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-out]   outputFileName for histogram;\n";
  std::cout << "           if omitted, no histogram is written; default: <empty>" << std::endl;
  std::cout << "  [-mask]  MaskFileName, mask should have the same size as the input image\n";
  std::cout << "           and be of pixeltype (convertable to) unsigned char,\n";
  std::cout << "           1 = within mask, 0 = outside mask;" << std::endl;
  std::cout << "  [-b]     NumberOfBins to use for histogram, default: 100;\n";
  std::cout << "           for an accurate estimate of median and quartiles\n";
  std::cout << "           for integer images, choose the number of bins\n";
  std::cout << "           much larger (~100x) than the number of gray values." << std::endl;
  std::cout << "Supported: 2D, 3D, float, (unsigned) short, (unsigned) char, 1, 2 or 3 components per pixel." << std::endl;
} // end PrintHelp
