/** \file
 \brief Add images with weights.
 
 \verbinclude weightedaddition.help
 */
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

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-w", "The weight filename." );

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
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  /** Get arguments. */
  std::vector<std::string> weightFileNames;
  parser->GetCommandLineArgument( "-w", weightFileNames );

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
 * ******************* GetHelpString *******************
 */
std::string GetHelpString()
{
  std::string helpText = "Usage: \
  pxweightedaddition \
    -in      inputFilenames \
    -w       weightFilenames \
    -out     outputFilename; always written as float \
  Supported: 2D, 3D, (unsigned) short, (unsigned) char, float.";
  
  return helpText;

} // end GetHelpString()
