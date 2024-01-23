/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __ITKToolsImageProperties_h_
#define __ITKToolsImageProperties_h_

#include <string>
#include "itkImageIOBase.h"


namespace itktools
{

/** Determine pixeltype (scalar/vector) of an image */
bool GetImagePixelType(
  const std::string & filename,
  std::string & pixeltype );

/** Determine componenttype (short, float etc) of an image */
bool GetImageComponentType(
  const std::string & filename,
  itk::ImageIOBase::IOComponentEnum & componenttype );

/** Determine the component type of an image. */
itk::ImageIOBase::IOComponentEnum GetImageComponentType(
  const std::string & filename );

/** Determine the number of components of each pixel in an image. */
bool GetImageNumberOfComponents(
  const std::string & filename,
  unsigned int & numberofcomponents );

/** Determine dimension of an image */
bool GetImageDimension(
  const std::string & filename,
  unsigned int & dimension );

/** Determine the size of an image.*/
bool GetImageSize(
  const std::string & filename,
  std::vector<unsigned int> & imagesize );

/** Determine the size of an image.*/
bool GetImageSize(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<unsigned int> & imageSize );

/** Determine the origin of an image.*/
bool GetImageOrigin(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageOrigin );

/** Determine the spacing of an image.*/
bool GetImageSpacing(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageSpacing );

/** Determine the direction of an image.*/
bool GetImageDirection(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageDirection );

/** Determine pixeltype (scalar/vector), componenttype (short, float etc),
 * dimension and numberofcomponents from an image
 * returns 0 when successful. 0 otherwise.
 */
bool GetImageProperties(
  const std::string & filename,
  itk::IOPixelEnum & pixeltype,
  itk::ImageIOBase::IOComponentEnum & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents );

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
bool GetImageProperties(
  const std::string & filename,
  itk::IOPixelEnum & pixeltype,
  itk::ImageIOBase::IOComponentEnum & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize );

bool GetImageProperties(
  const std::string & filename,
  itk::IOPixelEnum & pixeltype,
  itk::ImageIOBase::IOComponentEnum & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & size,
  std::vector<double> & spacing,
  std::vector<double> & origin,
  std::vector<double> & direction );

/** Determine component type,
 * dimension and the number of components from an image
 * returns 0 when successful. 0 otherwise.
 */
int GetImageProperties(
  const std::string & filename,
  itk::ImageIOBase::IOComponentEnum & componenttype,
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

#endif // end #ifndef __ITKToolsImageProperties_h_
