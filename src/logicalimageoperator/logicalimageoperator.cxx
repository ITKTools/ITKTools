#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkNotImageFilter.h"
#include "itkCastImageFilter.h"
#include <map>
#include <utility>
#include <vector>
#include <itksys/SystemTools.hxx>

#include "itkLogicalFunctors.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
    typedef itk::Image< type, dim > InputImageType; \
    function< InputImageType >( inputFileName1, inputFileName2, outputFileName, ops ); \
}

//-------------------------------------------------------------------------------------

/** Declare LogicalImageOperator. */
template< class InputImageType >
void LogicalImageOperator( 
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::string & ops );

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 5 || argc > 12 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::vector< std::string >	inputFileNames;
	bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );
  std::string ops = "AND";
  bool retops = parser->GetCommandLineArgument( "-ops", ops );

  /** Check if the required arguments are given. */
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if ( !retops )
	{
		std::cerr << "ERROR: You should specify \"-ops\"." << std::endl;
		return 1;
	}
  if( (inputFileNames.size() != 2) & (ops != "NOT") & (ops != "NOT_NOT") )
	{
		std::cout << "ERROR: You should specify two input images." << std::endl;
		return 1;
	}
  std::string inputFileName1 = inputFileNames[ 0 ]; 
  std::string inputFileName2 = "";
	if( (inputFileNames.size() == 2) & (ops != "NOT") )
  {
	  inputFileName2 = inputFileNames[ 1 ];
  }

  /** Determine image properties */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 2;  
  unsigned int NumberOfComponents = 1;  
  GetImageProperties(
    inputFileName1,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents);
  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:      " << ComponentType << std::endl;
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

	/** outputFileName */
  std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
	if ( outputFileName == "" )
	{
		/** get file name without its last (shortest) extension  */
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2;
    if ( inputFileName2 != "" )
    { 
      part2 = itksys::SystemTools::GetFilenameName(inputFileName2);
    }
    else
    {
      part2 = ".";
      part2 += itksys::SystemTools::GetFilenameLastExtension(inputFileName1);
    }
    /** compose outputfilename */
    outputFileName = part1 + ops + part2;
	}

	/** Run the program. */
	try
	{
    /** NB: do not add floating point support, since logical operators are
     * not defined on those types */
		run(LogicalImageOperator,unsigned char,2);
		run(LogicalImageOperator,unsigned char,3);
		run(LogicalImageOperator,char,2);
		run(LogicalImageOperator,char,3);
		run(LogicalImageOperator,unsigned short,2);
		run(LogicalImageOperator,unsigned short,3);
		run(LogicalImageOperator,short,2);
		run(LogicalImageOperator,short,3);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main


/** Macro for easily instantiating the correct logical functor
 * if, for example name is AND, the result is:
 * typedef itk::BinaryFunctorImageFilter<
 *   InputImageType,
 *   InputImageType,
 *   InputImageType,
 *   itk::Functor::AND<InputPixelType >	ANDFilterType;
 * logicalFilter = (ANDFilterType::New()).GetPointer();
 * 
 */
#define InstantiateLogicalFilter(name)\
  typedef itk::BinaryFunctorImageFilter<\
    InputImageType, InputImageType, InputImageType,\
    itk::Functor::name<InputPixelType> >	name##FilterType;\
  if ( logicalOperatorName == #name )\
  {\
    logicalFilter = ( name##FilterType::New() ).GetPointer();\
  }
  

	/**
	 * ******************* LogicalOps *******************
	 */

template< class InputImageType >
void LogicalImageOperator( 
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::string & ops )
{
	/** Typedefs. */
  typedef typename InputImageType::PixelType                      InputPixelType;
  typedef itk::ImageToImageFilter<InputImageType, InputImageType> BaseFilterType;
  typedef itk::NotImageFilter<InputImageType, InputImageType>     NotFilterType;
  /** \todo: write a real dummy filter which does really nothing */
  typedef itk::CastImageFilter<InputImageType,InputImageType>     DummyFilterType;
  typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;
  /**  a pair indicating which functor should be used for an operator,
   * and whether the arguments should be swapped */
  typedef std::pair< std::string, bool >              SimpleOperatorType; 
  typedef std::map<std::string, SimpleOperatorType>   SimplifyMapType;

	/** Declarations. */
	typename BaseFilterType::Pointer logicalFilter = 0;
	typename ReaderType::Pointer reader1 = ReaderType::New();
	typename ReaderType::Pointer reader2 = 0;
	typename WriterType::Pointer writer = WriterType::New();
  SimplifyMapType simpmap;

  /** Available SimpleOperatorTypes are defined in itkLogicalFunctors.h:
   * AND, OR, XOR, NOT_AND, NOT_OR, NOT_XOR, ANDNOT, ORNOT
   * 
   * The Simplification map (simpmap) defines for every possible logical
   * operation of the form
   *   [not]( ([not] A) [{&,|,^} ([not] B])] )
   * a simplified version.
   * 
   * example1: A ^ (!B) = XORNOT(A,B) = NOT_XOR(A,B) = ! (A ^ B)
   * example2: (!A) & B = NOTAND(A,B) = ANDNOT(B,A) = B & (!A)  
   **/
  
  simpmap["AND"] = SimpleOperatorType("AND", false); // SimpleOperatorType
  simpmap["OR"] = SimpleOperatorType("OR", false); // SimpleOperatorType
  simpmap["XOR"] = SimpleOperatorType("XOR", false); // SimpleOperatorType
  simpmap["NOT"] = SimpleOperatorType("NOT", false); // SimpleOperatorType
  
  simpmap["ANDNOT"] = SimpleOperatorType("ANDNOT", false); // SimpleOperatorType
  simpmap["ORNOT"] = SimpleOperatorType("ORNOT", false); // SimpleOperatorType
  simpmap["XORNOT"] = SimpleOperatorType("NOT_XOR", false); // see example1
  
  simpmap["NOTAND"] = SimpleOperatorType("ANDNOT", true); // see example2
  simpmap["NOTOR"] = SimpleOperatorType("ORNOT", true);
  simpmap["NOTXOR"] = SimpleOperatorType("NOT_XOR", false);

  simpmap["NOTANDNOT"] = SimpleOperatorType("NOT_OR", false);
  simpmap["NOTORNOT"] = SimpleOperatorType("NOT_AND", false);
  simpmap["NOTXORNOT"] = SimpleOperatorType("XOR", false);

  simpmap["NOT_AND"] = SimpleOperatorType("NOT_AND", false); // SimpleOperatorType
  simpmap["NOT_OR"] = SimpleOperatorType("NOT_OR", false); // SimpleOperatorType
  simpmap["NOT_XOR"] = SimpleOperatorType("NOT_XOR", false); // SimpleOperatorType
  simpmap["NOT_NOT"] = SimpleOperatorType("DUMMY", false); // SimpleOperatorType

  simpmap["NOT_ANDNOT"] = SimpleOperatorType("ORNOT", true); 
  simpmap["NOT_ORNOT"] = SimpleOperatorType("ANDNOT", true); 
  simpmap["NOT_XORNOT"] = SimpleOperatorType("XOR", false);
  
  simpmap["NOT_NOTAND"] = SimpleOperatorType("ORNOT", false);
  simpmap["NOT_NOTOR"] = SimpleOperatorType("ANDNOT", false);
  simpmap["NOT_NOTXOR"] = SimpleOperatorType("XOR", false);

  simpmap["NOT_NOTANDNOT"] = SimpleOperatorType("OR", false);
  simpmap["NOT_NOTORNOT"] = SimpleOperatorType("AND", false);
  simpmap["NOT_NOTXORNOT"] = SimpleOperatorType("NOT_XOR", false);

  /** Read the images. */
	reader1->SetFileName( inputFileName1.c_str() );
  std::cout << "Reading image1: " << inputFileName1 << std::endl;
  reader1->Update();
  std::cout << "Done reading image1." << std::endl;

  if (inputFileName2 != "")
  {
    reader2 = ReaderType::New();
    reader2->SetFileName( inputFileName2.c_str() );
    std::cout << "Reading image2: " << inputFileName2 << std::endl;
    reader2->Update();
    std::cout << "Done reading image2." << std::endl;
  }

  /** Set up the logicalFilter */
  
  SimpleOperatorType logicalOperator = SimpleOperatorType("DUMMY", false);
  std::string logicalOperatorName = logicalOperator.first;
  bool swapArguments = logicalOperator.second;
  if ( simpmap.count(ops) != 0 )
  {
    logicalOperator = simpmap[ops];
    logicalOperatorName = logicalOperator.first;
    swapArguments = logicalOperator.second;
    if ( ops != logicalOperatorName )
    {
      std::string withswapping = "";
      if (swapArguments)
      {
        withswapping = " with swapped arguments";
      }
      std::cout 
        << "The desired logical operation, "
        << ops 
        << ", is simplified to the "
        << logicalOperator.first 
        << " operation"
        << withswapping
        << "."
        << std::endl;
    }
  }
  else
  {
    std::cerr << "ERROR: The desired operator is unknown: " << ops << std::endl;
    std::cerr << "The output will just be a copy of " << inputFileName1 << "." << std::endl;
  }

  /** Assign logicalFilter the correct filter (as defined by the logicalOperatorName). */
  if ( logicalOperatorName == "NOT" )
  {
    logicalFilter = (NotFilterType::New()).GetPointer();
  }  
  if ( logicalOperatorName == "DUMMY" )
  {
    logicalFilter = (DummyFilterType::New()).GetPointer();
  }
  InstantiateLogicalFilter(AND);
  InstantiateLogicalFilter(OR);
  InstantiateLogicalFilter(XOR);
  InstantiateLogicalFilter(NOT_AND);
  InstantiateLogicalFilter(NOT_OR);
  InstantiateLogicalFilter(NOT_XOR);
  InstantiateLogicalFilter(ANDNOT);
  InstantiateLogicalFilter(ORNOT);

  if ( swapArguments )
  {
    /** swap the input files */
    logicalFilter->SetInput( 1, reader1->GetOutput() );
    logicalFilter->SetInput( 0, reader2->GetOutput() );
  }
  else
  {
    logicalFilter->SetInput( 0, reader1->GetOutput() );
    if ( reader2.IsNotNull() )
    {
      logicalFilter->SetInput( 1, reader2->GetOutput() );
    }
  }

  std::cout 
    << "Performing logical operation, " 
    << logicalOperatorName
    << ", on input image(s)..." 
    << std::endl;
  logicalFilter->Update();
  std::cout << "Done performing logical operation." << std::endl;

  /** Write the image to disk */
	writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( logicalFilter->GetOutput() );
  std::cout << "Writing output to disk as: " << outputFileName << std::endl;
	writer->Update();
  std::cout << "Done writing output to disk." << std::endl;
	

} // end LogicalOps


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "Logical operations on one or two images." << std::endl;
	std::cout << "Usage:" << std::endl << "pxlogicalimageoperator" << std::endl;
	std::cout << "\t-in\tinputFilename1 [inputFilename2]" << std::endl;
  std::cout << "\t-ops\tLogicalOperator of the following form:\n"
            << "\t    \t  [!]( ([!] A) [{&,|,^} ([!] B])] )\n"
            << "\t    \tnotation:\n"
            << "\t    \t  [NOT_][NOT][{AND,OR,XOR}[NOT]]\n"
            << "\t    \tnotation examples:\n"
            << "\t    \t  ANDNOT = A & (!B)\n" 
            << "\t    \t  NOTAND = (!A) & B\n" 
            << "\t    \t  NOTANDNOT = (!A) & (!B)\n" 
            << "\t    \t  NOT_NOTANDNOT = !( (!A) & (!B) )\n" 
            << "\t    \t  NOT_AND = !(A & B)\n"
            << "\t    \t  OR = A | B\n" 
            << "\t    \t  XOR = A ^ B\n" 
            << "\t    \t  NOT = !A \n" 
            << "\t    \t  NOT_NOT = A \n" 
            << "\t    \tInternally this expression is simplified.\n"
            << std::endl;
	std::cout << "\t[-out]\toutputFilename, default in1 + <ops> + in2 + .mhd" << std::endl;
  std::cout << "\t[-dim]\tdimension, default: automatically determined from inputimage1" << std::endl;
  std::cout << "\t[-pt]\tpixelType, default: automatically determined from inputimage1" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp

