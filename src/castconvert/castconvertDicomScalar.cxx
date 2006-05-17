#include "castconverthelpers.h"

int DicomFileConverterScalar( const std::string &inputPixelComponentType,
	const std::string &outputPixelComponentType, const std::string &inputDirectoryName,
	const std::string &outputFileName, int inputDimension )
{
	/** Support for 3D images. */
	if ( inputDimension == 3 )
	{
		/** From unsigned char to something else. */
		callCorrectReadDicomWriterMacro( unsigned char, unsigned char );
		callCorrectReadDicomWriterMacro( unsigned char, char );
		callCorrectReadDicomWriterMacro( unsigned char, unsigned short );
		callCorrectReadDicomWriterMacro( unsigned char, short );
		callCorrectReadDicomWriterMacro( unsigned char, unsigned int );
		callCorrectReadDicomWriterMacro( unsigned char, int );
		callCorrectReadDicomWriterMacro( unsigned char, unsigned long );
		callCorrectReadDicomWriterMacro( unsigned char, long );
		callCorrectReadDicomWriterMacro( unsigned char, float );
		callCorrectReadDicomWriterMacro( unsigned char, double );

		/** From char to something else. */
		callCorrectReadDicomWriterMacro( char, unsigned char );
		callCorrectReadDicomWriterMacro( char, char );
		callCorrectReadDicomWriterMacro( char, unsigned short );
		callCorrectReadDicomWriterMacro( char, short );
		callCorrectReadDicomWriterMacro( char, unsigned int );
		callCorrectReadDicomWriterMacro( char, int );
		callCorrectReadDicomWriterMacro( char, unsigned long );
		callCorrectReadDicomWriterMacro( char, long );
		callCorrectReadDicomWriterMacro( char, float );
		callCorrectReadDicomWriterMacro( char, double );

		/** From unsigned short to something else. */
		callCorrectReadDicomWriterMacro( unsigned short, unsigned char );
		callCorrectReadDicomWriterMacro( unsigned short, char );
		callCorrectReadDicomWriterMacro( unsigned short, unsigned short );
		callCorrectReadDicomWriterMacro( unsigned short, short );
		callCorrectReadDicomWriterMacro( unsigned short, unsigned int );
		callCorrectReadDicomWriterMacro( unsigned short, int );
		callCorrectReadDicomWriterMacro( unsigned short, unsigned long );
		callCorrectReadDicomWriterMacro( unsigned short, long );
		callCorrectReadDicomWriterMacro( unsigned short, float );
		callCorrectReadDicomWriterMacro( unsigned short, double );

		/** From short to something else. */
		callCorrectReadDicomWriterMacro( short, unsigned char );
		callCorrectReadDicomWriterMacro( short, char );
		callCorrectReadDicomWriterMacro( short, unsigned short );
		callCorrectReadDicomWriterMacro( short, short );
		callCorrectReadDicomWriterMacro( short, unsigned int );
		callCorrectReadDicomWriterMacro( short, int );
		callCorrectReadDicomWriterMacro( short, unsigned long );
		callCorrectReadDicomWriterMacro( short, long );
		callCorrectReadDicomWriterMacro( short, float );
		callCorrectReadDicomWriterMacro( short, double );

		/** From unsigned int to something else. */
		callCorrectReadDicomWriterMacro( unsigned int, unsigned char );
		callCorrectReadDicomWriterMacro( unsigned int, char );
		callCorrectReadDicomWriterMacro( unsigned int, unsigned short );
		callCorrectReadDicomWriterMacro( unsigned int, short );
		callCorrectReadDicomWriterMacro( unsigned int, unsigned int );
		callCorrectReadDicomWriterMacro( unsigned int, int );
		callCorrectReadDicomWriterMacro( unsigned int, unsigned long );
		callCorrectReadDicomWriterMacro( unsigned int, long );
		callCorrectReadDicomWriterMacro( unsigned int, float );
		callCorrectReadDicomWriterMacro( unsigned int, double );

		/** From int to something else. */
		callCorrectReadDicomWriterMacro( int, unsigned char );
		callCorrectReadDicomWriterMacro( int, char );
		callCorrectReadDicomWriterMacro( int, unsigned short );
		callCorrectReadDicomWriterMacro( int, short );
		callCorrectReadDicomWriterMacro( int, unsigned int );
		callCorrectReadDicomWriterMacro( int, int );
		callCorrectReadDicomWriterMacro( int, unsigned long );
		callCorrectReadDicomWriterMacro( int, long );
		callCorrectReadDicomWriterMacro( int, float );
		callCorrectReadDicomWriterMacro( int, double );

		/** From float to something else. */
		callCorrectReadDicomWriterMacro( float, unsigned char );
		callCorrectReadDicomWriterMacro( float, char );
		callCorrectReadDicomWriterMacro( float, unsigned short );
		callCorrectReadDicomWriterMacro( float, short );
		callCorrectReadDicomWriterMacro( float, unsigned int );
		callCorrectReadDicomWriterMacro( float, int );
		callCorrectReadDicomWriterMacro( float, unsigned long );
		callCorrectReadDicomWriterMacro( float, long );
		callCorrectReadDicomWriterMacro( float, float );
		callCorrectReadDicomWriterMacro( float, double );

		/** From double to something else. */
		callCorrectReadDicomWriterMacro( double, unsigned char );
		callCorrectReadDicomWriterMacro( double, char );
		callCorrectReadDicomWriterMacro( double, unsigned short );
		callCorrectReadDicomWriterMacro( double, short );
		callCorrectReadDicomWriterMacro( double, unsigned int );
		callCorrectReadDicomWriterMacro( double, int );
		callCorrectReadDicomWriterMacro( double, unsigned long );
		callCorrectReadDicomWriterMacro( double, long );
		callCorrectReadDicomWriterMacro( double, float );
		callCorrectReadDicomWriterMacro( double, double );
	}
	else
	{
		std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
		std::cerr << "Only 3D images are supported." << std::endl;
		return 1;
	} // end if over inputDimension

	/** Return a value. */
	return 0;

} // end support for SCALAR pixel type
