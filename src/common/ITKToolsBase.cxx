#include <algorithm>    // transform
#include <functional>   // ptr_fun
#include <cctype>       // toupper, tolower

#include "ITKToolsBase.h"

#include "itkImageIOFactory.h"

namespace itktools {
EnumComponentType EnumComponentTypeFromString(std::string typeString)
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
    
  if(typeString.compare("uchar")==0 )
    {
    return itk::ImageIOBase::UCHAR;
    }
  else if(typeString.compare("char")==0 )
    {
    return itk::ImageIOBase::CHAR;
    }
  else if(typeString.compare("ushort")==0 )
    {
    return itk::ImageIOBase::USHORT;
    }
  else if(typeString.compare("short")==0 )
    {
    return itk::ImageIOBase::SHORT;
    }
  else if(typeString.compare("uint")==0 )
    {
    return itk::ImageIOBase::UINT;
    }
  else if(typeString.compare("int")==0 || typeString.compare("integer")==0 )
    {
    return itk::ImageIOBase::INT;
    }
  else if(typeString.compare("ulong")==0 )
    {
    return itk::ImageIOBase::ULONG;
    }
  else if(typeString.compare("long")==0)
    {
    return itk::ImageIOBase::LONG;
    }
  else if(typeString.compare("float")==0)
    {
    return itk::ImageIOBase::FLOAT;
    }
  else if(typeString.compare("double")==0)
    {
    return itk::ImageIOBase::DOUBLE;
    }
  else
    {
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
    }
}

/**
 * ***************** GetImageComponentType ************************
 */
itktools::EnumComponentType GetImageComponentType(const std::string & filename)
{
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(
    filename.c_str(), itk::ImageIOFactory::ReadMode);
  if ( imageIO.IsNull() )
  {
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE; // complain
  }
  imageIO->SetFileName( filename.c_str() );
  imageIO->ReadImageInformation();
  itktools::EnumComponentType componentType = imageIO->GetComponentType();
  
  return componentType;
}

} // end itktools namespace