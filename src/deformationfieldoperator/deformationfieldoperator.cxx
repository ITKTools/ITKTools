#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "deformationfieldoperator.h"
#include "itkExceptionObject.h"
#include <itksys/SystemTools.hxx>


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileName, outputFileName, ops ); \
}

//-------------------------------------------------------------------------------------

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

	/** Get arguments. */
	std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );
  if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

	std::string ops = "MAGNITUDE";
	bool retops = parser->GetCommandLineArgument( "-ops", ops );
  
	std::string	outputFileName = "";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );
	if ( outputFileName == "" )
	{
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName);
    std::string ext = 
      itksys::SystemTools::GetFilenameLastExtension(inputFileName);
    outputFileName = part1 + ops + ext;
  }
	
  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string	PixelType = "VECTOR";
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = Dimension;
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
  std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tComponentType:      " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  
  if (NumberOfComponents != Dimension)
  { 
    std::cerr << "ERROR: The NumberOfComponents must equal the Dimension!" << std::endl;
    return 1; 
  }
  if (NumberOfComponents == 1)
  { 
    std::cerr << "Scalar images are not supported!" << std::endl;
    return 1; 
  }
  
	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace(ComponentType);
	
	/** Run the program. */
	try
	{
    run(DeformationFieldOperator,float,2);
    run(DeformationFieldOperator,float,3);
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
	
	/** End program. */
	return 0;

} // end main



	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "This program converts between deformations (displacement fields) and transformations, and computes the magnitude or jacobian of a deformation field." << std::endl;
	
	std::cout << "Usage:" << std::endl << "pxdeformationfieldoperator" << std::endl;
	std::cout << "\t-in   \tinputFilename" << std::endl;
  std::cout << "\t[-ops]\toperation; options: DEF2TRANS, TRANS2DEF, MAGNITUDE, JACOBIAN. default: MAGNITUDE" << std::endl;
  std::cout << "\t[-out]\toutputFilename; default: in + {operation}.mhd" << std::endl;
	std::cout << "Supported: 2D, 3D, vector of floats, number of components must equal number of dimensions." << std::endl;
} // end PrintHelp

