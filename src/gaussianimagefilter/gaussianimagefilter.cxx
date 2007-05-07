#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim >  OutputImageType; \
    function< OutputImageType >( inputFileName, outputFileName, sigma, order ); \
}

//-------------------------------------------------------------------------------------

/** Declare GaussianImageFilter. */
template< class OutputImageType >
void GaussianImageFilter( const std::string & inputFileName,
  const std::string & outputFileName, const float & sigma, const unsigned int & order );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
 	/** Check arguments for help. */
	if ( argc < 3 || argc > 11 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
  std::string inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  double sigma = 1.0;
  bool retstd = parser->GetCommandLineArgument( "-std", sigma );

  unsigned int order = 0;
  bool retord = parser->GetCommandLineArgument( "-ord", order );

  std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "BLURRED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::string componentType = "";
	bool retpt = parser->GetCommandLineArgument( "-pt", componentType );

 	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  /** Check options. */
  if ( order > 2 )
  {
    std::cerr << "ERROR: The order should not be higher than 2." << std::endl;
    std::cerr << "Only zeroth, first and second order derivatives are supported." << std::endl;
		return 1;
  }
 
  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  if ( !retpt ) componentType = ComponentTypeIn;

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1; 
  }

	/** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Run the program. */
	try
	{
    run( GaussianImageFilter, char, 2 );
		run( GaussianImageFilter, unsigned char, 2 );
    run( GaussianImageFilter, short, 2 );
		run( GaussianImageFilter, unsigned short, 2 );
    run( GaussianImageFilter, int, 2 );
		run( GaussianImageFilter, unsigned int, 2 );
    run( GaussianImageFilter, long, 2 );
		run( GaussianImageFilter, unsigned long, 2 );
    run( GaussianImageFilter, float, 2 );
		run( GaussianImageFilter, double, 2 );

    run( GaussianImageFilter, char, 3 );
		run( GaussianImageFilter, unsigned char, 3 );
    run( GaussianImageFilter, short, 3 );
		run( GaussianImageFilter, unsigned short, 3 );
    run( GaussianImageFilter, int, 3 );
		run( GaussianImageFilter, unsigned int, 3 );
    run( GaussianImageFilter, long, 3 );
		run( GaussianImageFilter, unsigned long, 3 );
    run( GaussianImageFilter, float, 3 );
		run( GaussianImageFilter, double, 3 );

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
	 * ******************* GaussianImageFilter *******************
	 */

template< class OutputImageType >
void GaussianImageFilter( const std::string & inputFileName,
  const std::string & outputFileName,
  const float & sigma,
  const unsigned int & order )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, OutputImageType >                     FilterType;
	typedef itk::ImageFileWriter< OutputImageType >			    WriterType;

	/**	Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup the smoothing filter. */
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNormalizeAcrossScale( false );
  filter->SetInput( reader->GetOutput() );
  filter->SetSigma( sigma );
  filter->SetOrder( order );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName );
	writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end GaussianImageFilter()


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxgaussianimagefilter" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
	std::cout << "  [-out]   outputFilename, default in + BLURRED.mhd" << std::endl;
  std::cout << "  [-std]   sigma, default 1.0" << std::endl;
  std::cout << "  [-ord]   order, default zero" << std::endl;
  std::cout << "             0: zero order = blurring" << std::endl;
  std::cout << "             1: first order = gradient" << std::endl;
  std::cout << "             2: second order derivative" << std::endl;
  std::cout << "  [-pt]    output pixel type, default equal to input" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;
} // end PrintHelp()

