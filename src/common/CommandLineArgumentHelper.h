#ifndef __CommandLineArgumentHelper_h
#define __CommandLineArgumentHelper_h

#include <string>

/** Replace underscores in the input string with spaces */
void ReplaceUnderscoreWithSpace( std::string & arg );

/** Determine pixeltype (scalar/vector), componenttype (short, float etc),
 * dimension and numberofcomponents from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents);

#endif // end #ifndef __CommandLineArgumentHelper_h

