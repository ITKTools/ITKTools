#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkGiplImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName, outputFileName); \
}

//-------------------------------------------------------------------------------------

/** Declare CropImage. */
template< class InputImageType >
void GiplConvert( std::string inputFileName, std::string outputFileName);

/** Declare other functions. */
void PrintHelp(void);


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 3 || argc > 9 )
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
	outputFileName += ".gipl";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

	std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

	/** Run the program. */
	try
	{
		run(GiplConvert, short, 3);
		run(GiplConvert, unsigned short, 3);
		run(GiplConvert, char, 3);
		run(GiplConvert, unsigned char, 3);
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
	 * ******************* GiplConvert *******************
	 */

template< class InputImageType >
void GiplConvert( std::string inputFileName, std::string outputFileName)
{
	/** Typedefs. */
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;
  typedef itk::GiplImageIO                            ImageIOType;

  /** Declarations */
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();
  typename ImageIOType::Pointer imageIO = ImageIOType::New();
  
	/** Read the image. 
   * We force a GiplImageIO, otherwise the GDCMImageIO is used which gives
   * problems. This is the whole reason of existence of this program. 
   * \todo Is this an ITK bug or a bug in the radiotherapy software that 
   * generates bad gipls? It seems that it is an ITK (GDCM) bug.
   */
	reader->SetFileName( inputFileName.c_str() );
  reader->SetImageIO(imageIO);
	reader->Update();

	/** Setup and process the pipeline. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( reader->GetOutput() );
  writer->Update();

} // end GiplConvert


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "This program converts gipls that cannot be converted by pxcastconvert." << std::endl;
  std::cout << "Usage:" << std::endl << "pxgiplconvert" << std::endl;
  std::cout << "\t-in\tinputFilename" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in + .mhd" << std::endl;
	std::cout << "\t[-dim]\tdimension, default 3" << std::endl;
	std::cout << "\t[-pt]\tpixelType, default short" << std::endl;
	std::cout << "Supported: 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp



