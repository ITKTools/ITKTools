#include "ITKToolsHelpers.h"

#include "itkImageIOFactory.h"
#include <algorithm> // std::transform
#include <cctype> // std::tolower (there is another one in <locale>)


namespace itktools
{

ComponentType GetComponentTypeFromString(std::string typeString)
{
  // Transform the string to all lower case
  std::transform(typeString.begin(), typeString.end(), typeString.begin(),
                   std::ptr_fun<int,int>(std::tolower) );

  /** Get rid of "nsigned " and "nsigned_". */
  const std::string::size_type npos = std::string::npos;
  std::string::size_type pos = typeString.find( "nsigned " );
  if ( pos != npos ) typeString = typeString.substr( pos + 8 );
  pos = typeString.find( "nsigned_" );
  if ( pos != npos ) typeString = typeString.substr( pos + 8 );
  pos = typeString.find( "nsigned" );
  if ( pos != npos ) typeString = typeString.substr( pos + 7 );

  return itk::ImageIOBase::GetComponentTypeFromString(typeString);
}


/**
 * ***************** GetImageComponentType ************************
 */

itktools::ComponentType GetImageComponentType(const std::string & filename)
{
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(
    filename.c_str(), itk::ImageIOFactory::ReadMode);
  if ( imageIO.IsNull() )
  {
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE; // complain
  }
  imageIO->SetFileName( filename.c_str() );
  imageIO->ReadImageInformation();
  itktools::ComponentType componentType = imageIO->GetComponentType();

  return componentType;
}

/**
  * ******************* TypeIsInteger *******************
  */
#if 0
bool TypeIsInteger( const std::string & componentType )
{
  /** Make sure the input has "_" instead of " ". */
  std::string compType = componentType;
  itktools::ReplaceSpaceWithUnderscore( compType );

  /** Check if the input image is of integer type. */
  bool typeIsInteger = false;
  if ( compType == "unsigned_char" || compType == "char"
    || compType == "unsigned_short" || compType == "short"
    || compType == "unsigned_int" || compType == "int"
    || compType == "unsigned_long" || compType == "long" )
  {
    typeIsInteger = true;
  }
  return typeIsInteger;
} // end TypeIsInteger()
#endif


/**
 * ******************* InputIsInteger *******************
 */

bool ComponentTypeIsInteger( const itktools::ComponentType inputComponentType)
{
  /** Check if the input image is of integer type. */
  bool inputIsInteger = false;
  if ( inputComponentType == itk::ImageIOBase::UCHAR || inputComponentType == itk::ImageIOBase::CHAR
    || inputComponentType == itk::ImageIOBase::USHORT || inputComponentType == itk::ImageIOBase::SHORT
    || inputComponentType == itk::ImageIOBase::UINT || inputComponentType == itk::ImageIOBase::INT
    || inputComponentType == itk::ImageIOBase::ULONG || inputComponentType == itk::ImageIOBase::LONG )
  {
    inputIsInteger = true;
  }

  return inputIsInteger;
} // end InputIsInteger()


/**
 * ******************* ArgumentIsInteger *******************
 */

bool StringIsInteger( const std::string & argument )
{
  /** Check if the argument is of integer type.
   *  This is done by checking if the string representation contains a '.' character.
   */
  std::basic_string<char>::size_type pos = argument.find( "." );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos == npos )
  {
    return true;
  }

  return false;

} // end ArgumentIsInteger()


/**
 * *************** ReplaceUnderscoreWithSpace ***********************
 */

void ReplaceUnderscoreWithSpace( std::string & arg )
{
  /** Get rid of the possible "_" in arg. */
  std::basic_string<char>::size_type pos = arg.find( "_" );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos )
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
  if ( pos != npos )
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
  if ( pos != npos ) arg = arg.substr( pos + 9 );
  pos = arg.find( "unsigned_" );
  if ( pos != npos ) arg = arg.substr( pos + 9 );

} // end RemoveUnsignedFromString()


/**
 * *************** ComponentTypeIsValid ***********************
 */

bool ComponentTypeIsValid( const ComponentType & componentType )
{
  /** Check argument. */
  if ( componentType == itk::ImageIOBase::UCHAR
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
 * *************** GetUnsignedComponentType ***********************
 */

ComponentType GetUnsignedComponentType( const ComponentType & componentType )
{
  if(componentType == itk::ImageIOBase::UCHAR)
  {
    return itk::ImageIOBase::CHAR;
  }
  else if(componentType == itk::ImageIOBase::UINT)
  {
    return itk::ImageIOBase::INT;
  }
  else if(componentType == itk::ImageIOBase::USHORT)
  {
    return itk::ImageIOBase::SHORT;
  }
  else if(componentType == itk::ImageIOBase::ULONG)
  {
    return itk::ImageIOBase::LONG;
  }
  else
  {
    return componentType;
  }
} // end RemoveUnsignedFromComponentType()


/**
 * *************** GetLargestComponentType ***********************
 */

ComponentType GetLargestComponentType(
  const ComponentType & type1, const ComponentType & type2 )
{
  /** Typedef's. */
  typedef std::map< ComponentType, unsigned int > RankingType;
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
  ComponentType type1Cleaned = GetUnsignedComponentType( type1 );
  ComponentType type2Cleaned = GetUnsignedComponentType( type2 );
  
  /** Determine which one is the largest. */
  ComponentType outputComponentType;
  if ( type1Cleaned == type2Cleaned )
  {
    outputComponentType = type1;
  }
  else
  {
    outputComponentType = ranking[ type1Cleaned ] > ranking[ type2Cleaned ] ? type1Cleaned : type2Cleaned;
  }

  /** Return a value. */
  return outputComponentType;

} // end GetLargestComponentType()


} // end itktools namespace