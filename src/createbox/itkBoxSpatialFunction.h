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
#ifndef __itkBoxSpatialFunction_h_
#define __itkBoxSpatialFunction_h_

#include "itkInteriorExteriorSpatialFunction.h"
#include "itkEuler2DTransform.h"
#include "itkEuler3DTransform.h"

namespace itk
{

/** \class BoxSpatialFunction
 * \brief Spatial function implementation of a sphere
 *
 * Implements a function that returns 0 for points inside or on the surface
 * of a sphere, 1 for points outside the sphere
 *
 * \ingroup SpatialFunctions
 */
template < unsigned int VImageDimension = 3,
  typename TInput = Point< double, VImageDimension > >
class ITK_EXPORT BoxSpatialFunction
: public InteriorExteriorSpatialFunction<VImageDimension,TInput>
{
public:
  /** Standard class typedefs. */
  typedef BoxSpatialFunction<VImageDimension,TInput>    Self;
  typedef InteriorExteriorSpatialFunction<
    VImageDimension,TInput>                             Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BoxSpatialFunction, InteriorExteriorSpatialFunction );

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  /** Input type for the function. */
  typedef typename Superclass::InputType InputType;

  /** Output type for the function. */
  typedef typename Superclass::OutputType OutputType;

  /** Typedefs for the orientation. */
  typedef Euler2DTransform< double >      Euler2DTransformType;
  typedef typename Euler2DTransformType::InputPointType InputPointType2D;
  typedef typename Euler2DTransformType::OutputPointType OutputPointType2D;
  typedef Euler3DTransform< double >      Euler3DTransformType;
  typedef typename Euler3DTransformType::InputPointType InputPointType3D;
  typedef typename Euler3DTransformType::OutputPointType OutputPointType3D;

  /** Set the center of the box. */
  virtual void SetCenter( const InputType _arg );

  /** Get the center of the box. */
  itkGetMacro( Center, InputType );

  /** Set the radius lengths of the box. */
  itkSetMacro( Radius, InputType );

  /** Get the radius lengths of the box. */
  itkGetMacro( Radius, InputType );

  /** Set the orientation vector, which are Euler angles. */
  virtual void SetOrientation( const InputType _arg );

  /** Evaluates the function at a given position. Returns a boolean. */
  OutputType Evaluate( const InputType & position ) const;

protected:
  BoxSpatialFunction();
  virtual ~BoxSpatialFunction();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  BoxSpatialFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The center of the box. */
  InputType m_Center;

  /** The radius lenths of the box. */
  InputType m_Radius;

  /** The orientation. */
  InputType m_Orientation;
  typename Euler2DTransformType::Pointer  m_Euler2DTransform;
  typename Euler3DTransformType::Pointer  m_Euler3DTransform;

}; // end class BoxSpatialFunction

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBoxSpatialFunction.txx"
#endif

#endif // end #ifndef __itkBoxSpatialFunction_h_
