#include "mainhelper2.h"

bool Morphology2D(
  const std::string & componentType,
  const unsigned int & Dimension,
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & operation,
  const std::string & type,
  const std::string & boundaryCondition,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const int & algorithm,
	const bool useCompression)
{
  bool supported = false;

  /** Erosion. */
  run( erosion, unsigned char, 2 );
  run( erosion, char, 2 );
  run( erosion, unsigned short, 2 );
  run( erosion, short, 2 );

  /** Dilation. */
  run( dilation, unsigned char, 2 );
  run( dilation, char, 2 );
  run( dilation, unsigned short, 2 );
  run( dilation, short, 2 );

  /** Opening. */
  run( opening, unsigned char, 2 );
  run( opening, char, 2 );
  run( opening, unsigned short, 2 );
  run( opening, short, 2 );

  /** Closing. */
  run( closing, unsigned char, 2 );
  run( closing, char, 2 );
  run( closing, unsigned short, 2 );
  run( closing, short, 2 );

  /** Gradient. */
  run2( gradient, unsigned char, 2 );
  run2( gradient, char, 2 );
  run2( gradient, unsigned short, 2 );
  run2( gradient, short, 2 );

  /** Return a value. */
  return supported;

} // end support for 2D images
