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
#include "ITKToolsHelpers.h"

#include "itkImageIOFactory.h"


namespace itktools
{

/**
 * ***************** GetITKToolsVersion ************************
 */

std::string GetITKToolsVersion( void )
{
  return "0.3.1";
} // end GetITKToolsVersion()


/**
 * ******************* StringIsInteger *******************
 */

bool StringIsInteger( const std::string & argument )
{
  /** Check if the argument is of integer type.
   *  This is done by checking if the string representation contains a '.' character.
   */
  std::basic_string<char>::size_type pos = argument.find( "." );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if( pos == npos )
  {
    return true;
  }

  return false;

} // end StringIsInteger()


/**
 * *************** ReplaceUnderscoreWithSpace ***********************
 */

void ReplaceUnderscoreWithSpace( std::string & arg )
{
  /** Get rid of the possible "_" in arg. */
  std::basic_string<char>::size_type pos = arg.find( "_" );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if( pos != npos )
  {
    arg.replace( pos, 1, " " );
  }

} // end ReplaceUnderscoreWithSpace()


/**
 * *************** ReplaceSpaceWithUnderscore ***********************
 */

void ReplaceSpaceWithUnderscore( std::string & arg )
{
  /** Get rid of the possible " " in arg. */
  std::basic_string<char>::size_type pos = arg.find( " " );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if( pos != npos )
  {
    arg.replace( pos, 1, "_" );
  }

} // end ReplaceSpaceWithUnderscore()


/**
 * *************** RemoveUnsignedFromString ***********************
 */

void RemoveUnsignedFromString( std::string & arg )
{
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  std::basic_string<char>::size_type pos = arg.find( "unsigned " );
  if( pos != npos ) arg = arg.substr( pos + 9 );
  pos = arg.find( "unsigned_" );
  if( pos != npos ) arg = arg.substr( pos + 9 );

} // end RemoveUnsignedFromString()


/**
 * ******************* ComponentTypeIsInteger *******************
 */

bool ComponentTypeIsInteger( const itk::ImageIOBase::IOComponentType & componentType )
{
  /** Check if the input image is of integer type. */
  bool componentIsInteger = false;
  if( componentType == itk::ImageIOBase::UCHAR  || componentType == itk::ImageIOBase::CHAR
    || componentType == itk::ImageIOBase::USHORT || componentType == itk::ImageIOBase::SHORT
    || componentType == itk::ImageIOBase::UINT   || componentType == itk::ImageIOBase::INT
    || componentType == itk::ImageIOBase::ULONG  || componentType == itk::ImageIOBase::LONG )
  {
    componentIsInteger = true;
  }

  return componentIsInteger;

} // end ComponentTypeIsInteger()


/**
 * *************** ComponentTypeIsValid ***********************
 */

bool ComponentTypeIsValid( const itk::ImageIOBase::IOComponentType & componentType )
{
  /** Check argument. */
  if( componentType == itk::ImageIOBase::UCHAR
    || componentType == itk::ImageIOBase::CHAR
    || componentType == itk::ImageIOBase::USHORT
    || componentType == itk::ImageIOBase::SHORT
    || componentType == itk::ImageIOBase::UINT
    || componentType == itk::ImageIOBase::INT
    || componentType == itk::ImageIOBase::ULONG
    || componentType == itk::ImageIOBase::LONG
    || componentType == itk::ImageIOBase::FLOAT
    || componentType == itk::ImageIOBase::DOUBLE )
  {
    return true;
  }

  return false;

} // end ComponentTypeIsValid()


/**
 * *************** RemoveUnsignedFromComponentType ***********************
 */

itk::ImageIOBase::IOComponentType RemoveUnsignedFromComponentType(
  const itk::ImageIOBase::IOComponentType & componentType )
{
  if( componentType == itk::ImageIOBase::UCHAR )
  {
    return itk::ImageIOBase::CHAR;
  }
  else if( componentType == itk::ImageIOBase::UINT )
  {
    return itk::ImageIOBase::INT;
  }
  else if( componentType == itk::ImageIOBase::USHORT )
  {
    return itk::ImageIOBase::SHORT;
  }
  else if( componentType == itk::ImageIOBase::ULONG )
  {
    return itk::ImageIOBase::LONG;
  }

  return componentType;

} // end RemoveUnsignedFromComponentType()


/**
 * *************** GetLargestComponentType ***********************
 */

itk::ImageIOBase::IOComponentType GetLargestComponentType(
  const itk::ImageIOBase::IOComponentType & type1,
  const itk::ImageIOBase::IOComponentType & type2 )
{
  /** Typedef's. */
  typedef std::map< itk::ImageIOBase::IOComponentType, unsigned int > RankingType;
  typedef RankingType::value_type               EntryType;

  /** Define the ranking. */
  RankingType ranking;
  ranking.insert( EntryType( itk::ImageIOBase::CHAR,   1 ) );
  ranking.insert( EntryType( itk::ImageIOBase::SHORT,  2 ) );
  ranking.insert( EntryType( itk::ImageIOBase::INT,    3 ) );
  ranking.insert( EntryType( itk::ImageIOBase::LONG,   4 ) );
  ranking.insert( EntryType( itk::ImageIOBase::FLOAT,  5 ) );
  ranking.insert( EntryType( itk::ImageIOBase::DOUBLE, 6 ) );

  /** Remove unsigned. */
  itk::ImageIOBase::IOComponentType type1Cleaned = RemoveUnsignedFromComponentType( type1 );
  itk::ImageIOBase::IOComponentType type2Cleaned = RemoveUnsignedFromComponentType( type2 );

  /** Determine which one is the largest. */
  itk::ImageIOBase::IOComponentType outputComponentType;
  if( type1Cleaned == type2Cleaned )
  {
    outputComponentType = type1;
  }
  else
  {
    outputComponentType = ranking[ type1Cleaned ] > ranking[ type2Cleaned ] ? type1 : type2;
  }

  /** Return a value. */
  return outputComponentType;

} // end GetLargestComponentType()


/**
 * *************** IsFilterSupportedCheck ***********************
 */

bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentType & inputType )
{
  if( !filter )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!\n"
      << "  dimension =              " << dim << "\n"
      << "  pixel (component) type = " << itk::ImageIOBase::GetComponentTypeAsString( inputType )
      << std::endl;
    return false;
  }

  return true;

} // end IsFilterSupportedCheck()


