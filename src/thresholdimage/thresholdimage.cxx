#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkOtsuThresholdWithMaskImageFilter.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkAdaptiveOtsuThresholdImageFilter.h"
#include "itkRobustAutomaticThresholdImageFilter.h"
#include "itkKappaSigmaThresholdImageFilter.h"
#include "itkMinErrorThresholdImageFilter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  if ( method == "Threshold" ) \
  { \
    ThresholdImage< InputImageType >( inputFileName, outputFileName, \
    inside, outside, \
    threshold1, threshold2 ); \
    supported = true; \
  } \
  else if ( method == "OtsuThreshold" ) \
  { \
    OtsuThresholdImage< InputImageType >( inputFileName, outputFileName, maskFileName, \
    inside, outside, \
    bins ); \
    supported = true; \
  } \
  else if ( method == "OtsuMultipleThreshold" ) \
  { \
    OtsuMultipleThresholdImage< InputImageType >( inputFileName, outputFileName, maskFileName, \
    inside, outside, \
    bins, numThresholds ); \
    supported = true; \
  } \
  else if ( method == "RobustAutomaticThreshold" ) \
  { \
    RobustAutomaticThresholdImage< InputImageType >( inputFileName, outputFileName, \
    inside, outside, \
    pow ); \
    supported = true; \
  } \
  else if ( method == "KappaSigmaThreshold" ) \
  { \
    KappaSigmaThresholdImage< InputImageType >( inputFileName, outputFileName, maskFileName, \
    inside, outside, \
    maskValue, sigma, iterations ); \
    supported = true; \
  } \
  else if ( method == "MinErrorThreshold" ) \
  { \
    MinErrorThresholdImage< InputImageType >( inputFileName, outputFileName, \
    inside, outside, \
    bins, mixtureType ); \
    supported = true; \
  } \
}

//-------------------------------------------------------------------------------------

/** Declare threshold functions. */
template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & threshold1,
  const double & threshold2 );

template< class InputImageType >
void OtsuThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins );

template< class InputImageType >
void OtsuMultipleThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & numThresholds );

// template< class InputImageType >
// void AdaptiveOtsuThresholdImage(
//   const std::string & inputFileName,
//   const std::string & outputFileName,
//   const unsigned int & radius,
//   const unsigned int & bins,
//   const unsigned int & controlPoints,
//   const unsigned int & levels,
//   const unsigned int & samples,
//   const unsigned int & splineOrder );

template< class InputImageType >
void RobustAutomaticThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & pow );

template< class InputImageType >
void KappaSigmaThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & maskValue,
  const double & sigma,
  const unsigned int & iterations );

template< class InputImageType >
void MinErrorThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & mixtureType );

