#include "computeDifferenceImageHelpers.h"

int ComputeScalarDifferenceImage3D( const std::string &inputPixelComponentType1,
  const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
  const std::string &image1FileName, const std::string &image2FileName,
  const std::string &outputFileName, int inputDimension )
{
  enum { ImageDims = 3 };

  if ( inputDimension == ImageDims )
  {
    /** Because we are taking differences we assume that conversions
     * should be made to the signed pixel types.
     */

    /** From unsigned char and unsigned char to something else. */
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, float, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned char, unsigned char, double, ImageDims );

    /** From char and char to something else. */
    callCorrectScalarDifferenceMacro( char, char, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, char, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, short, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, int, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, long, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, float, ImageDims );
    callCorrectScalarDifferenceMacro( char, char, double, ImageDims );

    /** From unsigned short and unsigned short to something else. */
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, float, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned short, unsigned short, double, ImageDims );

    /** From short and short to something else. */
    callCorrectScalarDifferenceMacro( short, short, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, char, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, short, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, int, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, long, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, float, ImageDims );
    callCorrectScalarDifferenceMacro( short, short, double, ImageDims );

    /** From unsigned int and unsigned int to something else. */
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, float, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned int, unsigned int, double, ImageDims );

    /** From int and int to something else. */
    callCorrectScalarDifferenceMacro( int, int, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, char, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, short, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, int, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, long, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, float, ImageDims );
    callCorrectScalarDifferenceMacro( int, int, double, ImageDims );

    /** From unsigned long and unsigned long to something else. */
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, char, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, short, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, int, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, long, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, float, ImageDims );
    callCorrectScalarDifferenceMacro( unsigned long, unsigned long, double, ImageDims );

    /** From long and long to something else. */
    callCorrectScalarDifferenceMacro( long, long, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, char, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, short, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, int, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, long, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, float, ImageDims );
    callCorrectScalarDifferenceMacro( long, long, double, ImageDims );

    /** From float and float to something else. */
    callCorrectScalarDifferenceMacro( float, float, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, char, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, short, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, int, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, long, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, float, ImageDims );
    callCorrectScalarDifferenceMacro( float, float, double, ImageDims );

    /** From double and double to something else. */
    callCorrectScalarDifferenceMacro( double, double, unsigned char, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, char, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, unsigned short, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, short, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, unsigned int, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, int, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, unsigned long, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, long, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, float, ImageDims );
    callCorrectScalarDifferenceMacro( double, double, double, ImageDims );

    /** etcetera. */
  }
  else
  {
    std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
    std::cerr << "Only "<< ImageDims << "D images are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a value. */
  return 0;

} // end support for SCALAR pixel type
