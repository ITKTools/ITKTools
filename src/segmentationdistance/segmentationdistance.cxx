#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
//#include "itkBinaryErodeImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkImageMomentsCalculator.h"
#include "itkCartesianToSphericalCoordinateImageFilter.h"
#include "itkAccumulateImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkDivideImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName1, inputFileName2, outputFileName,\
    manualcor, samples, thetasize, phisize ); \
}

//-------------------------------------------------------------------------------------

/** Declare DetectGoldMarkers. */
template< class InputImageType >
void SegmentationDistance(
  const std::string & inputFileName1,
  const std::string & inputFileName2,
	const std::string & outputFileName,
  const std::vector<double> & mancor,
  unsigned int samples,
	unsigned int thetasize,
	unsigned int phisize);

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 4 || argc > 12 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  /** Get the input file names. */
	std::vector< std::string >	inputFileNames;
	bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if( (inputFileNames.size() != 2)  )
	{
		std::cout << "ERROR: You should specify two input images." << std::endl;
		return 1;
	}
  std::string inputFileName1 = inputFileNames[ 0 ]; 
  std::string inputFileName2 = inputFileNames[ 1 ];
  
  /** Get the outputFileName */
  std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
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
  bool retc = parser->GetCommandLineArgument( "-c", manualcor);

  unsigned int samples = 20;
	bool rets = parser->GetCommandLineArgument( "-s", samples );

  unsigned int thetasize = 180;
	bool rett = parser->GetCommandLineArgument( "-t", thetasize );
  
  unsigned int phisize = 90;
	bool retp = parser->GetCommandLineArgument( "-p", phisize );
    
  /** This program supports only the following INTERNAL pixel type.
   * The input images  are assumed to be short. Input images other
   * than short are supported, but automatically converted to short. */
  unsigned int Dimension = 3;
  std::string PixelType = "float";
	
	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace(PixelType);
	
	/** Run the program. */
	try
	{
		run(SegmentationDistance, float, 3);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main


	/**
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
	unsigned int phisize)
{
	
	/** constants */
	const unsigned int Dimension = ImageType::ImageDimension;
  const unsigned int OutputDimension = Dimension-1;
  typedef typename ImageType::PixelType					      PixelType;
  typedef short                                       InputPixelType;
      
	/** TYPEDEF's. */
  typedef itk::Image<InputPixelType, Dimension>       InputImageType;
  typedef itk::Image<PixelType, OutputDimension>      OutputImageType;

  typedef itk::ImageFileReader<InputImageType>        ReaderType;
  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType, ImageType>                        DistanceMapFilterType;
  typedef itk::BinaryBallStructuringElement<
    InputPixelType, Dimension >												StructuringElementType;
	typedef itk::GrayscaleErodeImageFilter<
		InputImageType, InputImageType,
		StructuringElementType >													ErodeFilterType;
  typedef itk::SubtractImageFilter<
    InputImageType, InputImageType, ImageType>        SubtracterType;
  typedef itk::MultiplyImageFilter<
    ImageType, ImageType, ImageType>                  MultiplierType;
  typedef itk::ImageMomentsCalculator<
    InputImageType >                                  MomentCalculatorType;
  typedef itk::CartesianToSphericalCoordinateImageFilter<
    ImageType, ImageType>                             CSCFilterType;
  typedef itk::AccumulateImageFilter<
    ImageType, ImageType>                             AccumulatorType;
  typedef itk::DivideImageFilter<
    ImageType, ImageType, ImageType>                  DividerType;
  typedef itk::ExtractImageFilter<
    ImageType, OutputImageType>                       ExtracterType;
  typedef itk::ImageFileWriter<OutputImageType>       WriterType;

  typedef typename 
    ExtracterType::InputImageRegionType               RegionType;
  typedef typename RegionType::IndexType              IndexType;
  typedef typename RegionType::SizeType               SizeType;
  typedef typename CSCFilterType::SizeType            RTPSizeType;
  typedef typename CSCFilterType::PointType           PointType;
  typedef typename MomentCalculatorType::VectorType   VectorType;
  typedef typename StructuringElementType::RadiusType	RadiusType;
  typedef itk::ImageRegionIterator<
    ImageType>                                        OutputIteratorType;
  typedef typename  
    ErodeFilterType::DefaultBoundaryConditionType     BoundaryConditionType;

  /** Instantiate filters */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = ReaderType::New();
  typename DistanceMapFilterType::Pointer distanceMapFilter =
    DistanceMapFilterType::New();
  typename ErodeFilterType::Pointer erosion = ErodeFilterType::New();
  typename SubtracterType::Pointer subtracter = SubtracterType::New();
  typename MultiplierType::Pointer multiplier = MultiplierType::New();
  typename CSCFilterType::Pointer cscFilter1 = CSCFilterType::New();
  typename CSCFilterType::Pointer cscFilter2 = CSCFilterType::New();
  typename MomentCalculatorType::Pointer momentCalculator = 
    MomentCalculatorType::New();
  typename AccumulatorType::Pointer accumulator1 = AccumulatorType::New();
  typename AccumulatorType::Pointer accumulator2 = AccumulatorType::New();
  typename DividerType::Pointer divider = DividerType::New();
  typename ExtracterType::Pointer extracter = ExtracterType::New();
  typename WriterType::Pointer writer = WriterType::New();

   /** Read in the inputImages. */
	reader1->SetFileName( inputFileName1.c_str() );
  reader2->SetFileName( inputFileName2.c_str() );
	std::cout << "Reading input images..." << std::endl;		
	reader1->Update();
  reader2->Update();
	std::cout << "Input images read." << std::endl;

  /** Compute the distance map of image 1 */
  distanceMapFilter->SetInput( reader1->GetOutput() );
  distanceMapFilter->SetUseImageSpacing( true );
  distanceMapFilter->SetSquaredDistance( false );
  std::cout << "Computing distance map of input image 1..." << std::endl;
  distanceMapFilter->Update();
  std::cout << "Distance map computed." << std::endl;

  /** Compute the G = edge image of image 2 */
 	RadiusType	radiusarray;
	StructuringElementType	S_ball;
  radiusarray.Fill( 1 );
  /** Create and fill the radius. */
	radiusarray.Fill( 1 );
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		radiusarray.SetElement( i, 1 );
	}
 	/** Create the structuring element and set it into the erosion filter. */
	S_ball.SetRadius( radiusarray );
	S_ball.CreateStructuringElement();
	erosion->SetKernel( S_ball );
  erosion->SetInput( reader2->GetOutput() );
  /** Set a boundary condition value. This is the value outside the image.
   * use zero. */
  BoundaryConditionType bc;
  InputPixelType bcValue = itk::NumericTraits<InputPixelType>::Zero;
  bc.SetConstant( bcValue );
    erosion->OverrideBoundaryCondition(&bc);
  std::cout << "Computing erosion of input image 2..." << std::endl;
  erosion->Update();
  std::cout << "Erosion computed." << std::endl;

  /** compute difference of original and erosion */
  subtracter->SetInput1( reader2->GetOutput() );
  subtracter->SetInput2( erosion->GetOutput() );
  std::cout << "Computing edge image E of input image 2..." << std::endl;
  subtracter->Update();
  std::cout << "Edge image E computed." << std::endl;
  
  /** Computing DE = DistanceMap*EdgeImage */
	multiplier->SetInput1( distanceMapFilter->GetOutput() );
	multiplier->SetInput2( subtracter->GetOutput() );
	std::cout << "Computing DE = DistanceMap * Edge image..." << std::endl;
	multiplier->Update();
	std::cout << "Multiplying done." << std::endl;
  
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
    momentCalculator->SetImage( reader1->GetOutput() );
    std::cout << "Computing center of mass of image 1..." << std::endl;
    momentCalculator->Compute();
    VectorType corvec = momentCalculator->GetCenterOfGravity();
    std::cout << "Center of mass computed. Result: " << corvec << std::endl;
    // convert to point
    for ( unsigned int i=0; i< Dimension; ++i)
    {
      cor[i]=corvec[i];
    }
  }
  
  /** Computing spherical transforms */
  RTPSizeType rtpSize;
  rtpSize[0] = 2;
  rtpSize[1] = thetasize;
  rtpSize[2] = phisize;
  cscFilter1->SetInput( multiplier->GetOutput() );
  cscFilter1->SetOutputSize( rtpSize);
  cscFilter1->SetCenterOfRotation( cor );
  cscFilter1->SetNumberOfSamplesPerVoxel(samples);
  cscFilter2->SetInput( subtracter->GetOutput() );
  cscFilter2->SetOutputSize( rtpSize);
  cscFilter2->SetCenterOfRotation( cor );
  cscFilter2->SetNumberOfSamplesPerVoxel(samples);
  std::cout << "Computing spherical transforms of DE and E..." << std::endl;   
  cscFilter1->GetRandomGenerator()->SetSeed(12345);
  cscFilter1->Update();
  cscFilter2->GetRandomGenerator()->SetSeed(12345);
  cscFilter2->Update();
  std::cout << "Spherical transforms computed." << std::endl;

  /** Integrate along r dimension */
  accumulator1->SetInput( cscFilter1->GetOutput() );
  accumulator1->SetAccumulateDimension(0);
  accumulator1->SetAverage(false);
  accumulator2->SetInput( cscFilter2->GetOutput() );
  accumulator2->SetAccumulateDimension(0);
  accumulator2->SetAverage(false);
  std::cout << "Integrate along r dimension of the spherical transforms..." << std::endl;
  accumulator1->Update();
  accumulator2->Update();
  std::cout << "Done integrating." << std::endl;

  typename ImageType::Pointer accum2out = accumulator2->GetOutput();
  accum2out->DisconnectPipeline();
  OutputIteratorType it( accum2out,
    accum2out->GetLargestPossibleRegion() );
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
  divider->SetInput1( accumulator1->GetOutput() );
	divider->SetInput2( accum2out );
	std::cout << "Dividing the integrated spherical transforms..." << std::endl;
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

} // end DetectGoldMarkers


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "This program computes a spatial segmentation error map.\n" << std::endl;
	std::cout << "Usage:" << std::endl << "pxsegmentationdistance" << std::endl;
	std::cout << "\t-in   \tinputFilename1 inputFileName2" << std::endl;
	std::cout << "\t[-out]\toutputFilename, default <in1>DISTANCE<in2>.mhd" << std::endl;
  std::cout << "\t[-c]  \tCenter of rotation, used to compute the spherical transform. In world coordinates." << std::endl;
	std::cout << "\t[-s]  \tsamples [unsigned int]; number of samples per pixel, used to do the spherical transform; default 20." << std::endl;
	std::cout << "\t[-t]  \ttheta size; the size of the theta dimension. default: 180, which yields a spacing of 2 degrees." << std::endl;
  std::cout << "\t[-p]  \tphi size; the size of the phi dimension. default: 90, which yields a spacing of 2 degrees." << std::endl;
	std::cout << "Supported: 3D, short, and everything convertable to short." << std::endl;
} // end PrintHelp


