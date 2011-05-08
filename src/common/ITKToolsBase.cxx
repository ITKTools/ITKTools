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

  if(typeString.compare("uchar")==0 || typeString.compare("unsignedchar")==0 || typeString.compare("unsigned_char")==0)
    {
    return itk::ImageIOBase::UCHAR;
    }
  else if(typeString.compare("char")==0)
    {
    return itk::ImageIOBase::CHAR;
    }
  else if(typeString.compare("ushort")==0 || typeString.compare("unsignedshort")==0 || typeString.compare("unsigned_short")==0)
    {
    return itk::ImageIOBase::USHORT;
    }
  else if(typeString.compare("short")==0)
    {
    return itk::ImageIOBase::SHORT;
    }
  else if(typeString.compare("uint")==0 || typeString.compare("unsignedint")==0 || typeString.compare("unsigned_int")==0)
    {
    return itk::ImageIOBase::UINT;
    }
  else if(typeString.compare("int")==0 || typeString.compare("integer")==0)
    {
    return itk::ImageIOBase::INT;
    }
  else if(typeString.compare("ulong")==0 || typeString.compare("unsignedlong")==0 || typeString.compare("unsigned_long")==0)
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
 * ***************** FillImageIOBase ************************
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