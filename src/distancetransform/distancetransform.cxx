
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
//#include "itkOrderKDistanceTransformImageFilter.h"

/** Declare DistanceTransform. */
template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames,
  bool outputSquaredDistance,
  const std::string & method,
  const unsigned int & K );

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

  /** Get the input segmentation file name (mandatory). */
  std::string inputFileName;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get whether the squared distance should be returned instead of the
   * distance. Default: false, which is faster.
   */
  bool outputSquaredDistance = parser->ArgumentExists( "-s" );

  /** Get the outputFileName */
  std::vector<std::string> outputFileNames;
  bool retout = parser->GetCommandLineArgument( "-out", outputFileNames );

  std::string method = "Maurer";
  bool retm = parser->GetCommandLineArgument( "-m", method );

  unsigned int K = 5;
  bool retK = parser->GetCommandLineArgument( "-k", K );

  /** Checks. */
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

  //if ( method != "Maurer" && method != "Danielsson" && method != "OrderK" )
  if ( method != "Maurer" && method != "Danielsson" )
  {
    //std::cerr << "ERROR: the method should be one of {Maurer, Danielsson, OrderK}!"
    std::cerr << "ERROR: the method should be one of { Maurer, Danielsson }!"
      << std::endl;
    return 1;
  }

  if ( method == "OrderK" && outputFileNames.size() != 3 )
  {
    std::cerr << "ERROR: the method OrderK requires three output file names!\n";
    std::cerr << "  You only specified " << outputFileNames.size() << "."
      << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "unsigned char";
  std::string PixelType; //we don't use this
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
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for scalar image. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: vector images are not supported." << std::endl;
    return 1;
  }

  /** Check for dimension. */
  if ( Dimension != 2 && Dimension != 3 )
  {
    std::cerr
      << "ERROR: images of dimension "
      << Dimension
      << " are not supported!"
      << std::endl;
    return 1;
  }

  /** Run the program. */
  try
  {
    if ( Dimension == 2 )
    {
      DistanceTransform<2>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }
    if ( Dimension == 3 )
    {
      DistanceTransform<3>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }

  }
  catch( itk::ExceptionObject & e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  catch( std::exception & e )
  {
    std::cerr << "Caught std::exception: " << e.what() << std::endl;
    return 1;
  }
  catch ( ... )
  {
    std::cerr << "Caught unknown exception" << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main



/**
 * ******************* DistanceTransform ****************
 *
 */

template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames,
  bool outputSquaredDistance,
  const std::string & method,
  const unsigned int & K )
{
  const unsigned int              Dimension = NDimensions;
  typedef unsigned char           InputComponentType;
  typedef InputComponentType      InputPixelType;
  typedef float                   OutputComponentType;
  typedef OutputComponentType     OutputPixelType;

  typedef itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::Image< float, Dimension >            FloatImageType;
  typedef itk::Image< unsigned long, Dimension >    ULImageType;

  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType, OutputImageType >               MaurerDistanceType;
  typedef itk::SignedDanielssonDistanceMapImageFilter<
    InputImageType, OutputImageType >               DanielssonDistanceType;
//   typedef itk::OrderKDistanceTransformImageFilter<
//     FloatImageType, ULImageType >                   OrderKDistanceType;
//
//   typedef typename OrderKDistanceType::OutputImageType    VoronoiMapType;
//   typedef typename OrderKDistanceType::KDistanceImageType KDistanceImageType;
//   typedef typename OrderKDistanceType::KIDImageType       KIDImageType;

  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename OutputImageType::Pointer         OutputImagePointer;

  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  typedef itk::ImageFileReader< FloatImageType >    FloatReaderType;
  typedef itk::ImageFileWriter< OutputImageType >   WriterType;
//   typedef itk::ImageFileWriter< VoronoiMapType >    VoronoiWriterType;
//   typedef itk::ImageFileWriter< KDistanceImageType > KDistanceWriterType;
//   typedef itk::ImageFileWriter< KIDImageType >      KIDWriterType;

  /** Read the input images */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  typename FloatReaderType::Pointer freader = FloatReaderType::New();
  freader->SetFileName( inputFileName.c_str() );

  /** Setup the Maurer distance transform filter. */
  typename MaurerDistanceType::Pointer distance_Maurer
    = MaurerDistanceType::New();
  distance_Maurer->SetInput( reader->GetOutput() );
  distance_Maurer->SetUseImageSpacing( true );
  distance_Maurer->SetInsideIsPositive( false );
  distance_Maurer->SetSquaredDistance( outputSquaredDistance );
  distance_Maurer->SetBackgroundValue( 0 );

  /** Setup the Danielsson distance transform filter. */
  typename DanielssonDistanceType::Pointer distance_Danielsson
    = DanielssonDistanceType::New();
  distance_Danielsson->SetInput( reader->GetOutput() );
  distance_Danielsson->SetUseImageSpacing( true );
  distance_Danielsson->SetInsideIsPositive( false );
  distance_Danielsson->SetSquaredDistance( outputSquaredDistance );

  /** Setup the OrderK distance transform filter. */
//   typename OrderKDistanceType::Pointer distance_OrderK
//     = OrderKDistanceType::New();
//   distance_OrderK->SetInput( freader->GetOutput() );
//   distance_OrderK->SetUseImageSpacing( true );
//   distance_OrderK->SetInputIsBinary( false );
//   distance_OrderK->SetSquaredDistance( outputSquaredDistance );
//   distance_OrderK->SetK( K );

  /** Setup writer. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileNames[ 0 ].c_str() );

//   typename VoronoiWriterType::Pointer voronoiWriter = VoronoiWriterType::New();
//   typename KDistanceWriterType::Pointer kDistanceWriter = KDistanceWriterType::New();
//   typename KIDWriterType::Pointer kIDWriter = KIDWriterType::New();
//   voronoiWriter->SetFileName( outputFileNames[ 0 ].c_str() );
//   kDistanceWriter->SetFileName( outputFileNames[ 1 ].c_str() );
//   kIDWriter->SetFileName( outputFileNames[ 2 ].c_str() );

  /** Run! */
  if ( method == "Maurer" )
  {
    distance_Maurer->Update();
    writer->SetInput( distance_Maurer->GetOutput() );
    writer->Update();
  }
  else if ( method == "Danielsson" )
  {
    distance_Danielsson->Update();
    writer->SetInput( distance_Danielsson->GetOutput() );
    writer->Update();
  }
//   else if ( method == "OrderK" )
//   {
//     std::cerr << "to here";
//     freader->Update();
//     std::cerr << "to here";
//     distance_OrderK->Update();
//     std::cerr << "to here";
//     voronoiWriter->SetInput( distance_OrderK->GetVoronoiMap() );
//     kDistanceWriter->SetInput( distance_OrderK->GetKDistanceMap() );
//     kIDWriter->SetInput( distance_OrderK->GetKclosestIDMap() );
//
//     voronoiWriter->Update();
//     kDistanceWriter->Update();
//     kIDWriter->Update();
//   }

} // end DistanceTransform()


/**
 * ******************* PrintHelp *******************
 */

void PrintHelp( void )
{
  std::cout << "This program creates a signed distance transform.\n\n";
  std::cout << "Usage:\npxdistancetransform\n";
  std::cout << "  -in      inputFilename: the input image (a binary mask; "
    << "threshold at 0 is performed if the image is not binary).\n";
  std::cout << "  -out     outputFilename: the output of distance transform\n";
  std::cout << "  [-s]     flag: if set, output squared distances instead of distances\n";
  //std::cout << "  [-m]     method, one of {Maurer, Danielsson, OrderK}, default Maurer\n";
  std::cout << "  [-m]     method, one of {Maurer, Danielsson}, default Maurer\n";
  //std::cout << "  [-K]     for method \"OrderK\", specify K, default 5\n";
  std::cout << "Note: voxel spacing is taken into account. Voxels inside the "
    << "object (=1) receive a negative distance.\n";
  std::cout << "Supported: 2D/3D. input: unsigned char, output: float" << std::endl;

} // end PrintHelp()
