#include "castconverthelpers.h"

int FileConverterScalar2D( const std::string &inputPixelComponentType,
	const std::string &outputPixelComponentType, const std::string &inputFileName,
	const std::string &outputFileName, int inputDimension )
{
	enum { ImageDims = 2 };

	if ( inputDimension == ImageDims )
	{
		/** From unsigned char to something else. */
		callCorrectReadWriterMacro( unsigned char, unsigned char, ImageDims );
		callCorrectReadWriterMacro( unsigned char, char, ImageDims );
		callCorrectReadWriterMacro( unsigned char, unsigned short, ImageDims );
		callCorrectReadWriterMacro( unsigned char, short, ImageDims );
		callCorrectReadWriterMacro( unsigned char, unsigned int, ImageDims );
		callCorrectReadWriterMacro( unsigned char, int, ImageDims );
		callCorrectReadWriterMacro( unsigned char, unsigned long, ImageDims );
		callCorrectReadWriterMacro( unsigned char, long, ImageDims );
		callCorrectReadWriterMacro( unsigned char, float, ImageDims );
		callCorrectReadWriterMacro( unsigned char, double, ImageDims );

		/** From char to something else. */
		callCorrectReadWriterMacro( char, unsigned char, ImageDims );
		callCorrectReadWriterMacro( char, char, ImageDims );
		callCorrectReadWriterMacro( char, unsigned short, ImageDims );
		callCorrectReadWriterMacro( char, short, ImageDims );
		callCorrectReadWriterMacro( char, unsigned int, ImageDims );
		callCorrectReadWriterMacro( char, int, ImageDims );
		callCorrectReadWriterMacro( char, unsigned long, ImageDims );
		callCorrectReadWriterMacro( char, long, ImageDims );
		callCorrectReadWriterMacro( char, float, ImageDims );
		callCorrectReadWriterMacro( char, double, ImageDims );

		/** From unsigned short to something else. */
		callCorrectReadWriterMacro( unsigned short, unsigned char, ImageDims );
		callCorrectReadWriterMacro( unsigned short, char, ImageDims );
		callCorrectReadWriterMacro( unsigned short, unsigned short, ImageDims );
		callCorrectReadWriterMacro( unsigned short, short, ImageDims );
		callCorrectReadWriterMacro( unsigned short, unsigned int, ImageDims );
		callCorrectReadWriterMacro( unsigned short, int, ImageDims );
		callCorrectReadWriterMacro( unsigned short, unsigned long, ImageDims );
		callCorrectReadWriterMacro( unsigned short, long, ImageDims );
		callCorrectReadWriterMacro( unsigned short, float, ImageDims );
		callCorrectReadWriterMacro( unsigned short, double, ImageDims );

		/** From short to something else. */
		callCorrectReadWriterMacro( short, unsigned char, ImageDims );
		callCorrectReadWriterMacro( short, char, ImageDims );
		callCorrectReadWriterMacro( short, unsigned short, ImageDims );
		callCorrectReadWriterMacro( short, short, ImageDims );
		callCorrectReadWriterMacro( short, unsigned int, ImageDims );
		callCorrectReadWriterMacro( short, int, ImageDims );
		callCorrectReadWriterMacro( short, unsigned long, ImageDims );
		callCorrectReadWriterMacro( short, long, ImageDims );
		callCorrectReadWriterMacro( short, float, ImageDims );
		callCorrectReadWriterMacro( short, double, ImageDims );

		/** From unsigned int to something else. */
		callCorrectReadWriterMacro( unsigned int, unsigned char, ImageDims );
		callCorrectReadWriterMacro( unsigned int, char, ImageDims );
		callCorrectReadWriterMacro( unsigned int, unsigned short, ImageDims );
		callCorrectReadWriterMacro( unsigned int, short, ImageDims );
		callCorrectReadWriterMacro( unsigned int, unsigned int, ImageDims );
		callCorrectReadWriterMacro( unsigned int, int, ImageDims );
		callCorrectReadWriterMacro( unsigned int, unsigned long, ImageDims );
		callCorrectReadWriterMacro( unsigned int, long, ImageDims );
		callCorrectReadWriterMacro( unsigned int, float, ImageDims );
		callCorrectReadWriterMacro( unsigned int, double, ImageDims );

		/** From int to something else. */
		callCorrectReadWriterMacro( int, unsigned char, ImageDims );
		callCorrectReadWriterMacro( int, char, ImageDims );
		callCorrectReadWriterMacro( int, unsigned short, ImageDims );
		callCorrectReadWriterMacro( int, short, ImageDims );
		callCorrectReadWriterMacro( int, unsigned int, ImageDims );
		callCorrectReadWriterMacro( int, int, ImageDims );
		callCorrectReadWriterMacro( int, unsigned long, ImageDims );
		callCorrectReadWriterMacro( int, long, ImageDims );
		callCorrectReadWriterMacro( int, float, ImageDims );
		callCorrectReadWriterMacro( int, double, ImageDims );

		/** From unsigned long to something else. */
		callCorrectReadWriterMacro( unsigned long, unsigned char, ImageDims );
		callCorrectReadWriterMacro( unsigned long, char, ImageDims );
		callCorrectReadWriterMacro( unsigned long, unsigned short, ImageDims );
		callCorrectReadWriterMacro( unsigned long, short, ImageDims );
		callCorrectReadWriterMacro( unsigned long, unsigned int, ImageDims );
		callCorrectReadWriterMacro( unsigned long, int, ImageDims );
		callCorrectReadWriterMacro( unsigned long, unsigned long, ImageDims );
		callCorrectReadWriterMacro( unsigned long, long, ImageDims );
		callCorrectReadWriterMacro( unsigned long, float, ImageDims );
		callCorrectReadWriterMacro( unsigned long, double, ImageDims );

		/** From long to something else. */
		callCorrectReadWriterMacro( long, unsigned char, ImageDims );
		callCorrectReadWriterMacro( long, char, ImageDims );
		callCorrectReadWriterMacro( long, unsigned short, ImageDims );
		callCorrectReadWriterMacro( long, short, ImageDims );
		callCorrectReadWriterMacro( long, unsigned int, ImageDims );
		callCorrectReadWriterMacro( long, int, ImageDims );
		callCorrectReadWriterMacro( long, unsigned long, ImageDims );
		callCorrectReadWriterMacro( long, long, ImageDims );
		callCorrectReadWriterMacro( long, float, ImageDims );
		callCorrectReadWriterMacro( long, double, ImageDims );

		/** From float to something else. */
		callCorrectReadWriterMacro( float, unsigned char, ImageDims );
		callCorrectReadWriterMacro( float, char, ImageDims );
		callCorrectReadWriterMacro( float, unsigned short, ImageDims );
		callCorrectReadWriterMacro( float, short, ImageDims );
		callCorrectReadWriterMacro( float, unsigned int, ImageDims );
		callCorrectReadWriterMacro( float, int, ImageDims );
		callCorrectReadWriterMacro( float, unsigned long, ImageDims );
		callCorrectReadWriterMacro( float, long, ImageDims );
		callCorrectReadWriterMacro( float, float, ImageDims );
		callCorrectReadWriterMacro( float, double, ImageDims );

		/** From double to something else. */
		callCorrectReadWriterMacro( double, unsigned char, ImageDims );
		callCorrectReadWriterMacro( double, char, ImageDims );
		callCorrectReadWriterMacro( double, unsigned short, ImageDims );
		callCorrectReadWriterMacro( double, short, ImageDims );
		callCorrectReadWriterMacro( double, unsigned int, ImageDims );
		callCorrectReadWriterMacro( double, int, ImageDims );
		callCorrectReadWriterMacro( double, unsigned long, ImageDims );
		callCorrectReadWriterMacro( double, long, ImageDims );
		callCorrectReadWriterMacro( double, float, ImageDims );
		callCorrectReadWriterMacro( double, double, ImageDims );

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
