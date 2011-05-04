#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::VectorImage< type, dim >       InputImageType; \
  typedef itk::VectorImage< type, dim > OutputImageType; \
  function< InputImageType, OutputImageType >( inputFileNames, outputFileName, numberOfStreams ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ComposeVectorImage. */
template< class InputImageType, class OutputImageType >
void ComposeVectorImage(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const unsigned int & numberOfStreams );

/** Declare PrintHelp. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintHelp());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

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
  std::vector<std::string>  inputFileNames( 0, "" );
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileName = "VECTOR.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Support for streaming. */
  unsigned int numberOfStreams = 1;
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Check if the required arguments are given. */
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
void ComposeVectorImage(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const unsigned int & numberOfStreams )
{
  /** Typedef's. */
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::Image<typename InputImageType::InternalPixelType, InputImageType::ImageDimension> ScalarImageType;
  typedef itk::ImageToVectorImageFilter< ScalarImageType > ImageToVectorImageFilterType;
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
  typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
  // For each input image
  std::cout << "There are " << inputFileNames.size() << " input images." << std::endl;
  unsigned int currentOutputIndex = 0;
  for ( unsigned int inputImageIndex = 0; inputImageIndex < inputFileNames.size(); ++inputImageIndex )
  {
    typedef itk::VectorIndexSelectionCastImageFilter<InputImageType, ScalarImageType> ComponentExtractionType;

    // For each component of the current image
    std::cout << "There are " << readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel() << " components in image " 
              << inputImageIndex << std::endl;
    for ( unsigned int component = 0; component < readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel(); ++component )
    {
      typename ComponentExtractionType::Pointer componentExtractionFilter = ComponentExtractionType::New();
      componentExtractionFilter->SetIndex(component);
      componentExtractionFilter->SetInput(readers[inputImageIndex]->GetOutput());
      componentExtractionFilter->Update();
      
      imageToVectorImageFilter->SetNthInput( currentOutputIndex, componentExtractionFilter->GetOutput());
      currentOutputIndex++;
    }
  }
  
  imageToVectorImageFilter->Update();
  
  std::cout << "Output image has " << imageToVectorImageFilter->GetOutput()->GetNumberOfComponentsPerPixel() << " components." << std::endl;

  /** Write vector image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( imageToVectorImageFilter->GetOutput() );
  writer->SetNumberOfStreamDivisions( numberOfStreams );
  writer->Update();

} // end ComposeVectorImage()


/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText = "Usage: \
  pximagetovectorimage\n \
    -in      inputFilenames, at least 2\n \
    [-out]   outputFilename, default VECTOR.mhd\n \
    [-s]     number of streams, default 1.\n \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, \
  (unsigned) int, (unsigned) long, float, double.\n \
  Note: make sure that the input images are of the same type, size, etc.";

  return helpText;

} // end PrintHelp()

