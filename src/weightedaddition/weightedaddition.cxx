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

/**
 * ******************* GetHelpString *******************
 */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxweightedaddition" << std::endl
  << "  -in      inputFilenames" << std::endl
  << "  -w       weightFilenames" << std::endl
  << "  -out     outputFilename; always written as float" << std::endl
  << "Supported: 2D, 3D, (unsigned) short, (unsigned) char, float.";

  return ss.str();

} // end GetHelpString()

/** WeightedAddition */

class WeightedAdditionBase : public itktools::ITKToolsBase
{ 
public:
  WeightedAdditionBase(){};
  ~WeightedAdditionBase(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::vector<std::string> m_WeightFileNames;
  std::string m_OutputFileName;

}; // end WeightedAdditionBase


template< class TComponentType, unsigned int VDimension >
class WeightedAddition : public WeightedAdditionBase
{
public:
  typedef WeightedAddition Self;

  WeightedAddition(){};
  ~WeightedAddition(){};

  static Self * New( itktools::EnumComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** TYPEDEF's. */
    typedef itk::Image<TComponentType, VDimension>        InputImageType;
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
    unsigned int nrInputs = m_InputFileNames.size();
    if ( m_WeightFileNames.size() != nrInputs )
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
      inReaders[i]->SetFileName( m_InputFileNames[i].c_str() );
      wReaders[i] = ReaderType::New();
      wReaders[i]->SetFileName( m_WeightFileNames[i].c_str() );
      multipliers[i] = MultiplierType::New();
      multipliers[i]->SetInput(0, inReaders[i]->GetOutput() );
      multipliers[i]->SetInput(1, wReaders[i]->GetOutput() );
      multipliers[i]->InPlaceOn();
      adder->SetInput(i, multipliers[i]->GetOutput() );
    }

    /** Write the output image. */
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( adder->GetOutput() );
    writer->Update();
  }

}; // end WeightedAddition

//-------------------------------------------------------------------------------------

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

  /** Class that does the work */
  WeightedAdditionBase * weightedAddition = NULL;

  /** Short alias */
  unsigned int dim = Dimension;
 
  itktools::EnumComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!weightedAddition) weightedAddition = WeightedAddition< float, 2 >::New( componentType, dim );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!weightedAddition) weightedAddition = WeightedAddition< float, 3 >::New( componentType, dim );    
#endif
    if (!weightedAddition) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    weightedAddition->m_InputFileNames = inputFileNames;
    weightedAddition->m_WeightFileNames = weightFileNames;
    weightedAddition->m_OutputFileName = outputFileName;
  
    weightedAddition->Run();
    
    delete weightedAddition;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete weightedAddition;
    return 1;
  }

  /** End program. */
  return 0;

} // end main()
