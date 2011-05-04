#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

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
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

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

