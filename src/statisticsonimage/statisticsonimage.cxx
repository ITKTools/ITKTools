
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
}

/** Declare PrintHelp, implemented at the bottom of this file. */
void PrintHelp(void);

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
  
  std::string ComponentType = "float";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 2;  
  unsigned int NumberOfComponents = 1;  
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents);
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
	try
	{
    run(StatisticsOnImage, float, 2, 1);
    run(StatisticsOnImage, float, 2, 2);
    run(StatisticsOnImage, float, 2, 3);
    run(StatisticsOnImage, float, 3, 1);
    run(StatisticsOnImage, float, 3, 2);
    run(StatisticsOnImage, float, 3, 3);
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
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "Compute statistics on an image. For vector images, the magnitude is used." << std::endl;
  std::cout << "Usage:" << std::endl << "pxstatisticonimage" << std::endl;
	std::cout << "\t-in\tInputFilename" << std::endl;
  std::cout << "\t[-out]\tOutputFileName for histogram;\n";
  std::cout << "\t      \tif omitted, no histogram is written; default: <empty>" << std::endl;
  std::cout << "\t[-mask]\tMaskFileName, mask should have the same size as the input image\n";
  std::cout << "\t       \tand be of pixeltype (convertable to) unsigned char,\n";
  std::cout << "\t       \t1 = within mask, 0 = outside mask;" << std::endl;
  std::cout << "\t[-b]\tNumberOfBins to use for histogram, default: 100;\n";
  std::cout << "\t    \tfor an accurate estimate of median and quartiles\n";
  std::cout << "\t    \tfor integer images, choose the number of bins\n";
  std::cout << "\t    \tmuch larger (~100x) than the number of gray values." << std::endl;
  std::cout << "Supported: 2D, 3D, float, (unsigned) short, (unsigned) char, 1, 2 or 3 components per pixel." << std::endl;
} // end PrintHelp
