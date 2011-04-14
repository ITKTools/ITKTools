#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkGiplImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkArray.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CropImage. */
template< class InputImageType >
void GiplConvert(
  const std::string & inputFileName,
  const std::string & outputFileName );

/** Declare other functions. */
void PrintHelp( void );


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
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += ".gipl";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  /** Dummy image type. */
  const unsigned int DummyDimension = 3;
  typedef short      DummyPixelType;
  typedef itk::Image< DummyPixelType, DummyDimension >   DummyImageType;

  /** Test reader */
  typedef itk::ImageFileReader< DummyImageType >     ReaderType;
  typedef itk::GiplImageIO                            ImageIOType;

  typedef itk::Array<unsigned int> SizeArrayType;

  /** Create a test imageIO object */
  ImageIOType::Pointer testImageIO = ImageIOType::New();

  /** Create a testReader. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( inputFileName.c_str() );
  testReader->SetImageIO(testImageIO);

  /** Determine image properties */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  SizeArrayType imageSize;

  /** Generate all information. */
  try
  {
    testReader->GenerateOutputInformation();
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** Get the component type, number of components, dimension and pixel type. */
  Dimension = testImageIO->GetNumberOfDimensions();
  NumberOfComponents = testImageIO->GetNumberOfComponents();
  ComponentType = testImageIO->GetComponentTypeAsString( testImageIO->GetComponentType() );
  ReplaceUnderscoreWithSpace( ComponentType );
  PixelType = testImageIO->GetPixelTypeAsString( testImageIO->GetPixelType() );
  imageSize.SetSize(Dimension);
  for (unsigned int i = 0; i < Dimension; ++i)
  {
    imageSize[i] = testImageIO->GetDimensions(i);
  }

  /** Check inputPixelType. */
  if ( ComponentType != "unsigned char"
    && ComponentType != "char"
    && ComponentType != "unsigned short"
    && ComponentType != "short"
    && ComponentType != "unsigned int"
    && ComponentType != "int"
    && ComponentType != "unsigned long"
    && ComponentType != "long"
    && ComponentType != "float"
    && ComponentType != "double" )
  {
    /** In this case an illegal pixeltype  is found. */
    std::cerr
      << "ERROR while determining image properties!"
      << "The found componenttype is \""
      << ComponentType
      << "\", which is not supported."
      << std::endl;
    return 1;
  }

  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  std::cout << "\tSize                " << imageSize << std::endl;

  /** Let the user overrule this */
  bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if ( retdim | retpt )
  {
    std::cout << "The user has overruled this by specifying -pt and/or -dim:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( GiplConvert, short, 3 );
    run( GiplConvert, unsigned short, 3 );
    run( GiplConvert, char, 3 );
    run( GiplConvert, unsigned char, 3 );
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
   * ******************* GiplConvert *******************
   */

template< class InputImageType >
void GiplConvert(
  const std::string & inputFileName,
  const std::string & outputFileName )
{
  /** Typedefs. */
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< InputImageType >      WriterType;
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
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-out]   outputFilename, default in + .mhd" << std::endl;
  std::cout << "  [-dim]   dimension, default 3" << std::endl;
  std::cout << "  [-pt]    pixelType, default short" << std::endl;
  std::cout << "Supported: 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp



