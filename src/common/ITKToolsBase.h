#ifndef __ITKToolsBase_h
#define __ITKToolsBase_h

#include "itkImageIOBase.h"

namespace itktools {

typedef itk::ImageIOBase::IOComponentType EnumComponentType;

/** IsType
 * Test if an EnumComponentType corresponds to the template parameter
 */
template <class T>
bool IsType( EnumComponentType ct )
{
  return ct == itk::ImageIOBase::MapPixelType<T>::CType;
}

/** \class ITKToolsBase
 * Base class. Limited functionality for now.
 */
class ITKToolsBase
{
public:
  ITKToolsBase(){};
  virtual ~ITKToolsBase(){};

  virtual void Run(void) {};

};

} // end namespace

#endif __ITKToolsBase_h