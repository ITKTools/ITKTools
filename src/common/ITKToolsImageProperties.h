#ifndef __ITKImageProperties_h
#define __ITKImageProperties_h

#include <string>

#include "ITKToolsHelpers.h"

namespace itktools
{
/** Determine componenttype (short, float etc) of an image */
bool GetImageComponentType(
  const std::string & filename,
  ComponentType & componenttype);

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

/** Determine the size of an image.*/
bool GetImageSize(
  itk::ImageIOBase::Pointer imageIOBase,
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
  itk::ImageIOBase::IOPixelType & pixeltype,
  ComponentType & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize );

/** Determine component type,
 * dimension and the number of components from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  ComponentType & componenttype,
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

} // end namespace itktools
#endif