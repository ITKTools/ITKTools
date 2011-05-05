/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
/** \file
 \brief This program computes a spatial segmentation error map.
 
 \verbinclude segmentationdistance.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkConstantPadImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkImageMomentsCalculator.h"
#include "itkCastImageFilter.h"
#include "itkCartesianToSphericalCoordinateImageFilter.h"
#include "itkAccumulateImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkAddImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkLinearInterpolateImageFunction.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName1, inputFileName2, outputFileName,\
    manualcor, samples, thetasize, phisize, cartesianonly ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/* Declare DetectGoldMarkers. */
template< class InputImageType >
void SegmentationDistance(
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::vector<double> & mancor,
  unsigned int samples,
  unsigned int thetasize,
  unsigned int phisize,
  bool cartesianonly );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

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
  
  /** Get the input file names. */
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  if( (inputFileNames.size() != 2)  )
  {
    std::cout << "ERROR: You should specify two input images." << std::endl;
    return 1;
  }
  std::string inputFileName1 = inputFileNames[ 0 ];
  std::string inputFileName2 = inputFileNames[ 1 ];

  /** Get the outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2 =
      itksys::SystemTools::GetFilenameName(inputFileName2);

    std::string ops = "DISTANCE";

    /** compose outputfilename */
    outputFileName = part1 + ops + part2;
  }

  std::vector<double> manualcor;
  parser->GetCommandLineArgument( "-c", manualcor);

  unsigned int samples = 20;
  parser->GetCommandLineArgument( "-s", samples );

  unsigned int thetasize = 180;
  parser->GetCommandLineArgument( "-t", thetasize );

  unsigned int phisize = 90;
  parser->GetCommandLineArgument( "-p", phisize );

  std::string cartesianstr = "false";
  bool cartesianonly = false;
  parser->GetCommandLineArgument( "-car", cartesianstr);
  if ( cartesianstr == "true" )
  {
    cartesianonly = true;
  }

  /** This program supports only the following INTERNAL pixel type.
   * The input images  are assumed to be short. Input images other
   * than short are supported, but automatically converted to short. */
  unsigned int Dimension = 3;
  std::string PixelType = "float";

  /** Get rid of the possible "_" in PixelType. */
  ReplaceUnderscoreWithSpace( PixelType );

  /** Run the program. */
  bool supported = false;
  try
  {
    run( SegmentationDistance, float, 3 );
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



  /*
   * ******************* SegmentationDistanceHelper ****************
   *
   * The function that does the work, templated over the image type.
   */

template< class InputImageType1, class InputImageType2, class ImageType >
void SegmentationDistanceHelper(
  const InputImageType1 * inputImage1,
  const InputImageType2 * inputImage2,
  typename ImageType::Pointer & accum1,
  typename ImageType::Pointer & accum2,
  typename ImageType::Pointer & distanceTransformOnEdge,
  typename ImageType::Pointer & edgeImage,
  std::vector<double> & mancor,
  unsigned int samples,
  unsigned int thetasize,
  unsigned int phisize,
  bool cartesianonly,
  bool invertedImage  )
{

  /** constants */
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef typename ImageType::PixelType               PixelType;
  typedef typename InputImageType1::PixelType         InputPixelType1;
  typedef typename InputImageType2::PixelType         InputPixelType2;

  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType1, ImageType>                       DistanceMapFilterType1;
  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType2, ImageType>                       DistanceMapFilterType2;
  typedef itk::BinaryThresholdImageFilter<
    ImageType, ImageType  >                           ThresholdFilterType;
  typedef itk::MultiplyImageFilter<
    ImageType, ImageType, ImageType>                  MultiplierType;
  typedef itk::ImageMomentsCalculator<
    InputImageType1 >                                 MomentCalculatorType;
  typedef itk::CartesianToSphericalCoordinateImageFilter<
    ImageType, ImageType>                             CSCFilterType;
  typedef itk::LinearInterpolateImageFunction<
    ImageType, double>                                InterpolatorType;
  typedef itk::AccumulateImageFilter<
    ImageType, ImageType>                             AccumulatorType;

  typedef typename InputImageType1::IndexType         IndexType;
  typedef typename InputImageType1::SizeType          SizeType;
  typedef typename InputImageType1::SpacingType       SpacingType;
  typedef typename CSCFilterType::SizeType            RTPSizeType;
  typedef typename CSCFilterType::PointType           PointType;
  typedef typename CSCFilterType::MaskImageType       MaskImageType;
  typedef itk::CastImageFilter<
    ImageType, MaskImageType >                        ToMaskImageCasterType;
  typedef typename MomentCalculatorType::VectorType   VectorType;
  typedef itk::ImageRegionIterator<
    ImageType>                                        OutputIteratorType;

  /** Instantiate filters */
  typename DistanceMapFilterType1::Pointer distanceMapFilter1 =
    DistanceMapFilterType1::New();
  typename DistanceMapFilterType2::Pointer distanceMapFilter2 =
    DistanceMapFilterType2::New();
  typename ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  typename MultiplierType::Pointer multiplier = MultiplierType::New();
  typename MultiplierType::Pointer multiplier2 = MultiplierType::New();
  typename ToMaskImageCasterType::Pointer toMaskImageCaster = ToMaskImageCasterType::New();
  typename CSCFilterType::Pointer cscFilter1 = CSCFilterType::New();
  typename CSCFilterType::Pointer cscFilter2 = CSCFilterType::New();
  typename InterpolatorType::Pointer interpolator1 = InterpolatorType::New();
  typename InterpolatorType::Pointer interpolator2 = InterpolatorType::New();
  typename MomentCalculatorType::Pointer momentCalculator =
    MomentCalculatorType::New();
  typename AccumulatorType::Pointer accumulator1 = AccumulatorType::New();
  typename AccumulatorType::Pointer accumulator2 = AccumulatorType::New();

  /** Compute the distance map of image 1 */
  distanceMapFilter1->SetInput( inputImage1 );
  distanceMapFilter1->SetUseImageSpacing( true );
  distanceMapFilter1->SetSquaredDistance( false );
  std::cout << "Computing distance map D of input image 1..." << std::endl;
  distanceMapFilter1->Update();
  std::cout << "Distance map computed." << std::endl;

  /** Compute the distance map of image 2 */
  distanceMapFilter2->SetInput( inputImage2 );
  distanceMapFilter2->SetUseImageSpacing( true );
  distanceMapFilter2->SetSquaredDistance( false );
  std::cout << "Computing distance map D of input image 2..." << std::endl;
  distanceMapFilter2->Update();
  std::cout << "Distance map computed." << std::endl;

  /** Compute minimum spacing */
  double minSpacing = itk::NumericTraits<double>::max();
  SpacingType inputSpacing = inputImage1->GetSpacing();
  for (unsigned int i = 0; i < Dimension; ++i)
  {
    minSpacing = vnl_math_min( minSpacing, inputSpacing[i]);
  }

  /** Find distanceMap2==0 pixels */
  thresholder->SetInput( distanceMapFilter2->GetOutput() );
  thresholder->SetUpperThreshold(minSpacing*0.5);
  thresholder->SetLowerThreshold(-minSpacing*0.5);
  thresholder->SetInsideValue(1.0);
  thresholder->SetOutsideValue(0.0);
  std::cout << "Thresholding distance map 2..." << std::endl;
  thresholder->Update();
  std::cout << "Done thresholding." << std::endl;

  /** Save for the caller of this function */
  edgeImage = thresholder->GetOutput();
  multiplier2->SetInput1( edgeImage );
  multiplier2->SetInput2( distanceMapFilter1->GetOutput() );
  multiplier2->Update();
  distanceTransformOnEdge = multiplier2->GetOutput();
  if ( cartesianonly )
  {
    return;
  }

  /** Compute the center of gravity of image 1 */
  PointType cor;
  if ( mancor.size() == Dimension )
  {
    for ( unsigned int i=0; i< Dimension; ++i)
    {
      cor[i]=mancor[i];
    }
    std::cout << "Center of mass given by user: " << cor << std::endl;
  }
  else
  {
    mancor.resize( Dimension);
    momentCalculator->SetImage( inputImage1 );
    std::cout << "Computing center of mass of image 1..." << std::endl;
    momentCalculator->Compute();
    VectorType corvec = momentCalculator->GetCenterOfGravity();
    std::cout << "Center of mass computed. Result: " << corvec << std::endl;
    // convert to point
    for ( unsigned int i=0; i< Dimension; ++i)
    {
      cor[i] = corvec[i];
      /** return to user */
      mancor[i] = cor[i];
    }
  }

  /** Compute maximum r */
  IndexType inputIndex = inputImage1->
    GetLargestPossibleRegion().GetIndex();
  SizeType inputSize = inputImage1->
    GetLargestPossibleRegion().GetSize();
  double maxR = 0.0;
  IndexType cornerIndex = inputIndex;
  for (unsigned int i = 0; i < 2; ++i)
  {
    cornerIndex[0] = inputIndex[0] + i * (inputSize[0] -1);
    for (unsigned int j = 0; j < 2; ++j)
    {
      cornerIndex[1] = inputIndex[1] + j * (inputSize[1] -1);
      for (unsigned int k = 0; k < 2; ++k)
      {
        cornerIndex[2] = inputIndex[2] + k * (inputSize[2] -1 );
        PointType cornerPoint;
        inputImage1->TransformIndexToPhysicalPoint(
          cornerIndex, cornerPoint);
        VectorType vec = cornerPoint - cor;
        maxR = vnl_math_max( maxR, vec.GetNorm() );
      }
    }
  }

  /** Convert edgeImage to MaskImageType */
  toMaskImageCaster->SetInput( edgeImage );
  toMaskImageCaster->Update();

  /** Computing spherical transforms */
  RTPSizeType rtpSize;
  rtpSize[0] = static_cast<unsigned int>( vcl_ceil(maxR / minSpacing ) );
  std::cout << "r = " << rtpSize[0] << std::endl;
  rtpSize[1] = thetasize;
  rtpSize[2] = phisize;
  cscFilter1->SetInput( distanceMapFilter1->GetOutput() );
  cscFilter1->SetMaskImage( toMaskImageCaster->GetOutput() );
  cscFilter1->SetOutputSize( rtpSize);
  cscFilter1->SetCenterOfRotation( cor );
  cscFilter1->SetMaximumNumberOfSamplesPerVoxel(samples);
  cscFilter1->SetInterpolator( interpolator1);
  cscFilter2->SetInput( edgeImage );
  cscFilter2->SetMaskImage( toMaskImageCaster->GetOutput() );
  cscFilter2->SetOutputSize( rtpSize);
  cscFilter2->SetCenterOfRotation( cor );
  cscFilter2->SetMaximumNumberOfSamplesPerVoxel(samples);
  cscFilter2->SetInterpolator( interpolator2);
  std::cout << "Computing spherical transforms of D and E: S(D) and S(E)..." << std::endl;
  cscFilter1->GetRandomGenerator()->SetSeed(12345);
  cscFilter1->Update();
  cscFilter2->GetRandomGenerator()->SetSeed(12345);
  cscFilter2->Update();
  std::cout << "Spherical transforms computed." << std::endl;

  /** Computing DE = S(DistanceMap)*S(EdgeImage) */
  multiplier->SetInput1( cscFilter1->GetOutput() );
  multiplier->SetInput2( cscFilter2->GetOutput() );
  std::cout << "Computing DE = S(D) * S(E)..." << std::endl;
  multiplier->Update();
  std::cout << "Multiplying done." << std::endl;

  /** Integrate along r dimension */
  accumulator1->SetInput( multiplier->GetOutput() );
  accumulator1->SetAccumulateDimension(0);
  accumulator1->SetAverage(false);
  accumulator2->SetInput( cscFilter2->GetOutput() );
  accumulator2->SetAccumulateDimension(0);
  accumulator2->SetAverage(false);
  std::cout << "Integrate along r dimension of the spherical transforms..." << std::endl;
  accumulator1->Update();
  accumulator2->Update();
  std::cout << "Done integrating." << std::endl;

  accum1 = accumulator1->GetOutput();
  accum2 = accumulator2->GetOutput();

} // end SegmentationDistanceHelper



  /*
   * ******************* SegmentationDistance ****************
   *
   * The function that does the work, templated over the image type.
   */

template< class ImageType >
void SegmentationDistance(
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::vector<double> & mancor,
  unsigned int samples,
  unsigned int thetasize,
  unsigned int phisize,
  bool cartesianonly)
{

  /** constants */
  const unsigned int Dimension = ImageType::ImageDimension;
  const unsigned int OutputDimension = Dimension-1;
  typedef typename ImageType::PixelType               PixelType;
  typedef short                                       InputPixelType1;
  typedef short                                       InputPixelType2;

  /** TYPEDEF's. */
  typedef itk::Image<InputPixelType1, Dimension>      InputImageType1;
  typedef itk::Image<InputPixelType2, Dimension>      InputImageType2;

  typedef itk::Image<PixelType, OutputDimension>      OutputImageType;

  typedef itk::ImageFileReader<InputImageType1>       ReaderType1;
  typedef itk::ImageFileReader<InputImageType2>       ReaderType2;
  typedef itk::ConstantPadImageFilter<
    InputImageType1, InputImageType1>                 PadderType1;
  typedef itk::ConstantPadImageFilter<
    InputImageType2, InputImageType2>                 PadderType2;
  typedef itk::AddImageFilter<
    ImageType, ImageType, ImageType>                  AdderType;
  typedef itk::SubtractImageFilter<
    ImageType, ImageType, ImageType>                  SubtracterType;
  typedef itk::DivideImageFilter<
    ImageType, ImageType, ImageType>                  DividerType;
  typedef itk::ExtractImageFilter<
    ImageType, OutputImageType>                       ExtracterType;
  typedef itk::ImageFileWriter<OutputImageType>       WriterType;
  typedef itk::ImageFileWriter<ImageType>             WriterCartesianType;

  typedef typename
    ExtracterType::InputImageRegionType               RegionType;
  typedef typename RegionType::IndexType              IndexType;
  typedef typename RegionType::SizeType               SizeType;
  typedef itk::ImageRegionConstIterator<
    InputImageType1>                                  ConstInputIteratorType1;
  typedef itk::ImageRegionConstIterator<
    InputImageType2>                                  ConstInputIteratorType2;
  typedef itk::ImageRegionIterator<
    InputImageType1>                                  InputIteratorType1;
  typedef itk::ImageRegionIterator<
    InputImageType2>                                  InputIteratorType2;
  typedef itk::ImageRegionIterator<
    ImageType>                                        OutputIteratorType;
  typedef itk::ImageRegionConstIterator<
    ImageType>                                        ConstOutputImageIteratorType;

  /** Instantiate filters */
  typename ReaderType1::Pointer reader1 = ReaderType1::New();
  typename ReaderType2::Pointer reader2 = ReaderType2::New();
  typename PadderType1::Pointer padder1 = PadderType1::New();
  typename PadderType2::Pointer padder2 = PadderType2::New();
  typename AdderType::Pointer adder = AdderType::New();
  typename AdderType::Pointer adderEdgeCartesian = AdderType::New();
  typename SubtracterType::Pointer subtracter = SubtracterType::New();
  typename SubtracterType::Pointer subtracterDistCartesian = SubtracterType::New();
  typename DividerType::Pointer divider = DividerType::New();
  typename ExtracterType::Pointer extracter = ExtracterType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename WriterCartesianType::Pointer writerDistCartesian = WriterCartesianType::New();
  typename WriterCartesianType::Pointer writerEdgeCartesian = WriterCartesianType::New();

   /** Read in the inputImages. */
  reader1->SetFileName( inputFileName1.c_str() );
  reader2->SetFileName( inputFileName2.c_str() );
  std::cout << "Reading input images..." << std::endl;
  reader1->Update();
  reader2->Update();
  std::cout << "Input images read." << std::endl;

  /** Pad them with zeros, to make sure the edges of objects facing the boundary
   * of the image are counted as edges */
  padder1->SetInput( reader1->GetOutput() );
  padder2->SetInput( reader2->GetOutput() );
  unsigned long padsize[Dimension];
  for (unsigned int i = 0; i < Dimension; ++i)
  {
    padsize[i] = 1;
  }
  padder1->SetPadUpperBound(padsize);
  padder1->SetPadLowerBound(padsize);
  padder2->SetPadUpperBound(padsize);
  padder2->SetPadLowerBound(padsize);
  std::cout << "Padding input images with zeros..." << std::endl;
  padder1->Update();
  padder2->Update();
  std::cout << "Done padding." << std::endl;

  /** Compute the distance */
  typename ImageType::Pointer accum1 = 0;
  typename ImageType::Pointer accum2 = 0;
  typename ImageType::Pointer dist = 0;
  typename ImageType::Pointer edge = 0;

  std::vector<double> cor = mancor;

  SegmentationDistanceHelper<InputImageType1, InputImageType2, ImageType>(
    padder1->GetOutput(), padder2->GetOutput(), accum1, accum2, dist, edge,
    cor, samples, thetasize, phisize, cartesianonly, false);

  /** Compute 1 minus the input images */
  typename InputImageType1::Pointer invInputImage1 = InputImageType1::New();
  typename InputImageType2::Pointer invInputImage2 = InputImageType2::New();
  invInputImage1->SetRegions( padder1->GetOutput()->GetLargestPossibleRegion() );
  invInputImage2->SetRegions( padder2->GetOutput()->GetLargestPossibleRegion() );
  invInputImage1->SetSpacing( padder1->GetOutput()->GetSpacing() );
  invInputImage2->SetSpacing( padder2->GetOutput()->GetSpacing() );
  invInputImage1->SetOrigin( padder1->GetOutput()->GetOrigin() );
  invInputImage2->SetOrigin( padder2->GetOutput()->GetOrigin() );
  invInputImage1->Allocate();
  invInputImage2->Allocate();

  ConstInputIteratorType1 init1( padder1->GetOutput(), padder1->GetOutput()->GetLargestPossibleRegion() );
  ConstInputIteratorType2 init2( padder2->GetOutput(), padder2->GetOutput()->GetLargestPossibleRegion() );
  InputIteratorType1 invinit1( invInputImage1, invInputImage1->GetLargestPossibleRegion() );
  InputIteratorType2 invinit2( invInputImage2, invInputImage2->GetLargestPossibleRegion() );
  init1.GoToBegin();
  init2.GoToBegin();
  invinit1.GoToBegin();
  invinit2.GoToBegin();
  while ( !init1.IsAtEnd() )
  {
    invinit1.Value() = itk::NumericTraits<InputPixelType1>::One - init1.Value();
    invinit2.Value() = itk::NumericTraits<InputPixelType1>::One - init2.Value();

    ++init1;
    ++init2;
    ++invinit1;
    ++invinit2;
  }

  /** Compute again the distance */
  typename ImageType::Pointer accum1inv = 0;
  typename ImageType::Pointer accum2inv = 0;
  typename ImageType::Pointer distinv = 0;
  typename ImageType::Pointer edgeinv = 0;

  SegmentationDistanceHelper<InputImageType1, InputImageType2, ImageType>(
    invInputImage1, invInputImage2, accum1inv, accum2inv, distinv, edgeinv,
    cor, samples, thetasize, phisize, cartesianonly, true);

  //
  if ( cartesianonly )
  {


    /** Compute dist-distinv and edge+edgeinv */
    subtracterDistCartesian->SetInput1( dist );
    subtracterDistCartesian->SetInput2( distinv );
    adderEdgeCartesian->SetInput1( edge);
    adderEdgeCartesian->SetInput2( edgeinv);
    subtracterDistCartesian->Update();
    adderEdgeCartesian->Update();

    /** outputfilename extensie afknippen en DIST en EDGE toevoegen.*/
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(outputFileName);
    /** get file name extension */
    std::string part2 = "";
    part2 += itksys::SystemTools::GetFilenameLastExtension(outputFileName);
    std::string diststr = "DIST";
    std::string edgestr = "EDGE";

    /** compose outputfilename */
    std::string outputFileNameDIST = part1 + diststr + part2;
    std::string outputFileNameEDGE = part1 + edgestr + part2;

    /** Write to disk */
    writerDistCartesian->SetFileName( outputFileNameDIST );
    writerEdgeCartesian->SetFileName( outputFileNameEDGE );
    writerDistCartesian->SetInput( subtracterDistCartesian->GetOutput() );
    writerEdgeCartesian->SetInput( adderEdgeCartesian->GetOutput() );

    std::cout << "The spherical transforms are skipped and the results are written as:\n\t"
      << outputFileNameDIST << "\n\t"  << outputFileNameEDGE << std::endl;
    writerDistCartesian->Update();
    writerEdgeCartesian->Update();

    return;
  }

  /** Add the results (subtract for distance transform, because its negated) */
  subtracter->SetInput1( accum1);
  subtracter->SetInput2( accum1inv);
  adder->SetInput1( accum2);
  adder->SetInput2( accum2inv);
  std::cout << "Averaging the results of the normal images and the inverted images." << std::endl;
  subtracter->Update();
  adder->Update();
  std::cout << "Ready averaging..." << std::endl;

  typename ImageType::Pointer sumEdgeAccums = adder->GetOutput();
  sumEdgeAccums->DisconnectPipeline();
  OutputIteratorType it( sumEdgeAccums,
    sumEdgeAccums->GetLargestPossibleRegion() );
  const double smallnumber= 1e-10;
  it.GoToBegin();
  while (!it.IsAtEnd() )
  {
    /** If a (theta,phi) combination didn't pass through the edge
     * at least smallnumber times, do not count it. */
    if ( it.Value() < smallnumber )
    {
      it.Value() = itk::NumericTraits<PixelType>::max();
    }
    ++it;
  }

  /** Divide the integrated spherical transforms */
  divider->SetInput1( subtracter->GetOutput() );
  divider->SetInput2( sumEdgeAccums );
  std::cout << "Dividing the averaged integrated spherical transforms..." << std::endl;
  divider->Update();
  std::cout << "Dividing done." << std::endl;

  /** Collapse to 2d image */
  extracter->SetInput( divider->GetOutput() );
  RegionType extractionRegion = divider->GetOutput()->GetLargestPossibleRegion();
  SizeType extractionSize = extractionRegion.GetSize();
  extractionSize[0] = 0;
  extractionRegion.SetSize( extractionSize );
  extracter->SetExtractionRegion( extractionRegion );
  std::cout << "Collapsing the result to a 2d image..." << std::endl;
  extracter->Update();
  std::cout << "Done collapsing." << std::endl;

  /** Write the output image. */
  writer->SetInput( extracter->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  std::cout << "Saving the result to disk as: " << outputFileName << std::endl;
  writer->Update();
  std::cout << "Done." << std::endl;

} // end SegmentationDistance


  /**
   * ******************* GetHelpString *******************
   */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program computes a spatial segmentation error map." << std::endl
  << "Usage:" << std::endl
  << "pxsegmentationdistance" << std::endl
  << "  -in      inputFilename1 inputFileName2" << std::endl
  << "  [-out]   outputFilename, default <in1>DISTANCE<in2>.mhd" << std::endl
  << "  [-c]     Center of rotation, used to compute the spherical transform. In world coordinates." << std::endl
  << "  [-s]     samples [unsigned int]; maximum number of samples per pixel, used to do the spherical transform; default 20." << std::endl
  << "  [-t]     theta size; the size of the theta dimension. default: 180, which yields a spacing of 2 degrees." << std::endl
  << "  [-p]     phi size; the size of the phi dimension. default: 90, which yields a spacing of 2 degrees." << std::endl
  << "  [-car]   skip the polar transform and return two output images (outputFileNameDIST and outputFileNameEDGE): true or false; default = false" << std::endl
  << "           The EDGE output image is an edge mask for inputfile2. The DIST output image contains the distance at each edge pixel to the first inputFile." << std::endl
  << "Supported: 3D short for inputImage1, and everything convertable to short." << std::endl
  << "           3D short for inputImage2, and everything convertable to short.";

  return ss.str();
} // end GetHelpString

