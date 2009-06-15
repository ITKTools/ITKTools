#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkCylinderSpatialFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, dim ) \
if ( Dimension == dim ) \
{ \
  function< dim >( inputFileName, outputFileName, center, radius ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CreateCylinder. */
template< unsigned int Dimension >
void CreateCylinder(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & center,
  const double & radius );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check arguments for help. */
  if ( argc < 5 )
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

  std::string outputFileName = "";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> center;
  bool retc = parser->GetCommandLineArgument( "-c", center );

  double radius;
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( !retout )
  {
    std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
    return 1;
  }
  if ( !retc )
  {
    std::cerr << "ERROR: You should specify \"-c\"." << std::endl;
    return 1;
  }
  if ( !retr )
  {
    std::cerr << "ERROR: You should specify \"-r\"." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
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
  if ( retgip != 0 ) return 1;

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CreateCylinder, 2 );
    run( CreateCylinder, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main 

/**
 * ******************* CreateCylinder *******************
 */

template< unsigned int Dimension >
void CreateCylinder(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & center,
  const double & radius )
{
  /** Typedefs. */
  typedef float               InputPixelType;
  typedef unsigned char       OutputPixelType;
  typedef itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;
  typedef itk::ImageRegionIteratorWithIndex< OutputImageType >  IteratorType;
  typedef itk::CylinderSpatialFunction< Dimension > CylinderSpatialFunctionType;
  typedef typename CylinderSpatialFunctionType::InputType    InputType;
  typedef typename OutputImageType::IndexType   IndexType;
  typedef typename OutputImageType::PointType   PointType;

  /** Create a testReader. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( inputFileName.c_str() );
  testReader->Update();

  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->CopyInformation( testReader->GetOutput() );
  outputImage->SetRegions( outputImage->GetLargestPossibleRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer( 0 );

  /** Parse the arguments. */
  InputType   Center;
  PointType point;
  IndexType index;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    index[ i ] = center[ i ];
  }
  outputImage->TransformIndexToPhysicalPoint( index, point );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Center[ i ] = point[ i ];
  }

  /** Create and initialize ellipsoid. */
  typename CylinderSpatialFunctionType::Pointer cylinder
    = CylinderSpatialFunctionType::New();
  cylinder->SetCenter( Center );
  cylinder->SetRadius( radius );

  /** Create iterator, index and point. */
  IteratorType it( outputImage, outputImage->GetLargestPossibleRegion() );
  it.GoToBegin();

  /** Walk over the image. */
  while ( !it.IsAtEnd() )
  {
    index = it.GetIndex();
    outputImage->TransformIndexToPhysicalPoint( index, point );
    it.Set( cylinder->Evaluate( point ) );
    ++it;
  }

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( outputImage );
  writer->Update();

} // end CreateCylinder()


/**
 * ******************* PrintHelp *******************
 */

void PrintHelp( void )
{
  std::cout << "Usage:" << std::endl << "pxcreatecylinder" << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  -out     outputFilename" << std::endl;
  std::cout << "  -c       center (mm)" << std::endl;
  std::cout << "  -r       radii (mm)" << std::endl;
  std::cout << "Supported: 2D, 3D." << std::endl;

} // end PrintHelp()

