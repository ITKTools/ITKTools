#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check number of arguments. */
  if ( argc != 3 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "pxcountnonzerovoxels -in imagename" << std::endl;
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::string inputFileName;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  /** Checks. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify the input file name with \"-in\"." << std::endl;
    return 1;
  }

  // Some consts.
  const unsigned int  Dimension = 3;
  typedef short PixelType;

  // TYPEDEF's
  typedef itk::Image< PixelType, Dimension >          ImageType;
  typedef ImageType::SpacingType                      SpacingType;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageRegionConstIterator< ImageType >  IteratorType;

  /** Read image. */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: caught ITK exception while reading image "
      << inputFileName << "." << std::endl;
    std::cerr << excp << std::endl;
    return 1;
  }

  /** Get the spacing. */
  SpacingType sp = reader->GetOutput()->GetSpacing();
  double voxelVolume = 1.0;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    voxelVolume *= sp[ i ];
  }

  /** Create iterator and counter. */
  IteratorType it( reader->GetOutput(),
    reader->GetOutput()->GetLargestPossibleRegion() );
  it.GoToBegin();
  unsigned int counter = 0;

  /** Walk over the image. */
  while ( !it.IsAtEnd() )
  {
    if ( it.Value() )
    {
      counter++;
    }
    ++it;
  } // end while

  /** Print to screen. */
  std::cout << "count: " << counter << std::endl;
  std::cout << "volume: " << counter / voxelVolume / 1000.0 << std::endl;

  /** End program. Return a value. */
  return 0;

} // end main

