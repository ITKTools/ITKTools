#ifndef __CountNonZeroVoxels_CXX__
#define __CountNonZeroVoxels_CXX__

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check number of arguments. */
  if ( argc != 2 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "pxcountnonzerovoxels imagename" << std::endl;
    return 1;
  }

  // Some consts.
  const unsigned int  Dimension = 3;
  typedef short PixelType;

  // TYPEDEF's
  typedef itk::Image< PixelType, Dimension >          ImageType;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageRegionConstIterator< ImageType >  IteratorType;

  /** Get arguments. */
  std::string filename = argv[ 1 ];

  /** Read image. */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename.c_str() );

  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: caught ITK exception while reading image "
      << filename << "." << std::endl;
    std::cerr << excp << std::endl;
    return 1;
  }

  /** Create iterator and counter. */
  IteratorType it( reader->GetOutput(),
    reader->GetOutput()->GetLargestPossibleRegion() );
  it.GoToBegin();
  unsigned int counter = 0;

  /** Walk over the image. */
  while ( !it.IsAtEnd() )
  {
    if ( it.Value() ) counter++;
    /** Increase iterator. */
    ++it;
  } // end while

  /** Print to screen. */
  std::cout << counter << std::endl;

  /** End program. Return a value. */
  return 0;

} // end main


#endif // #ifndef __CountNonZeroVoxels_CXX__
