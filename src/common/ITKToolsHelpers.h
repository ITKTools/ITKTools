#ifndef __ITKToolsHelpers_h
#define __ITKToolsHelpers_h

#include "itkImageIOBase.h"

namespace itktools
{

/** Provide a shorter name for the component type enum */
typedef itk::ImageIOBase::IOComponentType ComponentType;

/** This is a wrapper function that converts a string to lower case
 *  and removes any underscores before trying to convert to the enum.
 */
ComponentType GetComponentTypeFromString(std::string);

/** Determine the component type of an image. */
ComponentType GetImageComponentType(
  const std::string & filename);

/** Test if a ComponentType corresponds to the template parameter */
template <class T>
bool IsType( ComponentType ct )
{
  return ct == itk::ImageIOBase::MapPixelType<T>::CType;
}

bool StringIsInteger( const std::string & argument );

bool ComponentTypeIsInteger( const itktools::ComponentType inputComponentType);

/** Replace a underscore in the input string with a space. */
void ReplaceUnderscoreWithSpace( std::string & arg );

/** Replace a space in the input string with an underscore. */
void ReplaceSpaceWithUnderscore( std::string & arg );

/** Remove "unsigned " or "unsigned_" from the input string. */
void RemoveUnsignedFromString( std::string & arg );

/** Remove "unsigned " or "unsigned_" from the input string. */
ComponentType GetUnsignedComponentType( const ComponentType componentType );

/** Check for a valid component type. */
bool ComponentTypeIsValid( const ComponentType arg );

/** Determine pixeltype (scalar/vector) of an image */
bool GetImagePixelType(
  const std::string & filename,
  std::string & pixeltype);


/** Selects the largest type of the two. The order is:
 * char < short < int < long < float < double.
 */
ComponentType GetLargestComponentType(
  const ComponentType & type1, const ComponentType & type2 );

} // end itktools namespace

#include "ITKToolsImageProperties.h" // functions from this header are not used here, but we always want to also include this when we include Helpers

#endif