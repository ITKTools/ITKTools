#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkImageToVectorImageFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( componentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim >  OutputImageType; \
  if ( retmag ) \
  { \
    function##Magnitude< OutputImageType >( inputFileName, outputFileName, sigma, order ); \
  } \
  if ( retlap ) \
  { \
    function##Laplacian< OutputImageType >( inputFileName, outputFileName, sigma ); \
  } \
  if ( !retmag && !retlap ) \
  { \
    function< OutputImageType >( inputFileName, outputFileName, sigma, order ); \
  } \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare GaussianImageFilter. */
template< class OutputImageType >
void GaussianImageFilter(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order );

template< class OutputImageType >
void GaussianImageFilterMagnitude(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order );

template< class OutputImageType >
void GaussianImageFilterLaplacian(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
 	/** Check arguments for help. */
	if ( argc < 3 || argc > 15 )
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

  std::vector<float> sigma;
  sigma.push_back(1.0); // default 1.0 for each resolution 
  bool retstd = parser->GetCommandLineArgument( "-std", sigma );

  std::vector<unsigned int> order;
  bool retord = parser->GetCommandLineArgument( "-ord", order );

  std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "BLURRED.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	bool retmag = parser->ArgumentExists( "-mag" );
  bool retlap = parser->ArgumentExists( "-lap" );

  std::string componentType = "";
	bool retpt = parser->GetCommandLineArgument( "-pt", componentType );

 	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  /** Check options. */
  for ( unsigned int i = 0; i < order.size(); ++i )
  {
    if ( order[ i ] > 2 )
    {
      std::cerr << "ERROR: The order should not be higher than 2." << std::endl;
      std::cerr << "Only zeroth, first and second order derivatives are supported." << std::endl;
      return 1;
    }
  }

  /** Check that not both mag and lap are given. */
  if ( retmag && retlap )
  {
    std::cerr << "ERROR: only one of \"-mag\" and \"-lap\" should be given!" << std::endl;
    return 1;
  }
 
  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 3;
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

  /** The default output is equal to the input, but can be overridden by
   * specifying -pt in the command line.
   */
  if ( !retpt ) componentType = ComponentTypeIn;

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1; 
  }

	/** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check order. */
  if ( order.size() != Dimension )
  {
    std::cerr << "ERROR: the # of orders should be equal to the image dimension!" << std::endl;
    return 1; 
  }

  /** Check sigma. */
  if ( sigma.size() != 1 && sigma.size() != Dimension )
  {
    std::cerr << "ERROR: the # of sigmas should be equal to 1 or the image dimension!" << std::endl;
    return 1; 
  }

  /** Run the program. */
  bool supported = false;
	try
	{
    run( GaussianImageFilter, char, 2 );
		run( GaussianImageFilter, unsigned char, 2 );
    run( GaussianImageFilter, short, 2 );
		run( GaussianImageFilter, unsigned short, 2 );
    run( GaussianImageFilter, int, 2 );
		run( GaussianImageFilter, unsigned int, 2 );
    run( GaussianImageFilter, long, 2 );
		run( GaussianImageFilter, unsigned long, 2 );
    run( GaussianImageFilter, float, 2 );
		run( GaussianImageFilter, double, 2 );

    run( GaussianImageFilter, char, 3 );
		run( GaussianImageFilter, unsigned char, 3 );
    run( GaussianImageFilter, short, 3 );
		run( GaussianImageFilter, unsigned short, 3 );
    run( GaussianImageFilter, int, 3 );
		run( GaussianImageFilter, unsigned int, 3 );
    run( GaussianImageFilter, long, 3 );
		run( GaussianImageFilter, unsigned long, 3 );
    run( GaussianImageFilter, float, 3 );
		run( GaussianImageFilter, double, 3 );

	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr <<
      "pixel (component) type = " << ComponentTypeIn <<
      " ; dimension = " << Dimension 
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ******************* GaussianImageFilter *******************
	 */

template< class OutputImageType >
void GaussianImageFilter(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, OutputImageType >                     FilterType;
  typedef typename FilterType::OrderType                  OrderType;
  typedef typename FilterType::SigmaType                  SigmaType;
	typedef itk::ImageFileWriter< OutputImageType >			    WriterType;

	/**	Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup the order and sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma[ 0 ] );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    orderFA[ i ] = order[ i ];
    if ( sigma.size() == Dimension ) sigmaFA[ i ] = sigma[ i ];
  }

  /** Setup the smoothing filter. */
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNormalizeAcrossScale( false );
  filter->SetInput( reader->GetOutput() );
  filter->SetSigma( sigmaFA );
  filter->SetOrder( orderFA );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName );
	writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end GaussianImageFilter()


  /**
	 * ******************* GaussianImageFilterMagnitude *******************
	 */

template< class OutputImageType >
void GaussianImageFilterMagnitude(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, InputImageType >                      SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;
  typedef typename SmoothingFilterType::OrderType         OrderType;
  typedef typename SmoothingFilterType::SigmaType         SigmaType;
  typedef itk::ImageToVectorImageFilter<
    InputImageType >                                      ImageToVectorImageFilterType;
  typedef typename ImageToVectorImageFilterType
    ::OutputImageType                                     VectorImageType;
  typedef itk::GradientToMagnitudeImageFilter<
    VectorImageType, OutputImageType >                    MagnitudeFilterType;
	typedef itk::ImageFileWriter< OutputImageType >			    WriterType;

	/**	Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup the order and sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma[ 0 ] );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    orderFA[ i ] = order[ i ];
    if ( sigma.size() == Dimension ) sigmaFA[ i ] = sigma[ i ];
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( Dimension );
  typename ImageToVectorImageFilterType::Pointer composeFilter
    = ImageToVectorImageFilterType::New();
  typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    /** Setup smoothing filter. */
    smoothingFilter[ i ] = SmoothingFilterType::New();
    smoothingFilter[ i ]->SetInput( reader->GetOutput() );
    smoothingFilter[ i ]->SetNormalizeAcrossScale( false );
    OrderType order2; order2.Fill( 0 ); order2[ i ] = orderFA[ i ];
    smoothingFilter[ i ]->SetSigma( sigmaFA );
    smoothingFilter[ i ]->SetOrder( order2 );
    smoothingFilter[ i ]->Update();

    /** Setup composition filter. */
    composeFilter->SetNthInput( i, smoothingFilter[ i ]->GetOutput() );
  }

  /** Compose vector image and compute magnitude. */
  magnitudeFilter->SetInput( composeFilter->GetOutput() );
  magnitudeFilter->Update();

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName );
	writer->SetInput( magnitudeFilter->GetOutput() );
  writer->Update();

} // end GaussianImageFilterMagnitude()


  /**
	 * ******************* GaussianImageFilterLaplacian *******************
	 */

