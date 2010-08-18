#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkAbsImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkNotImageFilter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, sigma, threshold, radius ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare DetectGoldMarkers. */
template< class InputImageType >
void DetectGoldMarkers(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & sigma,
  const double & threshold,
  const unsigned int & radius );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 4 || argc > 13 )
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
  outputFileName += "MASK.mhd";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  double sigma = 4.0;
  bool rets = parser->GetCommandLineArgument( "-s", sigma );

  double threshold = 0.9999;
  bool rett = parser->GetCommandLineArgument( "-t", threshold );

  unsigned int radius = 7;
  bool retradius = parser->GetCommandLineArgument( "-r", radius );

  unsigned int Dimension = 3;
  bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in PixelType. */
  ReplaceUnderscoreWithSpace(PixelType);

  /** Run the program. */
  bool supported = false;
  try
  {
    run( DetectGoldMarkers, short, 3 );
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
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


  /**
   * ******************* DetectGoldMarkers *******************
   *
   * The resize function templated over the input pixel type.
   */

template< class InputImageType >
void DetectGoldMarkers(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & sigma,
  const double & threshold,
  const unsigned int & radius )
{

  /** constants */
  const unsigned int Dimension = InputImageType::ImageDimension;
  const unsigned int NumberOfBins = static_cast<unsigned int>(
    vcl_ceil( 1.0 / (1.0 - threshold ) )   );
  const double SmallSigma = 1.0;

  /** TYPEDEF's. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef char                                        OutputPixelType;
  typedef float                                       InternalPixelType;

  typedef itk::Image<OutputPixelType, Dimension>      OutputImageType;
  typedef itk::Image<InternalPixelType, Dimension>    InternalImageType;
  typedef typename InputImageType::SpacingType        SpacingType;

  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::CastImageFilter<
    InputImageType, InternalImageType>                InputCasterType;
  typedef itk::LaplacianRecursiveGaussianImageFilter<
    InternalImageType, InternalImageType>             LaplacianerType;
  typedef itk::SmoothingRecursiveGaussianImageFilter<
    InternalImageType, InternalImageType>             SmootherType;
  typedef itk::SubtractImageFilter<InternalImageType,
    InternalImageType, InternalImageType>             SubtracterType;
  typedef itk::AbsImageFilter<
    InternalImageType, InternalImageType>             AbserType;
  typedef itk::MultiplyImageFilter<InternalImageType,
    InternalImageType, InternalImageType>             MultiplierType;
  typedef
    itk::Statistics::ScalarImageToHistogramGenerator<
    InternalImageType>                                HistogramGeneratorType;
  typedef itk::BinaryThresholdImageFilter<
    InternalImageType, OutputImageType>               ThresholderType;
  typedef itk::BinaryBallStructuringElement<
    OutputPixelType, Dimension>                       StructuringElementType;
  typedef itk::BinaryDilateImageFilter<
    OutputImageType, OutputImageType,
    StructuringElementType>                           DilaterType;
  typedef itk::NotImageFilter<
    OutputImageType, OutputImageType>                 NoterType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** DECLARATION'S. */
  SpacingType backupSpacing;
  SpacingType hackSpacing;
  InternalPixelType thresholdvalue;
  typename InputImageType::Pointer inputImage;
  typename OutputImageType::Pointer outputImage;
  typename ReaderType::Pointer reader = ReaderType::New();
  typename InputCasterType::Pointer caster = InputCasterType::New();
  typename LaplacianerType::Pointer laplacianer = LaplacianerType::New();
  typename SmootherType::Pointer smoother1 = SmootherType::New();
  typename SmootherType::Pointer smoother2 = SmootherType::New();
  typename SubtracterType::Pointer subtracter = SubtracterType::New();
  typename AbserType::Pointer abser = AbserType::New();
  typename MultiplierType::Pointer multiplier = MultiplierType::New();
  typename HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  StructuringElementType structuringElement;
  typename DilaterType::Pointer dilater = DilaterType::New();
  typename NoterType::Pointer noter = NoterType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */

  reader->SetFileName( inputFileName.c_str() );

  std::cout << "Reading input image..." << std::endl;
  reader->Update();
  std::cout << "Input image read." << std::endl;

  inputImage = reader->GetOutput();

  /** Make sure changes are not undone */
  inputImage->DisconnectPipeline();

  /** Change the spacing temporary to 1.0 */
  backupSpacing = inputImage->GetSpacing();
  hackSpacing.Fill( 1.0 );
  inputImage->SetSpacing( hackSpacing );

  /** Set up pipeline until thresholding */

  caster->SetInput( inputImage );

  laplacianer->SetSigma( sigma );
  laplacianer->SetInput( caster->GetOutput() );
  std::cout << "Computing Laplacian..." << std::endl;
  laplacianer->Update();
  std::cout << "Laplacian computed." << std::endl;

  smoother1->SetSigma( SmallSigma );
  smoother1->SetInput( caster->GetOutput() );
  subtracter->SetInput1( caster->GetOutput() );
  subtracter->SetInput2( smoother1->GetOutput() );
  std::cout << "Applying HighPass filter on input image..." << std::endl;
  subtracter->Update();
  std::cout << "Done." << std::endl;

  abser->SetInput( subtracter->GetOutput() );
  smoother2->SetSigma( sigma );
  smoother2->SetInput( abser->GetOutput() );
  std::cout << "Smoothing abs(HighPass)..." << std::endl;
  smoother2->Update();
  std::cout << "Smoothing ready." << std::endl;

  multiplier->SetInput1( laplacianer->GetOutput() );
  multiplier->SetInput2( smoother2->GetOutput() );
  std::cout << "Multiplying Laplacian with smoothed abs(HighPass)..." << std::endl;
  multiplier->Update();
  std::cout << "Multiplying done." << std::endl;

  /** Make a histogram to determine the threshold */
  histogramGenerator->SetInput( multiplier->GetOutput() );
  histogramGenerator->SetNumberOfBins( NumberOfBins );
  std::cout << "Determining histogram..." << std::endl;
  histogramGenerator->Compute();
  thresholdvalue = histogramGenerator->GetOutput()->Quantile( 0, threshold );
  std::cout
    << "Histogram has been computed. The most recent result will be thresholded at: "
    << thresholdvalue
    << " ." << std::endl;

  /** resume the pipeline */

  thresholder->SetLowerThreshold(
    itk::NumericTraits<InternalPixelType>::NonpositiveMin() );
  thresholder->SetUpperThreshold( thresholdvalue );
  thresholder->SetInsideValue( itk::NumericTraits<OutputPixelType>::Zero );
  thresholder->SetOutsideValue( itk::NumericTraits<OutputPixelType>::One );
  thresholder->SetInput( multiplier->GetOutput() );
  std::cout << "Applying threshold..." << std::endl;
  thresholder->Update();
  std::cout << "Threshold applied." << std::endl;

  structuringElement.SetRadius( radius );
  structuringElement.CreateStructuringElement();
  dilater->SetForegroundValue( itk::NumericTraits<OutputPixelType>::One  );
  dilater->SetBackgroundValue( itk::NumericTraits<OutputPixelType>::Zero );
  dilater->SetKernel( structuringElement );
  dilater->SetInput( thresholder->GetOutput() );
  std::cout << "Dilating..." << std::endl;
  dilater->Update();
  std::cout << "Dilating done." << std::endl;

  noter->SetInput( dilater->GetOutput() );
  std::cout << "Replacing 0 with 1 and vice versa..." << std::endl;
  noter->Update();
  std::cout << "Done." << std::endl;

  /** restore the correct spacing */
  outputImage = noter->GetOutput();
  outputImage->DisconnectPipeline();
  outputImage->SetSpacing( backupSpacing );

  /** Write the output image. */
  writer->SetInput( outputImage );
  writer->SetFileName( outputFileName.c_str() );
  std::cout << "Saving the resulting mask to disk..." << std::endl;
  writer->Update();
  std::cout << "Done." << std::endl;

} // end DetectGoldMarkers


  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp( void )
{
  std::cout << "This program creates a mask for mr_bffe images of the prostate that contains gold marker seeds." << std::endl;
  std::cout << "The program computes the following:\n"
    << "  Laplacian at scale 'sigma' (L_xx(sigma) + L_yy(sigma) + L_zz(sigma)),\n"
    << "  A highpass filtered version of the original image. (L - L(sigma)),\n"
    << "  BlurAbsHighPass = Gaussian(sigma)[ abs(highpass) ];\n"
    << "  FeatureImage = Laplacian times BlurAbsHighPass;\n"
    << "  Histogram of FeatureImage; the quantile 'threshold' is used to determine a threshold value,\n"
    << "  Threshold of the FeatureImage;\n"
    << "  Dilation of the Threshold FeatureImage with binary ball with specified 'radius';\n"
    << "  A Not-filter to make the gold markers 0 and the rest 1.\n" << std::endl;

  std::cout << "Usage:" << std::endl << "pxdetectgoldmarkers" << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-out]   outputFilename, default in + MASK.mhd" << std::endl;
  std::cout << "  [-r]     radius [voxels, unsigned int]; radius of ball structuring element used for dilation; default: 7." << std::endl;
  std::cout << "  [-s]     sigma [voxels, double]; standard deviation of the gaussian used to compute the Laplacian and the blurring; default 4.0." << std::endl;
  std::cout << "  [-t]     threshold (0,1); before dilation, a threshold is done; default: 0.9999" << std::endl;
  std::cout << "  [-dim]   dimension, default 3" << std::endl;
  std::cout << "  [-pt]    pixelType, default short" << std::endl;
  std::cout << "Supported: 3D, short." << std::endl;
} // end PrintHelp

