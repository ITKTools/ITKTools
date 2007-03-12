#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <itksys/SystemTools.hxx>
#include "itkImage.h"
#include "itkConstrainedValueAdditionImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <map>

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,typeout,dim) \
if ( outputComponentType == #typeout && inputDimension1 == dim ) \
{ \
    function< typeout, dim >( inputFileNames[ 0 ], inputFileNames[ 1 ], outputFileName ); \
}

//-------------------------------------------------------------------------------------

/** Declare ComputeAdditionImage. */
template< class OutputPixelType, unsigned int Dimension >
void ComputeAdditionImageScalar( std::string inputFileName1,
  std::string inputFileName2, std::string outputFileName );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 4 || argc > 8 )
	{
		PrintHelp();
		return 1;
	}

  /** TASK 1:
	 * Get the command line arguments.
	 * *******************************************************************
	 */

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get input file names. */
  std::vector<std::string> inputFileNames;
	bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if ( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify two input file names." << std::endl;
		return 1;
  }

  /** Get the output file name. */
  std::string	outputFileName = 
    itksys::SystemTools::GetFilenameWithoutExtension( inputFileNames[ 0 ] );
	outputFileName += "PLUS";
  outputFileName += itksys::SystemTools::GetFilenameWithoutExtension( inputFileNames[ 1 ] );
  outputFileName += ".mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get the outputComponentType. */
  std::string	outputComponentType = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", outputComponentType );

  /** Determine image properties of image 1. */
  std::string inputComponentType1 = "";
  std::string	inputPixelType1 = "";
  unsigned int inputDimension1 = 2;
  unsigned int numberOfComponents1 = 1;
  std::vector<unsigned int> imagesize1( inputDimension1, 0 );
  int retgip1 = GetImageProperties(
    inputFileNames[ 0 ],
    inputPixelType1,
    inputComponentType1,
    inputDimension1,
    numberOfComponents1,
    imagesize1 );
  if ( retgip1 ) return retgip1;

  /** Determine image properties of image 2. */
  std::string inputComponentType2 = "";
  std::string	inputPixelType2 = "";
  unsigned int inputDimension2 = 2;
  unsigned int numberOfComponents2 = 1;
  std::vector<unsigned int> imagesize2( inputDimension2, 0 );
  int retgip2 = GetImageProperties(
    inputFileNames[ 1 ],
    inputPixelType2,
    inputComponentType2,
    inputDimension2,
    numberOfComponents2,
    imagesize2 );
  if ( retgip2 ) return retgip2;

  /** TASK 2:
	 * Do some preparations and checks.
	 * *******************************************************************
	 */

	/** Check dimension equality. */
	if ( inputDimension1 != inputDimension2 )
	{
		std::cerr << "The dimensions of the input images are "
			<< inputDimension1 << " and "
			<< inputDimension2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

	/** Check equality of the pixel type. */
	if ( inputPixelType1 != inputPixelType2 )
	{
		std::cerr << "The pixel type of the input images are "
			<< inputPixelType1 << " and "
			<< inputPixelType2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

	/** Check equality of the number of components. */
	if ( numberOfComponents1 != numberOfComponents2 )
	{
		std::cerr << "The number of components of the input images are "
			<< numberOfComponents1 << " and "
			<< numberOfComponents2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
		return 1;
	}

  /** Check the outputComponentType.
   * If it is not given in the command line, then:
   * - if in1 and in2 are of the same type, out = in1
   * - if one is larger, take the larger one, where the ranking
   *   is specified using a map.
   * After this the inputComponentType? are invalid.
   */
  typedef std::map< std::string, unsigned int > RankingType;
  typedef RankingType::value_type					      EntryType;
  RankingType ranking;
  ranking.insert( EntryType( "char",   1 ) );
  ranking.insert( EntryType( "short",  2 ) );
  ranking.insert( EntryType( "int",    3 ) );
  ranking.insert( EntryType( "long",   4 ) );
  ranking.insert( EntryType( "float",  5 ) );
  ranking.insert( EntryType( "double", 6 ) );
	if ( outputComponentType == "" )
	{
    RemoveUnsignedFromString( inputComponentType1 );
    RemoveUnsignedFromString( inputComponentType2 );
    if ( inputComponentType1 == inputComponentType2 )
    {
      outputComponentType = inputComponentType1;
    }
    else
    {
      outputComponentType = ranking[ inputComponentType1 ] > ranking[ inputComponentType2 ]
        ? inputComponentType1 : inputComponentType2;
    }
	}

  /** TASK 3:
	 * Run the program.
	 * *******************************************************************
	 */

	try
	{
    /**
		 * ****************** Support for SCALAR pixel types. **********************************
		 */
		if ( strcmp( inputPixelType1.c_str(), "scalar" ) == 0 && numberOfComponents1 == 1 )
    {
      run(ComputeAdditionImageScalar,unsigned char,2);
      run(ComputeAdditionImageScalar,char,2);
      run(ComputeAdditionImageScalar,unsigned short,2);
      run(ComputeAdditionImageScalar,short,2);
      run(ComputeAdditionImageScalar,int,2);
      run(ComputeAdditionImageScalar,unsigned int,2);
      run(ComputeAdditionImageScalar,long,2);
      run(ComputeAdditionImageScalar,unsigned long,2);
      run(ComputeAdditionImageScalar,float,2);
      run(ComputeAdditionImageScalar,double,2);

      run(ComputeAdditionImageScalar,unsigned char,3);
      run(ComputeAdditionImageScalar,char,3);
      run(ComputeAdditionImageScalar,unsigned short,3);
      run(ComputeAdditionImageScalar,short,3);
      run(ComputeAdditionImageScalar,int,3);
      run(ComputeAdditionImageScalar,unsigned int,3);
      run(ComputeAdditionImageScalar,long,3);
      run(ComputeAdditionImageScalar,unsigned long,3);
      run(ComputeAdditionImageScalar,float,3);
      run(ComputeAdditionImageScalar,double,3);

      run(ComputeAdditionImageScalar,unsigned char,4);
      run(ComputeAdditionImageScalar,char,4);
      run(ComputeAdditionImageScalar,unsigned short,4);
      run(ComputeAdditionImageScalar,short,4);
      run(ComputeAdditionImageScalar,int,4);
      run(ComputeAdditionImageScalar,unsigned int,4);
      run(ComputeAdditionImageScalar,long,4);
      run(ComputeAdditionImageScalar,unsigned long,4);
      run(ComputeAdditionImageScalar,float,4);
      run(ComputeAdditionImageScalar,double,4);
    } // end scalar support
    /**
		 * ****************** Support for VECTOR pixel types. **********************************
		 *
		else if ( numberOfComponents1 > 1 )
    {
    } // end vector support*/
    else
		{
			std::cerr << "Pixel types are " << inputPixelType1
				<< ", component types are " << inputComponentType1
				<< " and number of components equals " << numberOfComponents1 << "." << std::endl;
			std::cerr << "ERROR: This image type is not supported." << std::endl;
			return 1;
		} // end support
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main()


	/**
	 * ******************* ComputeAdditionImageScalar *******************
	 *
	 * The ComputeAdditionImage function templated over the output pixel type.
	 */

template< class OutputPixelType, unsigned int Dimension >
void ComputeAdditionImageScalar( std::string inputFileName1,
  std::string inputFileName2, std::string outputFileName )
{
	/** Typedefs. */
  typedef itk::Image< double, Dimension >               DoubleImageType;
  typedef itk::Image< OutputPixelType, Dimension >      OutputImageType;
	typedef itk::ImageFileReader< DoubleImageType >			  ReaderType;
	typedef itk::ImageFileWriter< OutputImageType >			  WriterType;
  typedef itk::ConstrainedValueAdditionImageFilter<
    DoubleImageType, DoubleImageType, OutputImageType > AdditionFilterType;

	/** Declarations. */
	typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();
  typename AdditionFilterType::Pointer additionFilter = AdditionFilterType::New();

	/** Setup and execute the readers. */
	reader1->SetFileName( inputFileName1.c_str() );
  reader2->SetFileName( inputFileName2.c_str() );
	reader1->Update();
	reader2->Update();

  /** The sizes of the image1 and image2 must match. */
	typename DoubleImageType::SizeType size1
    = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();
	typename DoubleImageType::SizeType size2
    = reader2->GetOutput()->GetLargestPossibleRegion().GetSize();
	if ( size1 != size2 )
	{
		std::cerr << "The size of the two images are "
			<< size1 << " and "
			<< size2 << "." << std::endl;
		std::cerr << "They should match!" << std::endl;
	}

  /** Setup the difference filter. */
  additionFilter->SetInput1( reader1->GetOutput() );
  additionFilter->SetInput2( reader2->GetOutput() );

	/** Write the output image. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( additionFilter->GetOutput() );
	writer->Update();

} // end ComputeAdditionImage()


	/**
	 * ******************* PrintHelp *******************
	 */

void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxcomputeadditionimage" << std::endl;
	std::cout << "  -in      inputFilename1 inputFilename2" << std::endl;
	std::cout << "  [-out]   outputFilename, default in1PLUSin2.mhd" << std::endl;
	std::cout << "  [-opct]  output pixelType, default largest of in1 and in2" << std::endl;
	std::cout << "Supported: 2D, 3D, 4D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;
} // end PrintHelp()


