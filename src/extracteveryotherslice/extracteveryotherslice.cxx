#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, everyOther, direction ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ExtractEveryOtherSlice. */
template< class InputImageType >
void ExtractEveryOtherSlice(
  const std::string & inputFileName,
  const std::string & outputFileName,
	const unsigned int & everyOther,
  const unsigned int & direction );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 5 || argc > 13 )
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

	std::string	outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
	outputFileName += "EveryOtherKExtracted.mhd";
	bool retout = parser->GetCommandLineArgument( "-out", outputFileName );

	unsigned int everyOther;
	bool retK = parser->GetCommandLineArgument( "-K", everyOther );

	unsigned int direction = 2;
	bool retd = parser->GetCommandLineArgument( "-d", direction );

	unsigned int Dimension = 3;
	bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );

	std::string	PixelType = "short";
	bool retpt = parser->GetCommandLineArgument( "-pt", PixelType );

	/** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
	if ( !retK )
	{
		std::cerr << "ERROR: You should specify \"-K\"." << std::endl;
		return 1;
	}

	/** Get rid of the possible "_" in PixelType. */
	ReplaceUnderscoreWithSpace( PixelType );

	/** Check everyOther. */
	if ( everyOther < 2 )
	{
		std::cout << "ERROR: K should be larger than 1." << std::endl;
		return 1;
	}

	/** Check direction. */
	if ( direction + 1 > Dimension )
	{
		std::cout << "ERROR: direction should be 0, 1 or 2." << std::endl;
		return 1;
	}

	/** Run the program. */
  bool supported = false;
	try
	{
		run( ExtractEveryOtherSlice, unsigned char, 3 );
		run( ExtractEveryOtherSlice, char, 3 );
		run( ExtractEveryOtherSlice, unsigned short, 3 );
		run( ExtractEveryOtherSlice, short, 3 );
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr <<
      "pixel (component) type = " << PixelType <<
      " ; dimension = " << Dimension 
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ******************* ExtractEveryOtherSlice *******************
	 *
	 * The ExtractEveryOtherSlice function templated over the input pixel type.
	 */

template< class InputImageType >
void ExtractEveryOtherSlice(
  const std::string & inputFileName,
  const std::string & outputFileName,
	const unsigned int & everyOther,
  const unsigned int & direction )
{
	/** TYPEDEF's. */
	typedef itk::ImageSliceConstIteratorWithIndex<
		InputImageType >																	SliceConstIteratorType;
	typedef itk::ImageSliceIteratorWithIndex<
		InputImageType >																	SliceIteratorType;
	typedef itk::ImageFileReader< InputImageType >			ReaderType;
	typedef itk::ImageFileWriter< InputImageType >			WriterType;

	typedef typename InputImageType::RegionType				RegionType;
	typedef typename RegionType::IndexType						IndexType;
	typedef typename InputImageType::SizeType					SizeType;

	const unsigned int Dimension = InputImageType::ImageDimension;

	/** Read in the inputImage. */
	typename ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( inputFileName.c_str() );
	reader->Update();

	/** Define size of output image. */
	SizeType sizeIn = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
	SizeType sizeOut = sizeIn;
	sizeOut[ direction ] /= everyOther;

	/** Define region of output image. */
	RegionType region;
	region.SetIndex( reader->GetOutput()->GetLargestPossibleRegion().GetIndex() );
	region.SetSize( sizeOut );

	/** Create output image. */
	typename InputImageType::Pointer outputImage = InputImageType::New();
	outputImage->SetSpacing( reader->GetOutput()->GetSpacing() );
	outputImage->SetOrigin( reader->GetOutput()->GetOrigin() );
	outputImage->SetRegions( region );
	outputImage->Allocate();

	/** Create iterators. */
  SliceConstIteratorType itIn( reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion() );
	SliceIteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );
 
	/** Set direction, default slice = z. */
	if ( direction == 0 )
	{
		itIn.SetFirstDirection(1);
		itIn.SetSecondDirection(2);
		itOut.SetFirstDirection(1);
		itOut.SetSecondDirection(2);
	}
	else if ( direction == 1 )
	{
		itIn.SetFirstDirection(0);
		itIn.SetSecondDirection(2);
		itOut.SetFirstDirection(0);
		itOut.SetSecondDirection(2);
	}
	else if ( direction == 2 )
	{
		itIn.SetFirstDirection(0);
		itIn.SetSecondDirection(1);
		itOut.SetFirstDirection(0);
		itOut.SetSecondDirection(1);
	}

	/** Loop over images. */
	IndexType index;
	itIn.GoToBegin();
	itOut.GoToBegin();
	while( !itOut.IsAtEnd() )
	{
		while( !itOut.IsAtEndOfSlice() )
		{
			while( !itOut.IsAtEndOfLine() )
			{
				itOut.Set( itIn.Get() );
        ++itIn;
				++itOut;
			}
			itIn.NextLine();
			itOut.NextLine();
		}
		itIn.NextSlice();
		itOut.NextSlice();
		/** Skip some slices in inputImage. */
		index = itIn.GetIndex();
		for ( unsigned int i = 1; i < everyOther; i++ )
		{
			index[ direction ]++;
		}
		itIn.SetIndex( index );
	} 

	/** Write the output image. */
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( outputImage );
	writer->Update();

} // end ExtractEveryOtherSlice


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp( void )
{
	std::cout << "Usage:" << std::endl << "pxextracteveryotherslice" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
	std::cout << "  [-out]   outputFilename, default in + EveryOtherKExtracted.mhd" << std::endl;
	std::cout << "  -K       every other slice K" << std::endl;
	std::cout << "  [-d]     direction, default is z-axes" << std::endl;
	std::cout << "  [-dim]   dimension, default 3" << std::endl;
	std::cout << "  [-pt]    pixelType, default short" << std::endl;
	std::cout << "Supported: 3D, (unsigned) short, (unsigned) char." << std::endl;
} // end PrintHelp

