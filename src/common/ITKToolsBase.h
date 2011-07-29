#ifndef __ITKToolsBase_h
#define __ITKToolsBase_h

namespace itktools
{


/** \class ITKToolsBase
 * Base class for all ITKTools applications.
 */

class ITKToolsBase
{
public:
  ITKToolsBase(){};
  virtual ~ITKToolsBase(){};

  virtual void Run( void ) = 0;

};

} // end namespace

#endif //__ITKToolsBase_h
