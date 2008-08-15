#ifndef __GetPointsInImage_CXX__
#define __GetPointsInImage_CXX__

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"

#include <fstream>


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check number of arguments. */
  if ( argc != 3 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "pxgetpointsinimage pointsfilename imagename" << std::endl;
    std::cout << "NOTE: only 2D short are created and arguments should be in above order." << std::endl;
    return 1;
  }

  // Some consts.
  const unsigned int  Dimension = 2;
  typedef short PixelType;

  // TYPEDEF's
  typedef itk::Image< PixelType, Dimension >              ImageType;
  typedef itk::ImageRegionIteratorWithIndex< ImageType >  IteratorType;
  typedef itk::ImageFileReader< ImageType >               ReaderType;

  typedef ImageType::RegionType     RegionType;
  typedef RegionType::SizeType      SizeType;
  typedef ImageType::PointType      PointType;
  typedef ImageType::IndexType      IndexType;
  typedef ImageType::SpacingType    SpacingType;

  /** Get arguments. */
  std::string pointfilename = argv[ 1 ];
  std::string imagefilename = argv[ 2 ];

  /** Read the image. */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( imagefilename.c_str() );

  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: caught ITK exception while reading image "
      << imagefilename << "." << std::endl;
    std::cerr << excp << std::endl;
    return 1;
  }

  /** Create iterator. */
  IteratorType it( reader->GetOutput(),
    reader->GetOutput()->GetLargestPossibleRegion() );
  it.GoToBegin();

  /** Create vector of indices. */
  std::vector< std::vector<IndexType> > points( 1 );
  PixelType value;

  /** Loop over the image. */
  while ( !it.IsAtEnd() )
  {
    value = it.Get();
    if ( value != 0 )
    {
      /** Make the points vector long enough. */
      if ( value > points.size() ) points.resize( static_cast<unsigned int>( value ) );
      /** Insert a point at the value-th row of points. */
      //points[ value - 1 ].resize( 1 );
      //IndexType index = it.GetIndex();
      points[ value - 1 ].push_back( it.GetIndex() );
    }
    /** Increase iterator. */   
    ++it;
  } // end while

  /** Calculate the mean position of value. */
  std::vector<PointType> meanpoints( points.size() );
  for ( unsigned int i = 0; i < points.size(); i++ )
  {
    meanpoints[ i ].Fill( 0.0 );
    for ( unsigned int j = 0; j < points[ i ].size(); j++ )
    {
      meanpoints[ i ][ 0 ] += points[ i ][ j ][ 0 ];
      meanpoints[ i ][ 1 ] += points[ i ][ j ][ 1 ];
    }
    meanpoints[ i ][ 0 ] /= points[ i ].size();
    meanpoints[ i ][ 1 ] /= points[ i ].size();
  }

  /** Open the file containing the inputpoints.*/
  std::ofstream pointfile( pointfilename.c_str() );
  SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  if ( pointfile.is_open() )
  {
    /** Write the inputpoints to a text file. */
    pointfile << points.size() << std::endl;
    for ( unsigned int j = 0; j < points.size(); j++ )
    {
      /** Set the points in the file. */
      pointfile << meanpoints[ j ][ 0 ] << "\t"
        << meanpoints[ j ][ 1 ] << std::endl;
    }
  }
  else
  {
    std::cerr << "WARNING: the file \"" << pointfilename
      << "\" could not be opened!" << std::endl;
    return 1;
  } // end if

  /** Close all files. */
  pointfile.close();
  
  /** End program. Return a value. */
  return 0;

} // end main


#endif // #ifndef __GetPointsInImage_CXX__
