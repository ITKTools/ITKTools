#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim >       InputImageType; \
  typedef itk::VectorImage< type, dim > OutputImageType; \
  function< InputImageType, OutputImageType >( inputFileNames, outputFileName ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ComposeVectorImage. */
template< class InputImageType, class OutputImageType >
void ComposeVectorImage(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Check arguments for help. */
  if ( argc < 4 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::vector<std::string>  inputFileNames( 0, "" );
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileName = "VECTOR.mhd";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two (2) input files." << std::endl;
    return 1;
  }
 
  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileNames[ 0 ],
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
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1; 
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( ComposeVectorImage, char, 2 );
    run( ComposeVectorImage, unsigned char, 2 );
    run( ComposeVectorImage, short, 2 );
    run( ComposeVectorImage, unsigned short, 2 );
    run( ComposeVectorImage, int, 2 );
    run( ComposeVectorImage, unsigned int, 2 );
    run( ComposeVectorImage, long, 2 );
    run( ComposeVectorImage, unsigned long, 2 );
    run( ComposeVectorImage, float, 2 );
    run( ComposeVectorImage, double, 2 );

    run( ComposeVectorImage, char, 3 );
    run( ComposeVectorImage, unsigned char, 3 );
    run( ComposeVectorImage, short, 3 );
    run( ComposeVectorImage, unsigned short, 3 );
    run( ComposeVectorImage, int, 3 );
    run( ComposeVectorImage, unsigned int, 3 );
    run( ComposeVectorImage, long, 3 );
    run( ComposeVectorImage, unsigned long, 3 );
    run( ComposeVectorImage, float, 3 );
    run( ComposeVectorImage, double, 3 );
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
      << "pixel (component) type = " << ComponentTypeIn
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main


  /**
   * ******************* ComposeVectorImage *******************
   */

template< class InputImageType, class OutputImageType >
void ComposeVectorImage( const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName )
{
  /** Typedef's. */
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::ImageToVectorImageFilter< InputImageType > FilterType;;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input images. */
  std::vector<typename ReaderType::Pointer> readers( inputFileNames.size() );
  for ( unsigned int i = 0; i < inputFileNames.size(); ++i )
  {
    readers[ i ] = ReaderType::New();
    readers[ i ]->SetFileName( inputFileNames[ i ] );
    readers[ i ]->Update();
  }

  /** Create index extractor and writer. */
  typename FilterType::Pointer composer = FilterType::New();
  for ( unsigned int i = 0; i < inputFileNames.size(); ++i )
  {
    composer->SetNthInput( i, readers[ i ]->GetOutput() );
  }

  /** Write vector image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( composer->GetOutput() );
  writer->Update();

} // end ComposeVectorImage()


  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp()
{
  std::cout << "Usage:" << std::endl << "pximagetovectorimage" << std::endl;
  std::cout << "  -in      inputFilenames, at least 2" << std::endl;
  std::cout << "  [-out]   outputFilename, default VECTOR.mhd" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;
  std::cout << "Note: make sure that the input images are of the same type, size, etc." << std::endl;
} // end PrintHelp()

