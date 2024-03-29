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
#ifndef __ITKToolsHelpers_h_
#define __ITKToolsHelpers_h_

#include "itkImageIOBase.h"
#include "ITKToolsBase.h"

// functions from this header are not used here, but we always
// want to also include this when we include Helpers
#include "ITKToolsImageProperties.h"


namespace itktools
{

/** Return the version number. */
std::string GetITKToolsVersion( void );

/** Test if a ComponentType corresponds to the template parameter. */
template <class T>
bool IsType( itk::ImageIOBase::IOComponentEnum ct )
{
  return ct == itk::ImageIOBase::MapPixelType<T>::CType;
}

/** StringIsInteger. Check if string contains a dot ".". */
bool StringIsInteger( const std::string & argument );

/** Replace a underscore in the input string with a space. */
void ReplaceUnderscoreWithSpace( std::string & arg );

/** Replace a space in the input string with an underscore. */
void ReplaceSpaceWithUnderscore( std::string & arg );

/** Remove "unsigned " or "unsigned_" from the input string. */
void RemoveUnsignedFromString( std::string & arg );

/** ComponentTypeIsInteger. */
bool ComponentTypeIsInteger(
  const itk::ImageIOBase::IOComponentEnum & inputComponentType );

/** Remove "unsigned " or "unsigned_" from the component type. */
itk::ImageIOBase::IOComponentEnum RemoveUnsignedFromComponentType(
  const itk::ImageIOBase::IOComponentEnum & componentType );

/** Check for a valid component type. */
bool ComponentTypeIsValid( const itk::ImageIOBase::IOComponentEnum & arg );

/** Selects the largest type of the two. The order is:
 * char < short < int < long < float < double.
 */
itk::ImageIOBase::IOComponentEnum GetLargestComponentType(
  const itk::ImageIOBase::IOComponentEnum & type1,
  const itk::ImageIOBase::IOComponentEnum & type2 );

/** IsFilterSupportedCheck. Unify error message printing. */
bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentEnum & inputType );

/** IsFilterSupportedCheck. Unify error message printing. */
bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentEnum & inputType,
  const itk::ImageIOBase::IOComponentEnum & outputType );

/** IsFilterSupportedCheck. Unify error message printing. */
bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentEnum & inputType1,
  const itk::ImageIOBase::IOComponentEnum & inputType2,
  const itk::ImageIOBase::IOComponentEnum & outputType );

/** NumberOfComponentsCheck. Unify error message printing. */
bool NumberOfComponentsCheck( const unsigned int & numberOfComponents );

} // end itktools namespace

#endif // end #ifndef __ITKToolsHelpers_h_
