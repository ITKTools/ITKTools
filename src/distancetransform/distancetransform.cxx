
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"

/** Declare DistanceTransform. */
template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::string & outputFileName,
  bool outputSquaredDistance );


/** Declare PrintHelp. */
void PrintHelp(void);


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
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  /** Get whether the squared distance should be returned instead of the distance (default: false) */
  bool outputSquaredDistance = parser->ArgumentExists( "-s" );  

  /** Get the outputFileName */
  std::string outputFileName = "";
  bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
   
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

  /** Possibly overrule the dimension (not mandatory) */  
  bool retd = parser->GetCommandLineArgument( "-d", Dimension );  
  if ( retd )
  {
    std::cout << "Dimension overruled by user: "  << Dimension << std::endl;
  }

  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: vector images are not supported." << std::endl;
    return 1;
  }
  if ( (Dimension!=2) && (Dimension!=3) )
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
    if (Dimension==2)
    {
      DistanceTransform<2>(
        inputFileName,
        outputFileName,
        outputSquaredDistance );
    }
    if (Dimension==3)
    {
      DistanceTransform<3>(
        inputFileName,
        outputFileName,
        outputSquaredDistance );
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

  std::cout << "pxdistancetransform has finished!" << std::endl;
  
  /** End program. */
  return 0;

} // end main



/**
 * ******************* DistanceTransform ****************
 *
 * The function that does the work, templated over the image dimension.
 */

template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::string & outputFileName,
  bool outputSquaredDistance)
{
  const unsigned int              Dimension = NDimensions;
  typedef unsigned char           InputComponentType;
  typedef InputComponentType      InputPixelType;
  typedef float                   OutputComponentType;
  typedef OutputComponentType     OutputPixelType;  
  
  typedef itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;

  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType, OutputImageType >               DistanceTransformFilterType;
  
  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename OutputImageType::Pointer         OutputImagePointer;
   
  typedef itk::ImageFileReader< InputImageType >    InputImageReaderType;
  typedef itk::ImageFileWriter< OutputImageType >   OutputImageWriterType;
  
  /** Declare/initialize some variables */
  OutputImagePointer outputImage = 0;
  DistanceTransformFilterType::Pointer distanceTransform =
    DistanceTransformFilterType::New();
       
  /** Read the input images */
  std::cout << "Reading input image..." << std::endl;
  typename InputImageReaderType::Pointer inputImageReader =
    InputImageReaderType::New();
  inputImageReader->SetFileName( inputFileName.c_str() );
  inputImageReader->Update();
  std::cout << "Done reading input image." << std::endl; 
      
  /** Setup the distance transform filter */
  distanceTransform->SetInput( inputImageReader->GetOutput() );    
  distanceTransform->SetUseImageSpacing( true );
  distanceTransform->SetInsideIsPositive( false );
  distanceTransform->SetSquaredDistance( outputSquaredDistance );
  distanceTransform->SetBackgroundValue( 0 );
      
  /** Run!! */
  std::cout << "Performing distance transform algorithm..." << std::endl;
  distanceTransform->Update();
  std::cout << "Done performing distance transform algorithm." << std::endl;
    
  /** Write result output */
  std::cout << "Writing output image ..." << std::endl;  
  outputImage = distanceTransform->GetOutput();
  if ( ( outputFileName != "" ) && outputImage.IsNotNull() )
  {    
    typename OutputImageWriterType::Pointer outputImageWriter = 
      OutputImageWriterType::New();
    outputImageWriter->SetFileName( outputFileName.c_str() );    
    outputImageWriter->SetInput( outputImage );    
    outputImageWriter->Update();
  }
  else
  {
    itkGenericExceptionMacro( "ERROR: Output image could not be written." );
  }
  std::cout << "Done writing output image." << std::endl;      
 
} // end DistanceTransform



/**
 * ******************* PrintHelp *******************
 */

void PrintHelp()
{
  std::cout << "This program creates a signed distance transform.\n" << std::endl;
  std::cout << "Usage:\npxdistancetransform" << std::endl;
  std::cout << "  -in      inputFilename0: the input image (a binary mask; threshold at 0 is performed if the image is not binary)." << std::endl;
  std::cout << "  -out     outputFilename: the output of distance transform" << std::endl;
  std::cout << "  [-s]     flag: if set, output squared distances instead of distances" << std::endl;
  std::cout << "Note: voxel spacing is taken into account. Voxels inside the object (=1) receive a negative distance." << std::endl;
  std::cout << "Supported: 2D/3D. input: unsigned char, output: float" << std::endl;
} // end PrintHelp


