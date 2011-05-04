#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkReshapeImageToImageFilter.h"
#include "itkImageFileWriter.h"
#include <itksys/SystemTools.hxx>
//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( inputFilename, outputFilename, outputSize ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare PerformPCA. */
template< class ImageType >
void Reshape(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned long> & outputSize );

/** Declare other functions. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-s", "Output size." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::string inputFilename = "";
  parser->GetCommandLineArgument( "-in", inputFilename );

  std::string base = itksys::SystemTools::GetFilenameWithoutLastExtension(
    inputFilename );
  std::string ext  = itksys::SystemTools::GetFilenameLastExtension(
    inputFilename );
  std::string outputFilename = base + "_reshaped" + ext;
  parser->GetCommandLineArgument( "-out", outputFilename );

  std::vector<unsigned long> outputSize;
  parser->GetCommandLineArgument( "-s", outputSize );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> inputSize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFilename,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    inputSize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "  Vector images are not supported." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check dimensions. */
  if ( inputSize.size() != outputSize.size() )
  {
    std::cerr << "ERROR: input and output dimension should be the same.\n";
    std::cerr << "  Please, specify only " << Dimension
      << "numbers with \"-s\"." << std::endl;
    return 1;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( Reshape, unsigned char, 2 );
    run( Reshape, char, 2 );
    run( Reshape, unsigned short, 2 );
    run( Reshape, short, 2 );
    run( Reshape, unsigned int, 2 );
    run( Reshape, int, 2 );
    run( Reshape, unsigned long, 2 );
    run( Reshape, long, 2 );
    run( Reshape, float, 2 );
    run( Reshape, double, 2 );

    /*run( Reshape, unsigned char, 3 );
    run( Reshape, char, 3 );
    run( Reshape, unsigned short, 3 );
    run( Reshape, short, 3 );
    run( Reshape, unsigned int, 3 );
    run( Reshape, int, 3 );
    run( Reshape, unsigned long, 3 );
    run( Reshape, long, 3 );
    run( Reshape, float, 3 );
    run( Reshape, double, 3 );*/
  }
  catch ( itk::ExceptionObject & e )
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

} // end main()


/**
 * ******************* Reshape *******************
 */

template< class ImageType >
void Reshape(
  const std::string & inputFilename,
  const std::string & outputFilename,
  const std::vector<unsigned long> & outputSize )
{
  /** Typedefs. */
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ReshapeImageToImageFilter< ImageType > ReshapeFilterType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef typename ReshapeFilterType::SizeType        SizeType;

  /** Translate vector to SizeType. */
  SizeType size;
  for ( unsigned int i = 0; i < outputSize.size(); ++i )
  {
    size[ i ] = outputSize[ i ];
  }


  /** Reader. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename.c_str() );

  /** Reshaper. */
  typename ReshapeFilterType::Pointer reshaper = ReshapeFilterType::New();
  reshaper->SetInput( reader->GetOutput() );
  reshaper->SetOutputSize( size );
  reshaper->Update();

  /** Writer. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFilename.c_str() );
  writer->SetInput( reshaper->GetOutput() );
  writer->Update();

} // end Reshape()


/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText = "Usage: \
  pxpca \
    -in      inputFilename \
    [-out]   outputFileName, default inputFileName_reshaped \
    -s       size of the output image \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return helpText;

} // end PrintHelp()

