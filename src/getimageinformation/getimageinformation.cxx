#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageFileReader.h"

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 4 )
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

  unsigned int index;
  bool reti = parser->GetCommandLineArgument( "-i", index );

	bool exdim = parser->ArgumentExists( "-dim" );
	bool expt = parser->ArgumentExists( "-pt" );
	bool exct = parser->ArgumentExists( "-ct" );
	bool exnoc = parser->ArgumentExists( "-noc" );
  bool exsz = parser->ArgumentExists( "-sz" );
	bool exsp = parser->ArgumentExists( "-sp" );
	bool exo = parser->ArgumentExists( "-o" );

	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}

  /** Typedef's. */
  const unsigned int Dimension = 3;
  typedef short      PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageIOBase                    ImageIOBaseType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;

  /** Create a testReader and generate all information. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( inputFileName.c_str() );
	try
	{
		testReader->GenerateOutputInformation();
	}
	catch( itk::ExceptionObject  &  err  )
  {
    std::cerr  << "ExceptionObject caught !"  << std::endl;
    std::cerr  << err <<  std::endl;
    return 1;
  }

  /** Extract the ImageIO from the testReader. */
  ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();
  unsigned int dim = testImageIOBase->GetNumberOfDimensions();

  /** Check the index. */
  if ( index > dim - 1 )
  {
    std::cerr << "ERROR: index out of bounds." << std::endl;
    return 1;
  }

  /** Print image dimension. */
  if ( exdim )
  {
    std::cout << dim;
    return 0;
  }
  
  /** Print image pixel type. */
  if ( expt )
  {
    //ReplaceUnderscoreWithSpace( PixelType );
    std::cout << testImageIOBase->GetPixelTypeAsString(
      testImageIOBase->GetPixelType() );
    return 0;
  }

  /** Print image component type. */
  if ( exct )
  {
    std::cout << testImageIOBase->GetComponentTypeAsString(
      testImageIOBase->GetComponentType() );
    return 0;
  }

  /** Print image number of components. */
  if ( exnoc )
  {
    std::cout << testImageIOBase->GetNumberOfComponents();
    return 0;
  }

  /** Print image size. */
  if ( exsz )
  {
    if ( reti )
    {
      std::cout << testImageIOBase->GetDimensions( index );
    }
    else
    {
      for ( unsigned int i = 0; i < dim - 1; i++ )
      {
        std::cout << testImageIOBase->GetDimensions( i ) << " ";
      }
      std::cout << testImageIOBase->GetDimensions( dim - 1 );
    }
    return 0;
  }

  /** Print image spacing. */
  if ( exsp )
  {
    if ( reti )
    {
      std::cout << testImageIOBase->GetSpacing( index );
    }
    else
    {
      for ( unsigned int i = 0; i < dim - 1; i++ )
      {
        std::cout << testImageIOBase->GetDimensions( i ) << " ";
      }
      std::cout << testImageIOBase->GetSpacing( dim - 1 );
    }
    return 0;
  }

  /** Print image origin. */
  if ( exsp )
  {
    if ( reti )
    {
      std::cout << testImageIOBase->GetOrigin( index );
    }
    else
    {
      for ( unsigned int i = 0; i < dim - 1; i++ )
      {
        std::cout << testImageIOBase->GetDimensions( i ) << " ";
      }
      std::cout << testImageIOBase->GetOrigin( dim - 1 );
    }
    return 0;
  }
	
	/** End program. */
	return 1;

} // end main



	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxgetimageinformation" << std::endl;
	std::cout << "  -in     inputFileName" << std::endl;
	std::cout << "  [-dim]  dimension, default 3" << std::endl;
	std::cout << "  [-pt]   pixelType, default scalar" << std::endl;
  std::cout << "  [-ct]   componentType, default short" << std::endl;
  std::cout << "  [-noc]  #components, default 1" << std::endl;
  std::cout << "  [-sz]   size" << std::endl;
  std::cout << "  [-sp]   spacing" << std::endl;
  std::cout << "  [-o]    origin" << std::endl;
  std::cout << "Image information about the inputFileName is printed to screen." << std::endl;
  std::cout << "Only one option should be given, e.g. -sp, then the spacing is printed." << std::endl;
  std::cout << "  [-i]    index, if this option is given only e.g. spacing[index] is printed." << std::endl;
} // end PrintHelp

