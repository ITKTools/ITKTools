#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkChangeLabelImageFilter.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileName, outputFileName, inValues, outValues ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare IntensityReplaceImageFilter. */
template< class TOutputPixel, unsigned int NDimension >
void IntensityReplaceImageFilter( const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<std::string> & inValues,
  const std::vector<std::string> & outValues );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
 	/** Check arguments for help. */
	if ( argc < 7 )
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

  /** Read as vector of strings, since we don't know yet if it will be
   * integers or floats */
  std::vector< std::string > inValues;
	bool reti = parser->GetCommandLineArgument( "-i", inValues );
  std::vector< std::string > outValues;
	bool reto = parser->GetCommandLineArgument( "-o", outValues );

  std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "LUTAPPLIED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::string componentType = "";
	bool retpt = parser->GetCommandLineArgument( "-pt", componentType );

 	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if ( !reti )
	{
		std::cerr << "ERROR: You should specify \"-i\"." << std::endl;
		return 1;
	}
  if ( !reto )
	{
		std::cerr << "ERROR: You should specify \"-o\"." << std::endl;
		return 1;
	}
  if ( inValues.size() != outValues.size() )
  {
    std::cerr << "ERROR: \"-i\" and \"-o\" should be followed by an equal number of values!" << std::endl;
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
    std::cerr << "ERROR: error while getting image properties of the input image!" << std::endl;
    return 1;
  }

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.   */
  if ( !retpt ) componentType = ComponentTypeIn;

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1; 
  }

	/** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( componentType );


  /** Run the program. */
  bool supported = false;
	try
	{
    run( IntensityReplaceImageFilter, char, 2 );
		run( IntensityReplaceImageFilter, unsigned char, 2 );
    run( IntensityReplaceImageFilter, short, 2 );
		run( IntensityReplaceImageFilter, unsigned short, 2 );
    run( IntensityReplaceImageFilter, int, 2 );
		run( IntensityReplaceImageFilter, unsigned int, 2 );
    run( IntensityReplaceImageFilter, long, 2 );
		run( IntensityReplaceImageFilter, unsigned long, 2 );
    run( IntensityReplaceImageFilter, float, 2 );
		run( IntensityReplaceImageFilter, double, 2 );

    run( IntensityReplaceImageFilter, char, 3 );
		run( IntensityReplaceImageFilter, unsigned char, 3 );
    run( IntensityReplaceImageFilter, short, 3 );
		run( IntensityReplaceImageFilter, unsigned short, 3 );
    run( IntensityReplaceImageFilter, int, 3 );
		run( IntensityReplaceImageFilter, unsigned int, 3 );
    run( IntensityReplaceImageFilter, long, 3 );
		run( IntensityReplaceImageFilter, unsigned long, 3 );
    run( IntensityReplaceImageFilter, float, 3 );
		run( IntensityReplaceImageFilter, double, 3 );

	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr <<
      "pixel (component) type = " << componentType <<
      " ; dimension = " << Dimension 
      << std::endl;
    return 1;
  }
 	
	/** End program. */
	return 0;

} // end main


/**
 * ******************* IntensityReplaceImageFilter *******************
 */

template< class TOutputPixel, unsigned int NDimension >
void IntensityReplaceImageFilter( const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<std::string> & inValues,
  const std::vector<std::string> & outValues )
{
  /** Typedefs. */
  typedef TOutputPixel                                    OutputPixelType;
  const unsigned int Dimension = NDimension;

  typedef OutputPixelType                                 InputPixelType;
  
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >        OutputImageType;

	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
	typedef itk::ChangeLabelImageFilter<
    InputImageType, OutputImageType >                     ReplaceFilterType;
  typedef itk::ImageFileWriter< OutputImageType >			    WriterType;

	/**	Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename ReplaceFilterType::Pointer replaceFilter = ReplaceFilterType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Set up reader */
  reader->SetFileName( inputFileName );
  
  /** Setup the the input and the 'change map' of the replace filter. */
  replaceFilter->SetInput( reader->GetOutput() );
  if ( itk::NumericTraits<OutputPixelType>::is_integer )
  {
    for (unsigned int i = 0; i < inValues.size(); ++i)
    {
      const InputPixelType inval = static_cast< InputPixelType >(
        atoi( inValues[i].c_str() )   );
      const OutputPixelType outval = static_cast< OutputPixelType >(
        atoi( outValues[i].c_str() )   );
      replaceFilter->SetChange( inval, outval );
    }
  }
  else
  {
    for (unsigned int i = 0; i < inValues.size(); ++i)
    {
      const InputPixelType inval = static_cast< InputPixelType >(
        atof( inValues[i].c_str() )   );
      const OutputPixelType outval = static_cast< OutputPixelType >(
        atof( outValues[i].c_str() )   );
      replaceFilter->SetChange( inval, outval );
    }
  }
    
  /** Set up writer. */
	writer->SetFileName( outputFileName );
	writer->SetInput( replaceFilter->GetOutput() );
  writer->Update();

} // end IntensityReplaceImageFilter()


/**
 * ******************* PrintHelp *******************
 */
void PrintHelp()
{
  std::cout << "This program replaces some user specified intensity values in an image.\n";
	std::cout << "Usage:\n"
            << "pxintensityreplace\n";
  std::cout << "  -in      inputFilename\n";
	std::cout << "  [-out]   outputFilename, default in + LUTAPPLIED.mhd\n";
  std::cout << "  -i       input pixel values that should be replaced\n";
  std::cout << "  -o       output pixel values that replace the corresponding input values\n";
  std::cout << "  [-pt]    output pixel type, default equal to input\n";
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int,\n"
            << "(unsigned) long, float, double.\n";
  std::cout << "If \"-pt\" is used, the input is immediately converted to that particular\n"
            << "type, after which the intensity replacement is performed.\n";
  std::cout << std::endl;
} // end PrintHelp()

