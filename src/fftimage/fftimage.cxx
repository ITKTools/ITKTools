#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include <itksys/SystemTools.hxx>

#include "itkFFTWRealToComplexConjugateImageFilter.h"
#include "itkFFTWComplexConjugateToRealImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkComposeComplexImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
  if ( op == "forward" ) \
  { \
    FFTImage< type, dim >( inputFileNames[ 0 ], outputFileNames ); \
  } \
  else \
  { \
    IFFTImage< type, dim >( inputFileNames, outputFileNames[ 0 ], xdim ); \
  } \
}

//-------------------------------------------------------------------------------------

/** Declare FFTImage. */
template< class PixelType, unsigned int Dimension >
void FFTImage( const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames );

/** Declare IFFTImage. */
template< class PixelType, unsigned int Dimension >
void IFFTImage( const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::string & xdim );

/** Declare other functions. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 5 || argc > 14 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments: op: forward or backward */
  std::string op = "";
  bool retop = parser->GetCommandLineArgument( "-op", op );
  op = itksys::SystemTools::LowerCase( op );
  if ( op == "inverse" ) op = "backward";

  /** Get arguments: in */
  std::vector<std::string>  inputFileNames;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

  /** Get arguments: out */
  std::vector<std::string>  outputFileNames( 3 );
  std::string inputpart = "";
  if ( retin )
  {
    inputpart = inputFileNames[ 0 ].substr( 0, inputFileNames[ 0 ].rfind( "." ) );
  }
  if ( op == "forward" )
  {
  outputFileNames[ 0 ] = inputpart + "Complex.mhd";
  outputFileNames[ 0 ] = inputpart + "Real.mhd";
  outputFileNames[ 1 ] = inputpart + "Imaginary.mhd";
  }
  else if ( op == "backward" )
  {
  outputFileNames[ 0 ] = inputpart + "IFFT.mhd";
  }
  bool retout = parser->GetCommandLineArgument( "-out", outputFileNames );

  /** Get arguments: opct */
  std::string componentType = "float";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentType );

  /** Get arguments: xdim */
  std::string xdim = "even";
  bool retxdim = parser->GetCommandLineArgument( "-xdim", xdim );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( !retop )
  {
    std::cerr << "ERROR: You should specify \"-op\"." << std::endl;
    return 1;
  }

  /** Check operator. */
  if ( op != "forward" && op != "backward" )
  {
    std::cerr << "ERROR: \"-op\" should be one of {forward, backward}." << std::endl;
    return 1;
  }
  
  /** Check input. */
  if ( op == "forward" && inputFileNames.size() > 1 )
  {
    std::cerr << "ERROR: Only one input file is expected." << std::endl;
    return 1;
  }
  if ( op == "backward" && inputFileNames.size() > 2 )
  {
    std::cerr << "ERROR: Only one or two input files are expected." << std::endl;
    return 1;
  }
  if ( op == "backward" && retxdim )
  {
    if ( xdim != "odd" && xdim != "even" )
    {
      std::cerr << "ERROR: \"-xdim\" should be one of {odd, even}." << std::endl;
      return 1;
    }
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
  if ( op == "forward" && NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot take the forward Fourier transform of vector images." << std::endl;
    return 1; 
  }
  if ( op == "backward" && inputFileNames.size() == 1 && NumberOfComponents != 2 )
  {
    std::cerr << "ERROR: The NumberOfComponents is not 2!" << std::endl;
    std::cerr << "Cannot take the inverse Fourier transform of non-complex images." << std::endl;
    return 1; 
  }
  if ( op == "backward" && inputFileNames.size() == 2 && NumberOfComponents != 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is not 1!" << std::endl;
    std::cerr << "If two input images are given, they are expected to be two scalar images." << std::endl;
    std::cerr << "The first image is considered the real part, the second the imaginary part." << std::endl;
    return 1; 
  }

  /** Run the program. */
  try
  {
    run( float, 2 );
    run( double, 2 );

    run( float, 3 );
    run( double, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main()


  /**
   * ******************* FFTImage *******************
   */

template< class PixelType, unsigned int Dimension >
void FFTImage( const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames )
{
  /** Typedefs. */
  typedef itk::Image< PixelType, Dimension >        ImageType;
  typedef itk::ImageFileReader< ImageType >         ReaderType;
  typedef itk::ImageFileWriter< ImageType >         WriterType;
  typedef itk::FFTWRealToComplexConjugateImageFilter<
    PixelType, Dimension >                          FFTFilterType;
  typedef typename FFTFilterType::OutputImageType   ComplexImageType;
  typedef itk::ImageFileWriter< ComplexImageType >  ComplexWriterType;
  typedef itk::ComplexToRealImageFilter<
    ComplexImageType, ImageType >                   RealFilterType;
  typedef itk::ComplexToImaginaryImageFilter<
    ComplexImageType, ImageType >                   ImaginaryFilterType;
  
  /** Read the image as float or double. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Compute the FFT of the image. */
  typename FFTFilterType::Pointer fftFilter = FFTFilterType::New();
  fftFilter->SetInput( reader->GetOutput() );
  fftFilter->Update();

  /** Write the output image(s).
   * If 1 output image is given: write the complex image.
   * If 2 output images are given: write the real and imaginary images.
   * If 3 output images are given: write the complex, real and imaginary images.
   */
  if ( outputFileNames.size() == 1 || outputFileNames.size() == 3 )
  {
    typename ComplexWriterType::Pointer complexWriter = ComplexWriterType::New();
    complexWriter->SetFileName( outputFileNames[ 0 ].c_str() );
    complexWriter->SetInput( fftFilter->GetOutput() );
    complexWriter->Update();
  }
  if ( outputFileNames.size() > 1 )
  {
    typename RealFilterType::Pointer realFilter = RealFilterType::New();
    realFilter->SetInput( fftFilter->GetOutput() );

    typename ImaginaryFilterType::Pointer imaginaryFilter = ImaginaryFilterType::New();
    imaginaryFilter->SetInput( fftFilter->GetOutput() );

    typename WriterType::Pointer writer1 = WriterType::New();
    typename WriterType::Pointer writer2 = WriterType::New();

    if ( outputFileNames.size() == 2 )
    {
      writer1->SetFileName( outputFileNames[ 0 ].c_str() );
      writer2->SetFileName( outputFileNames[ 1 ].c_str() );
    }
    else
    {
      writer1->SetFileName( outputFileNames[ 1 ].c_str() );
      writer2->SetFileName( outputFileNames[ 2 ].c_str() );
    }
    writer1->SetInput( realFilter->GetOutput() );
    writer1->SetFileName( outputFileNames[ 1 ].c_str() );
    writer1->Update();

    writer2->SetInput( imaginaryFilter->GetOutput() );
    writer2->SetFileName( outputFileNames[ 2 ].c_str() );
    writer2->Update();
  }
  
} // end FFTImage()


  /**
   * ******************* IFFTImage *******************
   */

template< class PixelType, unsigned int Dimension >
void IFFTImage( const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName, const std::string & xdim )
{
  /** Typedefs. */
  typedef itk::Image< PixelType, Dimension >            ImageType;
  typedef itk::FFTWComplexConjugateToRealImageFilter<
    PixelType, Dimension >                              IFFTFilterType;
  typedef typename IFFTFilterType::TInputImageType      ComplexImageType;
  typedef itk::ImageFileReader< ImageType >             ReaderType;
  typedef itk::ImageFileReader< ComplexImageType >      ComplexReaderType;
  typedef itk::ComposeComplexImageFilter< ImageType >   ComposeComplexImageFilterType;
  typedef itk::ImageFileWriter< ImageType >             WriterType;
  
  /** The IFFT of the image. */
  typename IFFTFilterType::Pointer ifftFilter = IFFTFilterType::New();

  /** Read one complex image, or two scalar images, which need to
   * be combined into one complex image. */
  typename ComplexReaderType::Pointer complexReader
    = ComplexReaderType::New();
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = ReaderType::New();
  typename ComposeComplexImageFilterType::Pointer composer
    = ComposeComplexImageFilterType::New();
  if ( inputFileNames.size() == 1 )
  {
    complexReader->SetFileName( inputFileNames[ 0 ].c_str() );
    ifftFilter->SetInput( complexReader->GetOutput() );
  }
  else if  ( inputFileNames.size() == 2 )
  {
    reader1->SetFileName( inputFileNames[ 0 ].c_str() );
    reader2->SetFileName( inputFileNames[ 1 ].c_str() );
    composer->SetInput( 0, reader1->GetOutput() );
    composer->SetInput( 1, reader2->GetOutput() );
    ifftFilter->SetInput( composer->GetOutput() );
  }

  /** Setup IFFT filter. */
  if ( xdim == "odd" )
  {
    ifftFilter->SetActualXDimensionIsOdd( true );
  }
  else
  {
    ifftFilter->SetActualXDimensionIsOdd( false );
  }

  /** Write the output image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( ifftFilter->GetOutput() );
  writer->Update();
  
} // end IFFTImage()


  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp( void )
{
  std::cout << "Usage:" << std::endl << "pxfftimage" << std::endl;
  std::cout << "  -in      inputFilenames\n";
  std::cout << "             forward: only one input\n";
  std::cout << "             backward, # given:\n";
  std::cout << "               1: a complex image\n";
  std::cout << "               2: a real and imaginary part" << std::endl;
  std::cout << "  -op      operator, {forward, backward} FFT" << std::endl;
  std::cout << "  [-out]   outputFilenames\n";
  std::cout << "             forward, # given:\n";
  std::cout << "               1: write the complex image, default in + Complex.mhd\n";
  std::cout << "               2: write the real and imaginary images, default in + Real.mhd and in + Imaginary.mhd\n";
  std::cout << "               3: write the complex, real and imaginary images\n";
  std::cout << "             backward: only one output, default in + IFFT" << std::endl;
  std::cout << "  [-opct]  the output type\n";
  std::cout << "             choose from {float, double}, default float" << std::endl;
  std::cout << "  [-xdim]  the backward transform needs to know if the actual x-dimension was odd or even.\n";
  std::cout << "             choose from {odd, even}, default even" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;

} // end PrintHelp()
