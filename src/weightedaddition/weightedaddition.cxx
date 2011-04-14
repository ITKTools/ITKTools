
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNaryAddImageFilter.h"
#include "itkMultiplyImageFilter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef  itk::Image< type, dim >   InputImageType; \
  function< InputImageType >( inputFileNames, weightFileNames, outputFileName ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare WeightedAddition. */
template< class InputImageType >
void WeightedAddition(
  const std::vector<std::string> & inputFileNames,
  const std::vector<std::string> & weightFileNames,
  const std::string & outputFileName
  );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check number of arguments. */
  if ( argc < 6 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::vector<std::string> inputFileNames;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

  /** Get arguments. */
  std::vector<std::string> weightFileNames;
  bool retw = parser->GetCommandLineArgument( "-w", weightFileNames );
  if ( !retw )
  {
    std::cerr << "ERROR: You should specify \"-w\"." << std::endl;
    return 1;
  }

  std::string outputFileName("");
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Determine input image properties. */
  std::string ComponentType = "float";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileNames[0],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Anyway, float is only supported. */
  ComponentType = "float";
  // bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

  /** Error checking. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }


  /** Run the program. */
  bool supported = false;
  try
  {
    run( WeightedAddition, float, 2 );
    run( WeightedAddition, float, 3 );
  }
  catch( itk::ExceptionObject &e )
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

} // end main()


/**
 * ******************* WeightedAddition *******************
 */

template< class InputImageType >
void WeightedAddition(
  const std::vector<std::string> & inputFileNames,
  const std::vector<std::string> & weightFileNames,
  const std::string & outputFileName )
{
  /** TYPEDEF's. */
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::MultiplyImageFilter<
    InputImageType, InputImageType, InputImageType>     MultiplierType;
  typedef itk::NaryAddImageFilter<
    InputImageType, InputImageType >                    AdderType;
  typedef itk::ImageFileWriter< InputImageType >        WriterType;
  typedef typename InputImageType::PixelType            PixelType;
  typedef typename ReaderType::Pointer                  ReaderPointer;
  typedef typename MultiplierType::Pointer              MultiplierPointer;
  typedef typename AdderType::Pointer                   AdderPointer;
  typedef typename WriterType::Pointer                  WriterPointer;

  /** DECLARATION'S. */
  unsigned int nrInputs = inputFileNames.size();
  if ( weightFileNames.size() != nrInputs )
  {
    itkGenericExceptionMacro( << "ERROR: Number of weight images does not equal number of input images!" );
  }

  std::vector< ReaderPointer > inReaders( nrInputs );
  std::vector< ReaderPointer > wReaders( nrInputs );
  std::vector< MultiplierPointer > multipliers( nrInputs );
  AdderPointer adder = AdderType::New();
  WriterPointer writer = WriterType::New();

  for ( unsigned int i = 0; i < nrInputs; ++i )
  {
    inReaders[i] = ReaderType::New();
    inReaders[i]->SetFileName( inputFileNames[i].c_str() );
    wReaders[i] = ReaderType::New();
    wReaders[i]->SetFileName( weightFileNames[i].c_str() );
    multipliers[i] = MultiplierType::New();
    multipliers[i]->SetInput(0, inReaders[i]->GetOutput() );
    multipliers[i]->SetInput(1, wReaders[i]->GetOutput() );
    multipliers[i]->InPlaceOn();
    adder->SetInput(i, multipliers[i]->GetOutput() );
  }

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( adder->GetOutput() );
  writer->Update();

} // end WeightedAddition()


/**
 * ******************* PrintHelp *******************
 */
void PrintHelp()
{
  std::cout << "Usage:" << std::endl << "pxweightedaddition" << std::endl;
  std::cout << "  -in      inputFilenames" << std::endl;
  std::cout << "  -w       weightFilenames" << std::endl;
  std::cout << "  -out     outputFilename; always written as float" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char, float." << std::endl;

} // end PrintHelp()
