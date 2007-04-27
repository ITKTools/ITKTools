#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim >       InputImageType; \
    function< InputImageType >( inputFileName, outputFileName ); \
}

//-------------------------------------------------------------------------------------

/** Declare BinaryThinning. */
template< class InputImageType >
void BinaryThinning( const std::string & inputFileName,
  const std::string & outputFileName );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
 	/** Check arguments for help. */
	if ( argc < 3 || argc > 5 )
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

  std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "THINNED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

 	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
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

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported. Thinning only works on binary images." << std::endl;
    return 1; 
  }

	/** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Run the program. */
	try
	{
    run( BinaryThinning, char, 2 );
		run( BinaryThinning, unsigned char, 2 );
    run( BinaryThinning, short, 2 );
		run( BinaryThinning, unsigned short, 2 );
    run( BinaryThinning, int, 2 );
		run( BinaryThinning, unsigned int, 2 );
    run( BinaryThinning, long, 2 );
		run( BinaryThinning, unsigned long, 2 );
    run( BinaryThinning, float, 2 );
		run( BinaryThinning, double, 2 );

    run( BinaryThinning, char, 3 );
		run( BinaryThinning, unsigned char, 3 );
    run( BinaryThinning, short, 3 );
		run( BinaryThinning, unsigned short, 3 );
    run( BinaryThinning, int, 3 );
		run( BinaryThinning, unsigned int, 3 );
    run( BinaryThinning, long, 3 );
		run( BinaryThinning, unsigned long, 3 );
    run( BinaryThinning, float, 3 );
		run( BinaryThinning, double, 3 );
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
	 * ******************* BinaryThinning *******************
	 */

template< class InputImageType >
void BinaryThinning( const std::string & inputFileName,
  const std::string & outputFileName )
{
  /** Typedef's. */
	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
  typedef itk::BinaryThinningImageFilter<
    InputImageType, InputImageType >                      FilterType;
	typedef itk::ImageFileWriter< InputImageType >			    WriterType;

	/**	Read in the input images. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Thin the image. */
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName );
	writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end BinaryThinning()


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxbinarythinning" << std::endl;
	std::cout << "  -in     inputFilename" << std::endl;
	std::cout << "  [-out]  outputFilename, default in + THINNED.mhd" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;
  std::cout << "Note that the thinning algorithm used here is really a 2D thinning algortihm." << std::endl;
  std::cout << "In 3D the thinning is performed slice by slice." << std::endl;

} // end PrintHelp()

