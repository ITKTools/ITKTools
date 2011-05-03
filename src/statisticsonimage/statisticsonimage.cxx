
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
    numberOfBins, select ); \
  supported = true; \
}

/** Declare PrintHelp, implemented at the bottom of this file. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------


int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
  }
  
  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string histogramOutputFileName = "";
  parser->GetCommandLineArgument( "-out", histogramOutputFileName );

  unsigned int numberOfBins = 100;
  parser->GetCommandLineArgument( "-b", numberOfBins );

  std::string select = "";
  bool rets = parser->GetCommandLineArgument( "-s", select );

  /** Check selection. */
  if ( rets && ( select != "arithmetic" && select != "geometric"
    && select != "histogram" ) )
  {
    std::cerr << "ERROR: -s should be one of {arithmetic, geometric, histogram}"
      << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string PixelType; //we don't use this
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
    run( StatisticsOnImage, float, 4, 1 );
    run( StatisticsOnImage, float, 4, 4 );
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

std::string PrintHelp( void )
{
  std::string helpText = "Compute statistics on an image. For vector images, the magnitude is used. \
  Usage: \
  pxstatisticsonimage \
    -in      inputFilename \
    [-out]   outputFileName for histogram;\n \
             if omitted, no histogram is written; default: <empty> \
    [-mask]  MaskFileName, mask should have the same size as the input image\n \
             and be of pixeltype (convertable to) unsigned char,\n \
             1 = within mask, 0 = outside mask; \
    [-b]     NumberOfBins to use for histogram, default: 100;\n \
             for an accurate estimate of median and quartiles\n \
             for integer images, choose the number of bins\n \
             much larger (~100x) than the number of gray values. \
             if equal 0, then the intensity range (max - min) is chosen. \
    [-s]     select which to compute {arithmetic, geometric, histogram}, default all;\n \
  Supported: 2D, 3D, 4D, float, (unsigned) short, (unsigned) char, 1, 2 or 3 components per pixel.\n \
	For 4D, only 1 or 4 components per pixel are supported.";

  return helpText;

} // end PrintHelp()
