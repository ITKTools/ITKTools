#ifndef __CommandLineArgumentHelper_h
#define __CommandLineArgumentHelper_h

#include <string>
#include <vector>
#include <map>

/** Replace a underscore in the input string with a space. */
void ReplaceUnderscoreWithSpace( std::string & arg );

/** Replace a space in the input string with an underscore. */
void ReplaceSpaceWithUnderscore( std::string & arg );

/** Remove "unsigned " or "unsigned_" from the input string. */
void RemoveUnsignedFromString( std::string & arg );

/** Check for a valid component type. */
bool CheckForValidComponentType( const std::string & arg );

/** Determine pixeltype (scalar/vector), componenttype (short, float etc),
 * dimension and numberofcomponents from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize );

/** Determine pixeltype (scalar/vector), componenttype (short, float etc),
 * dimension and numberofcomponents from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize,
  std::vector<double> & imagespacing,
  std::vector<double> & imageoffset );

/** Selects the largest type of the two. The order is:
 * char < short < int < long < float < double.
 */
std::string GetLargestComponentType(
  const std::string & type1, const std::string & type2 );


#endif // end #ifndef __CommandLineArgumentHelper_h

