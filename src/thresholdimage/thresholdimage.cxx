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
    ThresholdImage< InputImageType >( inputFileName, outputFileName, threshold1, threshold2 ); \
    supported = true; \
  } \
  else if ( method == "OtsuThreshold" ) \
  { \
    OtsuThresholdImage< InputImageType >( inputFileName, outputFileName, maskFileName, bins ); \
    supported = true; \
  } \
  else if ( method == "OtsuMultipleThreshold" ) \
  { \
    OtsuMultipleThresholdImage< InputImageType >( inputFileName, outputFileName, \
      maskFileName, bins, numThresholds ); \
    supported = true; \
  } \
  else if ( method == "RobustAutomaticThreshold" ) \
  { \
    RobustAutomaticThresholdImage< InputImageType >( inputFileName, outputFileName, \
      pow ); \
    supported = true; \
  } \
  else if ( method == "KappaSigmaThreshold" ) \
  { \
    KappaSigmaThresholdImage< InputImageType >( inputFileName, outputFileName, \
      maskFileName, maskValue, sigma, iterations ); \
    supported = true; \
  } \
  else if ( method == "MinErrorThreshold" ) \
  { \
    MinErrorThresholdImage< InputImageType >( inputFileName, outputFileName, bins, mixtureType ); \
    supported = true; \
  } \
}

//-------------------------------------------------------------------------------------

/** Declare threshold functions. */
template< class InputImageType >
void ThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & threshold1,
  const double & threshold2 );

template< class InputImageType >
void OtsuThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const unsigned int & bins );

template< class InputImageType >
void OtsuMultipleThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
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
  const double & pow );

template< class InputImageType >
void KappaSigmaThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const unsigned int & maskValue,
  const double & sigma,
  const unsigned int & iterations );

template< class InputImageType >
void MinErrorThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const unsigned int & bins,
  const unsigned int & mixtureType );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 3 )
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
  outputFileName += "THRESHOLDED.mhd";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string method = "Threshold";
  parser->GetCommandLineArgument( "-m", method );

  double threshold1 = itk::NumericTraits<double>::NonpositiveMin();
  bool rett1 = parser->GetCommandLineArgument( "-t1", threshold1 );

  double threshold2 = itk::NumericTraits<double>::One;
  bool rett2 = parser->GetCommandLineArgument( "-t2", threshold2 );

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
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
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
  thresholder->SetInsideValue( itk::NumericTraits<OutputPixelType>::Zero );
  thresholder->SetOutsideValue( itk::NumericTraits<OutputPixelType>::One );
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
  const unsigned int & bins )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

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
  thresholder->SetInsideValue( 0 );
  thresholder->SetOutsideValue( 1 );
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
  const unsigned int & bins,
  const unsigned int & numThresholds )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

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
  //thresholder->SetInsideValue( 0 );
  //thresholder->SetOutsideValue( 1 );
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
  const double & pow )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

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
  thresholder->SetInsideValue( 1 );
  thresholder->SetOutsideValue( 0 );
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
  const unsigned int & maskValue,
  const double & sigma,
  const unsigned int & iterations )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

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
  thresholder->SetInsideValue( 1 );
  thresholder->SetOutsideValue( 0 );
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
  const unsigned int & bins,
  const unsigned int & mixtureType )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

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
  thresholder->SetInsideValue( 1 );
  thresholder->SetOutsideValue( 0 );
  thresholder->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end MinErrorThresholdImage()


/**
 * ******************* PrintHelp *******************
 */

void PrintHelp( void )
{
  std::cout << "This program thresholds an image.\n";
  std::cout << "Usage:\npxthresholdimage\n";
  std::cout << "  -in      inputFilename\n";
  std::cout << "  [-out]   outputFilename; default in + THRESHOLDED.mhd\n";
  std::cout << "  [-mask]  maskFilename, optional for \"OtsuThreshold\", "
    << "required for \"KappaSigmaThreshold\"\n";
  std::cout << "  [-m]     method, choose one of { Threshold, OtsuThreshold, "
    << "OtsuMultipleThreshold, AdaptiveOtsuThreshold, RobustAutomaticThreshold, "
    << "KappaSigmaThreshold, MinErrorThreshold }\n";
  std::cout << "           default \"Threshold\"\n";
  std::cout << "  [-t1]    lower threshold, for \"Threshold\", default -infinity\n";
  std::cout << "  [-t2]    upper threshold, for \"Threshold\", default 1.0\n";
  std::cout << "  [-t]     number of thresholds, for \"OtsuMultipleThreshold\", default 1\n";
  std::cout << "  [-b]     number of histogram bins, for \"OtsuThreshold\", \"MinErrorThreshold\" "
    << "and \"AdaptiveOtsuThreshold\", default 128\n";
  std::cout << "  [-r]     radius, for \"AdaptiveOtsuThreshold\", default 8\n";
  std::cout << "  [-cp]    number of control points, for \"AdaptiveOtsuThreshold\", default 50\n";
  std::cout << "  [-l]     number of levels, for \"AdaptiveOtsuThreshold\", default 3\n";
  std::cout << "  [-s]     number of samples, for \"AdaptiveOtsuThreshold\", default 5000\n";
  std::cout << "  [-o]     spline order, for \"AdaptiveOtsuThreshold\", default 3\n";
  std::cout << "  [-p]     power, for \"RobustAutomaticThreshold\", default 1\n";
  std::cout << "  [-sigma] sigma factor, for \"KappaSigmaThreshold\", default 2\n";
  std::cout << "  [-iter]  number of iterations, for \"KappaSigmaThreshold\", default 2\n";
  std::cout << "  [-mv]    mask value, for \"KappaSigmaThreshold\", default 1\n";
  std::cout << "  [-mt]    mixture type (1 - Gaussians, 2 - Poissons), for \"MinErrorThreshold\", default 1\n";
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double." << std::endl;

} // end PrintHelp()