/**
 * *************** IsFilterSupportedCheck ***********************
 */

bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentType & inputType,
  const itk::ImageIOBase::IOComponentType & outputType )
{
  if( !filter )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!\n"
      << "  dimension =                     " << dim << "\n"
      << "  input  pixel (component) type = " << itk::ImageIOBase::GetComponentTypeAsString( inputType ) << "\n"
      << "  output pixel (component) type = " << itk::ImageIOBase::GetComponentTypeAsString( outputType )
      << std::endl;
    return false;
  }

  return true;

} // end IsFilterSupportedCheck()


/**
 * *************** IsFilterSupportedCheck ***********************
 */

bool IsFilterSupportedCheck(
  const ITKToolsBase * filter,
  const unsigned int & dim,
  const itk::ImageIOBase::IOComponentType & inputType1,
  const itk::ImageIOBase::IOComponentType & inputType2,
  const itk::ImageIOBase::IOComponentType & outputType )
{
  if( !filter )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!\n"
      << "  dimension =                     " << dim << "\n"
      << "  input  pixel (component) type 1 = " << itk::ImageIOBase::GetComponentTypeAsString( inputType1 ) << "\n"
      << "  input  pixel (component) type 2 = " << itk::ImageIOBase::GetComponentTypeAsString( inputType2 ) << "\n"
      << "  output pixel (component) type   = " << itk::ImageIOBase::GetComponentTypeAsString( outputType )
      << std::endl;
    return false;
  }

  return true;

} // end IsFilterSupportedCheck()


/**
 * *************** NumberOfComponentsCheck ***********************
 */

bool NumberOfComponentsCheck( const unsigned int & numberOfComponents )
{
  if( numberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "  Vector images are not supported for this filter." << std::endl;
    return false;
  }

  return true;

} // end NumberOfComponentsCheck()


} // end itktools namespace
