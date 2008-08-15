#include "computeDifferenceImageHelpers.h"

int ComputeVectorDifferenceImage3D( const std::string &inputPixelComponentType1,
  const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
  const std::string &image1FileName, const std::string &image2FileName,
  const std::string &outputFileName, int inputDimension, int vectorDimension )
{
  enum { ImageDims = 3 };

  if ( inputDimension == ImageDims )
  {
    /** Because we are taking differences we assume that conversions
     * should be made to the signed pixel types.
     */

    if ( vectorDimension == 2 )
    {
      /** From unsigned char and unsigned char to something else. */
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, double, ImageDims, 2 );

      /** From char and char to something else. */
      callCorrectVectorDifferenceMacro( char, char, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( char, char, double, ImageDims, 2 );

      /** From unsigned short and unsigned short to something else. */
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, double, ImageDims, 2 );

      /** From short and short to something else. */
      callCorrectVectorDifferenceMacro( short, short, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( short, short, double, ImageDims, 2 );

      /** From unsigned int and unsigned int to something else. */
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, double, ImageDims, 2 );

      /** From int and int to something else. */
      callCorrectVectorDifferenceMacro( int, int, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( int, int, double, ImageDims, 2 );

      /** From unsigned long and unsigned long to something else. */
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, double, ImageDims, 2 );

      /** From long and long to something else. */
      callCorrectVectorDifferenceMacro( long, long, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( long, long, double, ImageDims, 2 );

      /** From float and float to something else. */
      callCorrectVectorDifferenceMacro( float, float, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( float, float, double, ImageDims, 2 );

      /** From double and double to something else. */
      callCorrectVectorDifferenceMacro( double, double, unsigned char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, char, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, unsigned short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, short, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, unsigned int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, int, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, unsigned long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, long, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, float, ImageDims, 2 );
      callCorrectVectorDifferenceMacro( double, double, double, ImageDims, 2 );

      /** etcetera. */
    } // end support for vectors of length 2
    else if ( vectorDimension == 3 )
    {
      /** From unsigned char and unsigned char to something else. */
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned char, unsigned char, double, ImageDims, 3 );

      /** From char and char to something else. */
      callCorrectVectorDifferenceMacro( char, char, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( char, char, double, ImageDims, 3 );

      /** From unsigned short and unsigned short to something else. */
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned short, unsigned short, double, ImageDims, 3 );

      /** From short and short to something else. */
      callCorrectVectorDifferenceMacro( short, short, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( short, short, double, ImageDims, 3 );

      /** From unsigned int and unsigned int to something else. */
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned int, unsigned int, double, ImageDims, 3 );

      /** From int and int to something else. */
      callCorrectVectorDifferenceMacro( int, int, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( int, int, double, ImageDims, 3 );

      /** From unsigned long and unsigned long to something else. */
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( unsigned long, unsigned long, double, ImageDims, 3 );

      /** From long and long to something else. */
      callCorrectVectorDifferenceMacro( long, long, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( long, long, double, ImageDims, 3 );

      /** From float and float to something else. */
      callCorrectVectorDifferenceMacro( float, float, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( float, float, double, ImageDims, 3 );

      /** From double and double to something else. */
      callCorrectVectorDifferenceMacro( double, double, unsigned char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, char, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, unsigned short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, short, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, unsigned int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, int, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, unsigned long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, long, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, float, ImageDims, 3 );
      callCorrectVectorDifferenceMacro( double, double, double, ImageDims, 3 );

      /** etcetera. */
    } // end support for vectors of length 3
    else
    {
      std::cerr << "VectorDimension equals " << vectorDimension
        << ", which is not supported for "
        << ImageDims << "D images." << std::endl;
      return 1;
    } // end if over vector dimension
  } // end support for 2D images
  else
  {
    std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
    std::cerr << "Only "<< ImageDims << "D images are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a value. */
  return 0;

} // end support for VECTOR pixel type
