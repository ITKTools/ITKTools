#ifndef __CommandLineArgumentHelper_h
#define __CommandLineArgumentHelper_h

// STL
#include <string>
#include <vector>
#include <map>

// ITK
#include "itkImageIOBase.h"

// ITKTools
#include "ITKToolsBase.h"

/** Replace a underscore in the input string with a space. */
void ReplaceUnderscoreWithSpace( std::string & arg );

/** Replace a space in the input string with an underscore. */
void ReplaceSpaceWithUnderscore( std::string & arg );

/** Remove "unsigned " or "unsigned_" from the input string. */
void RemoveUnsignedFromString( std::string & arg );

/** Check for a valid component type. */
bool CheckForValidComponentType( const std::string & arg );

/** Determine pixeltype (scalar/vector) of an image */
bool GetImagePixelType(
  const std::string & filename,
  std::string & pixeltype);

/** Determine componenttype (short, float etc) of an image */
bool GetImageComponentType(
  const std::string & filename,
  itktools::EnumComponentType & componenttype);

/** Determine dimension of an image */
bool GetImageDimension(
  const std::string & filename,
  unsigned int & dimension);

/** Determine the number of components of each pixel in an image. */
bool GetImageNumberOfComponents(
  const std::string & filename,
  unsigned int & numberofcomponents);

/** Determine the size of an image.*/
bool GetImageSize(
  const std::string & filename,
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
  std::vector<unsigned int> & imagesize );

/** Determine pixeltype (scalar/vector), componenttype (short, float etc),
 * dimension and numberofcomponents from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  std::string & pixelTypeAsString,
  std::string & componentTypeAsString,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & size,
  std::vector<double> & spacing,
  std::vector<double> & origin,
  std::vector<double> & direction );

/** Determine image properties, returning an ImageIOBase. */
bool GetImageIOBase(
  const std::string & filename,
  itk::ImageIOBase::Pointer & imageIOBase );

/** Fill an ImageIOBase with values. */
void FillImageIOBase(
  itk::ImageIOBase::Pointer & imageIOBase,
  const std::string & pixelTypeAsString,
  const std::string & componentTypeAsString,
  const unsigned int & imageDimension,
  const unsigned int & numberOfComponents,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction );

/** Fill vectors, etc with information from ImageIOBase. */
bool GetImageInformationFromImageIOBase(
  const itk::ImageIOBase::Pointer & imageIOBase,
  std::string & pixelTypeAsString,
  std::string & componentTypeAsString,
  unsigned int & imageDimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & size,
  std::vector<double> & spacing,
  std::vector<double> & origin,
  std::vector<double> & direction );

/** Selects the largest type of the two. The order is:
 * char < short < int < long < float < double.
 */
std::string GetLargestComponentType(
  const std::string & type1, const std::string & type2 );

#include "CommandLineArgumentHelper.txx"

#endif // end #ifndef __CommandLineArgumentHelper_h
