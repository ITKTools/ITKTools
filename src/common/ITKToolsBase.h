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
#ifndef __ITKToolsBase_h_
#define __ITKToolsBase_h_


namespace itktools
{

/** Macro's to define an object creation method.
 * When used throughout the toolkit it unifies the way filters are instantiated.
 */

  // add macro for only component type
  // add macro for only dimension

#define itktoolsOneTypeNewMacro( object )                                       \
static object * New( unsigned int dim,                                          \
  itk::ImageIOBase::IOComponentEnum componentType )                             \
{                                                                               \
  if( VDimension == dim                                                         \
    && itktools::IsType<TComponentType>( componentType ) )                      \
  {                                                                             \
    return new object;                                                          \
  }                                                                             \
  return 0;                                                                     \
}

#define itktoolsTwoTypeNewMacro( object )                                       \
static object * New( unsigned int dim,                                          \
  itk::ImageIOBase::IOComponentEnum inputComponentType,                         \
  itk::ImageIOBase::IOComponentEnum outputComponentType )                       \
{                                                                               \
  if( VDimension == dim                                                         \
    && itktools::IsType<TInputComponentType>( inputComponentType )              \
    && itktools::IsType<TOutputComponentType>( outputComponentType ) )          \
  {                                                                             \
    return new object;                                                          \
  }                                                                             \
  return 0;                                                                     \
}

#define itktoolsThreeTypeNewMacro( object )                                     \
static object * New( unsigned int dim,                                          \
  itk::ImageIOBase::IOComponentEnum inputComponentType1,                        \
  itk::ImageIOBase::IOComponentEnum inputComponentType2,                        \
  itk::ImageIOBase::IOComponentEnum outputComponentType )                       \
{                                                                               \
  if( VDimension == dim                                                         \
    && itktools::IsType<TInputComponentType1>( inputComponentType1 )            \
    && itktools::IsType<TInputComponentType2>( inputComponentType2 )            \
    && itktools::IsType<TOutputComponentType>( outputComponentType ) )          \
  {                                                                             \
    return new object;                                                          \
  }                                                                             \
  return 0;                                                                     \
}


/** \class ITKToolsBase
 * \brief Base class for all ITKTools applications.
 */

class ITKToolsBase
{
public:
  ITKToolsBase(){};
  virtual ~ITKToolsBase(){};

  /** All sub-classes should overwrite Run() to implement functionality. */
  virtual void Run( void ) = 0;

}; // end class ITKToolsBase()

} // end namespace itktools

#endif //__ITKToolsBase_h_