/** Declare PrintHelp. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "THRESHOLDED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string method = "Threshold";
  parser->GetCommandLineArgument( "-m", method );

  double threshold1 = itk::NumericTraits<double>::NonpositiveMin();
  parser->GetCommandLineArgument( "-t1", threshold1 );

  double threshold2 = itk::NumericTraits<double>::One;
  parser->GetCommandLineArgument( "-t2", threshold2 );

  double inside = itk::NumericTraits<double>::One;
  parser->GetCommandLineArgument( "-inside", inside );

  double outside = itk::NumericTraits<double>::Zero;
  parser->GetCommandLineArgument( "-outside", outside );

  unsigned int radius = 8;
  parser->GetCommandLineArgument( "-r", radius );

  unsigned int bins = 128;
  parser->GetCommandLineArgument( "-b", bins );

  unsigned int numThresholds = 1;
  parser->GetCommandLineArgument( "-t", numThresholds );

  unsigned int controlPoints = 50;
  parser->GetCommandLineArgument( "-cp", controlPoints );

  unsigned int levels = 3;
  parser->GetCommandLineArgument( "-l", levels );

  unsigned int samples = 5000;
  parser->GetCommandLineArgument( "-s", samples );

  unsigned int splineOrder = 3;
  parser->GetCommandLineArgument( "-o", splineOrder );

  double pow = 1.0;
  parser->GetCommandLineArgument( "-p", pow );

  double sigma = 2.0;
  parser->GetCommandLineArgument( "-sigma", sigma );

  unsigned int iterations = 2;
  parser->GetCommandLineArgument( "-iter", iterations );

  unsigned int maskValue = 1;
  parser->GetCommandLineArgument( "-mv", maskValue );

  unsigned int mixtureType = 1;
  parser->GetCommandLineArgument( "-mt", mixtureType );

  /** Checks. */
  if ( method != "Threshold"
    && method != "OtsuThreshold"
    && method != "OtsuMultipleThreshold"
    && method != "AdaptiveOtsuThreshold"
    && method != "RobustAutomaticThreshold"
    && method != "KappaSigmaThreshold"
    && method != "MinErrorThreshold" )
  {
    std::cerr << "ERROR: method \"-m\" should be one of { Threshold, "
      << "OtsuThreshold, OtsuMultipleThreshold, AdaptiveOtsuThreshold, "
      << "RobustAutomaticThreshold, KappaSigmaThreshold, MinErrorThreshold }." << std::endl;
    return 1;
  }
  if ( method == "KappaSigmaThreshold" && maskFileName == "" )
  {
    std::cerr << "ERROR: the method \"KappaSigmaThreshold\" requires setting a mask using \"-mask\"." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
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
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Run the program. */
  bool supported = false;
  try
  {
    /** 2D. */
    run( ThresholdImage, char, 2 );
    run( ThresholdImage, unsigned char, 2 );
    run( ThresholdImage, short, 2 );
    run( ThresholdImage, unsigned short, 2 );
    run( ThresholdImage, float, 2 );
    run( ThresholdImage, double, 2 );

    /** 3D. */
    run( ThresholdImage, char, 3 );
    run( ThresholdImage, unsigned char, 3 );
    run( ThresholdImage, short, 3 );
    run( ThresholdImage, unsigned short, 3 );
    run( ThresholdImage, float, 3 );
    run( ThresholdImage, double, 3 );

  } // end run
  catch ( itk::ExceptionObject &e )
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
 * ******************* ThresholdImage *******************
 */

template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & threshold1,
  const double & threshold2 )
{
  /** Typedef's. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef InputImageType                              OutputImageType;
  typedef InputPixelType                              OutputPixelType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::BinaryThresholdImageFilter<
    InputImageType, OutputImageType>                  ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Declarations. */
  InputPixelType lowerthreshold;
  typename ReaderType::Pointer reader = ReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  lowerthreshold = static_cast<InputPixelType>( vnl_math_max(
    static_cast<double>( itk::NumericTraits<InputPixelType>::NonpositiveMin() ),
    threshold1 ) );
  thresholder->SetLowerThreshold( lowerthreshold );
  thresholder->SetUpperThreshold( static_cast<InputPixelType>( threshold2 ) );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end ThresholdImage()


/**
 * ******************* OtsuThresholdImage *******************
 */

template< class InputImageType >
void OtsuThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::OtsuThresholdWithMaskImageFilter<
    InputImageType, OutputImageType>                    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetNumberOfHistogramBins( bins );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader1->GetOutput() );
  if ( maskFileName != "" )
  {
    reader2->SetFileName( maskFileName.c_str() );
    thresholder->SetMaskImage( reader2->GetOutput() );
  }

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end OtsuThresholdImage()


/**
 * ******************* OtsuMultipleThresholdImage *******************
 */

template< class InputImageType >
void OtsuMultipleThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & itkNotUsed( maskFileName ),
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & numThresholds )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::OtsuMultipleThresholdsImageFilter<
    InputImageType, OutputImageType>                    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  //typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetInput( reader1->GetOutput() );
  thresholder->SetNumberOfHistogramBins( bins );
  //thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  //thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetNumberOfThresholds( numThresholds );
//   if ( maskFileName != "" )
//   {
//     reader2->SetFileName( maskFileName.c_str() );
//     thresholder->SetMaskImage( reader2->GetOutput() );
//   }

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end OtsuMultipleThresholdImage()


// /**
//  * ******************* AdaptiveOtsuThresholdImage *******************
//  */
//
// template< class InputImageType >
// void AdaptiveOtsuThresholdImage(
//   const std::string & inputFileName,
//   const std::string & outputFileName,
//   const unsigned int & radius,
//   const unsigned int & bins,
//   const unsigned int & controlPoints,
//   const unsigned int & levels,
//   const unsigned int & samples,
//   const unsigned int & splineOrder )
// {
//   /** Typedef's. */
//   const unsigned int ImageDimension = InputImageType::ImageDimension;
//
//   typedef unsigned char                                 OutputPixelType;
//   typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
//   typedef itk::ImageFileReader< InputImageType >        ReaderType;
//   typedef itk::AdaptiveOtsuThresholdImageFilter<
//     InputImageType, OutputImageType>                    ThresholderType;
//   typedef itk::ImageFileWriter< OutputImageType >       WriterType;
//   typedef ThresholderType::InputSizeType                RadiusType;
//
//   /** Declarations. */
//   typename ReaderType::Pointer reader = ReaderType::New();
//   typename ThresholderType::Pointer thresholder = ThresholderType::New();
//   typename WriterType::Pointer writer = WriterType::New();
//   RadiusType Radius; Radius.Fill( radius );
//
//   /** Read in the inputImage. */
//   reader->SetFileName( inputFileName.c_str() );
//
//   /** Apply the threshold. */
//   thresholder->SetRadius( Radius );
//   thresholder->SetNumberOfHistogramBins( bins );
//   thresholder->SetNumberOfControlPoints( controlPoints );
//   thresholder->SetNumberOfLevels( levels );
//   thresholder->SetNumberOfSamples( samples );
//   thresholder->SetSplineOrder( splineOrder );
//   thresholder->SetInsideValue( 1 );
//   thresholder->SetOutsideValue( 0 );
//   thresholder->SetInput( reader->GetOutput() );
//
//   /** Write the output image. */
//   writer->SetInput( thresholder->GetOutput() );
//   writer->SetFileName( outputFileName.c_str() );
//   writer->Update();
//
// } // end AdaptiveOtsuThresholdImage()