template< class OutputImageType >
void GaussianImageFilterLaplacian(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef typename OutputImageType::PixelType             OutputPixelType;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
	typedef itk::ImageFileReader< InputImageType >			    ReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, InputImageType >                      SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;
  typedef typename SmoothingFilterType::OrderType         OrderType;
  typedef typename SmoothingFilterType::SigmaType         SigmaType;
  typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< OutputImageType >     IteratorType;
	typedef itk::ImageFileWriter< OutputImageType >			    WriterType;

	/**	Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup sigma. */
  SigmaType sigmaFA; sigmaFA.Fill( sigma[ 0 ] );
  if ( sigma.size() == Dimension )
  {
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      sigmaFA[ i ] = sigma[ i ];
    }
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( Dimension );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    /** Setup smoothing filter. */
    smoothingFilter[ i ] = SmoothingFilterType::New();
    smoothingFilter[ i ]->SetInput( reader->GetOutput() );
    smoothingFilter[ i ]->SetNormalizeAcrossScale( false );
    OrderType order; order.Fill( 0 ); order[ i ] = 2;
    smoothingFilter[ i ]->SetSigma( sigmaFA );
    smoothingFilter[ i ]->SetOrder( order );
    smoothingFilter[ i ]->Update();
  }

  /** Create output image. */
  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->CopyInformation( reader->GetOutput() );
  outputImage->SetRegions( reader->GetOutput()->GetLargestPossibleRegion() );
  outputImage->Allocate();

  /** Setup iterators. */
  std::vector< ConstIteratorType > itIn( Dimension );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    itIn[ i ] = ConstIteratorType( smoothingFilter[ i ]->GetOutput(),
      smoothingFilter[ i ]->GetOutput()->GetLargestPossibleRegion() );
    itIn[ i ].GoToBegin();
  }
  IteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );
  itOut.GoToBegin();

  /** Fill the output image by adding the second order derivatives. */
  while ( !itOut.IsAtEnd() )
  {
    InputPixelType value = itk::NumericTraits<InputPixelType>::Zero;
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      value += itIn[ i ].Get();
      ++itIn[ i ];
    }
    itOut.Set( static_cast<OutputPixelType>( value ) );
    ++itOut;
  }

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName );
	writer->SetInput( outputImage );
  writer->Update();

} // end GaussianImageFilterLaplacian()


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxgaussianimagefilter" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
	std::cout << "  [-out]   outputFilename, default in + BLURRED.mhd" << std::endl;
  std::cout << "  [-std]   sigma, for each dimension, default 1.0" << std::endl;
  std::cout << "  [-ord]   order, for each dimension, default zero\n";
  std::cout << "             0: zero order = blurring\n";
  std::cout << "             1: first order = gradient\n";
  std::cout << "             2: second order derivative" << std::endl;
  std::cout << "  [-mag]   compute the magnitude of the separate blurrings, default false" << std::endl;
  std::cout << "  [-lap]   compute the laplacian, default false" << std::endl;
  std::cout << "  [-pt]    output pixel type, default equal to input" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;
} // end PrintHelp()

