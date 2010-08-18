#include "mainhelper2.h"

bool Morphology3D(
  const std::string & componentType,
  const unsigned int & Dimension,
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & operation,
  const std::string & type,
  const std::string & boundaryCondition,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const int & algorithm )
{
  bool supported = false;

  /** Erosion. */
  run( erosion, unsigned char, 3 );
  run( erosion, char, 3 );
  run( erosion, unsigned short, 3 );
  run( erosion, short, 3 );

  /** Dilation. */
  run( dilation, unsigned char, 3 );
  run( dilation, char, 3 );
  run( dilation, unsigned short, 3 );
  run( dilation, short, 3 );

  /** Opening. */
  run( opening, unsigned char, 3 );
  run( opening, char, 3 );
  run( opening, unsigned short, 3 );
  run( opening, short, 3 );

  /** Closing. */
  run( closing, unsigned char, 3 );
  run( closing, char, 3 );
  run( closing, unsigned short, 3 );
  run( closing, short, 3 );

  /** Gradient. */
  run2( gradient, unsigned char, 3 );
  run2( gradient, char, 3 );
  run2( gradient, unsigned short, 3 );
  run2( gradient, short, 3 );

  /** Return a value. */
  return supported;

} // end support for 3D images
