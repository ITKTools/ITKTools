#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "vnl/vnl_math.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ComputeBoundingBox. */
template< class InputImageType >
void ComputeBoundingBox( std::string inputFileName );

/** Declare other functions. */
void PrintHelp( void );


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 3 || argc > 7 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );
	
	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  
  /** Let the user overrule this */
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );
	bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if (retdim | retpt)
  {
    std::cout << "The user has overruled this by specifying -pt and/or -dim:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if (NumberOfComponents > 1)
  { 
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1; 
  }
 
	/** Get rid of the possible "_" in ComponentType. */
	ReplaceUnderscoreWithSpace( ComponentType );

  /** Overrule it, since only short will do something */
  if ( ComponentType != "short" )
  {
    /** Try short anyway, but warn user */
    ComponentType = "short";
    std::cout << "WARNING: the image will be converted to short!" << std::endl;
  }
	/** Run the program. */
  bool supported = false;
	try
	{
		run(ComputeBoundingBox, short, 3);
    run(ComputeBoundingBox, short, 2);
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
	 * ******************* ComputeBoundingBox *******************
	 */

template< class InputImageType >
void ComputeBoundingBox( std::string inputFileName )
{
	/** Typedefs. */
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
  typedef itk::ImageRegionConstIteratorWithIndex<
    InputImageType>                                   IteratorType;  
  typedef typename InputImageType::PixelType          PixelType;
  typedef typename InputImageType::IndexType          IndexType;
  typedef typename InputImageType::PointType          PointType;
  const unsigned int dimension = InputImageType::GetImageDimension();
	
  /** Declarations */
	typename ReaderType::Pointer reader = ReaderType::New();
  typename InputImageType::Pointer image;
  IndexType minIndex;
  IndexType maxIndex;
    
  /** Read input image */
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();
  image = reader->GetOutput();

  /** Define iterator on input image */
  IteratorType iterator( image, image->GetLargestPossibleRegion() );
  
  /** Initialize the two corner points */
  iterator.GoToReverseBegin();
  minIndex = iterator.GetIndex();
  iterator.GoToBegin();
  maxIndex = iterator.GetIndex();
  PixelType zero = itk::NumericTraits< PixelType>::Zero;
  
  while ( ! iterator.IsAtEnd() )
  {
    if ( iterator.Get() > zero )
    {
      const IndexType & index = iterator.GetIndex();
      for ( unsigned int i = 0; i < dimension; ++i)
      {
        minIndex[i] = vnl_math_min( index[i], minIndex[i] );
        maxIndex[i] = vnl_math_max( index[i], maxIndex[i] );
      }
    }
    ++iterator;
  }

  PointType minPoint;
  PointType maxPoint;
  image->TransformIndexToPhysicalPoint(minIndex, minPoint);
  image->TransformIndexToPhysicalPoint(maxIndex, maxPoint);

  std::cout << "MinimumIndex = " << minIndex << "\n"
            << "MaximumIndex = " << maxIndex << std::endl;
  std::cout << std::showpoint;
  std::cout << "MinimumPoint = " << minPoint << "\n"
            << "MaximumPoint = " << maxPoint << std::endl;

} // end ComputeBoundingBox


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "This program computes the bounding box of an image." << std::endl;
  std::cout << "Every pixel > 0 is considered to be within the bounding box." << std::endl;
  std::cout << "Returns the minimum and maximum indices/points that lie within the bounding box." << std::endl;
  std::cout << "Usage:" << std::endl << "pxcomputeboundingbox" << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  [-dim]   dimension, default 3" << std::endl;
	std::cout << "  [-pt]    pixelType, default short" << std::endl;
	std::cout << "Supported: 2D, 3D, short. Images with PixelType other than short are automatically converted. " << std::endl;
} // end PrintHelp