/**
 * ******************* RobustAutomaticThresholdImage *******************
 */

template< class InputImageType >
void RobustAutomaticThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & pow )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef float                                         GMPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::Image< GMPixelType, ImageDimension >     GMImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<
    InputImageType, GMImageType >                       GMFilterType;
  typedef itk::RobustAutomaticThresholdImageFilter<
    InputImageType, GMImageType, OutputImageType >      ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename GMFilterType::Pointer gradientFilter = GMFilterType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get the gradient magnitude of the input. */
  gradientFilter->SetInput( reader->GetOutput() );
  gradientFilter->SetSigma( 1.0 );
  gradientFilter->SetNormalizeAcrossScale( false );

  /** Apply the threshold. */
  thresholder->SetPow( pow );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );
  thresholder->SetGradientImage( gradientFilter->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end RobustAutomaticThresholdImage()


/**
 * ******************* KappaSigmaThresholdImage *******************
 */

template< class InputImageType >
void KappaSigmaThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & maskValue,
  const double & sigma,
  const unsigned int & iterations )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::KappaSigmaThresholdImageFilter<
    InputImageType, MaskImageType, OutputImageType >    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );
  reader2->SetFileName( maskFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetMaskValue( maskValue );
  thresholder->SetSigmaFactor( sigma );
  thresholder->SetNumberOfIterations( iterations );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader1->GetOutput() );
  thresholder->SetMaskImage( reader2->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end KappaSigmaThresholdImage()


/**
 * ******************* MinErrorThresholdImage *******************
 */

template< class InputImageType >
void MinErrorThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & mixtureType )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::MinErrorThresholdImageFilter<
    InputImageType, OutputImageType >                   ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();

  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetNumberOfHistogramBins( bins );
  thresholder->SetMixtureType( mixtureType );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end MinErrorThresholdImage()


/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText = "This program thresholds an image.\n \
  Usage:\npxthresholdimage\n \
    -in        inputFilename\n \
    [-out]     outputFilename; default in + THRESHOLDED.mhd\n \
    [-mask]    maskFilename, optional for \"OtsuThreshold\", \
  required for \"KappaSigmaThreshold\"\n \
    [-m]       method, choose one of { Threshold, OtsuThreshold, \
  OtsuMultipleThreshold, AdaptiveOtsuThreshold, RobustAutomaticThreshold, \
  KappaSigmaThreshold, MinErrorThreshold }\n \
               default \"Threshold\"\n \
    [-t1]      lower threshold, for \"Threshold\", default -infinity\n \
    [-t2]      upper threshold, for \"Threshold\", default 1.0\n \
    [-inside]  inside value, default 1\n \
    [-outside] outside value, default 0\n \
    [-t]       number of thresholds, for \"OtsuMultipleThreshold\", default 1\n \
    [-b]       number of histogram bins, for \"OtsuThreshold\", \"MinErrorThreshold\" \
  and \"AdaptiveOtsuThreshold\", default 128\n \
    [-r]       radius, for \"AdaptiveOtsuThreshold\", default 8\n \
    [-cp]      number of control points, for \"AdaptiveOtsuThreshold\", default 50\n \
    [-l]       number of levels, for \"AdaptiveOtsuThreshold\", default 3\n \
    [-s]       number of samples, for \"AdaptiveOtsuThreshold\", default 5000\n \
    [-o]       spline order, for \"AdaptiveOtsuThreshold\", default 3\n \
    [-p]       power, for \"RobustAutomaticThreshold\", default 1\n \
    [-sigma]   sigma factor, for \"KappaSigmaThreshold\", default 2\n \
    [-iter]    number of iterations, for \"KappaSigmaThreshold\", default 2\n \
    [-mv]      mask value, for \"KappaSigmaThreshold\", default 1\n \
    [-mt]      mixture type (1 - Gaussians, 2 - Poissons), for \"MinErrorThreshold\", default 1\n \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

  return helpText;
} // end PrintHelp()

