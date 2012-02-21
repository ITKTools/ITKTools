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
#ifndef __itkSimpleBoxSpatialFunction_h_
#define __itkSimpleBoxSpatialFunction_h_

#include "itkInteriorExteriorSpatialFunction.h"

namespace itk
{

/** \class SimpleBoxSpatialFunction
 * \brief Spatial function implementation of a sphere
 *
 * Implements a function that returns 0 for points inside or on the surface
 * of a sphere, 1 for points outside the sphere
 *
 * \ingroup SpatialFunctions
 */
template <unsigned int VImageDimension=3,typename TInput=Point<double,VImageDimension> >
class ITK_EXPORT SimpleBoxSpatialFunction
: public InteriorExteriorSpatialFunction<VImageDimension,TInput>
{
public:
  /** Standard class typedefs. */
  typedef SimpleBoxSpatialFunction<VImageDimension,TInput> Self;
  typedef InteriorExteriorSpatialFunction<VImageDimension,TInput> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleBoxSpatialFunction,InteriorExteriorSpatialFunction);

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  /** Input type for the function. */
  typedef typename Superclass::InputType InputType;

  /** Output type for the function. */
  typedef typename Superclass::OutputType OutputType;

  /** Evaluates the function at a given position */
  OutputType Evaluate(const InputType& position) const;

  /** Get and set the first point that defines the box. */
  itkGetMacro( PointA, InputType);
  itkSetMacro( PointA, InputType);

  /** Get and set the second point that defines the box */
  itkGetMacro( PointB, InputType);
  itkSetMacro( PointB, InputType);

protected:
  SimpleBoxSpatialFunction();
  virtual ~SimpleBoxSpatialFunction();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  SimpleBoxSpatialFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The first point that defines the box */
  InputType m_PointA;

  /** The second point that defines the box */
  InputType m_PointB;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSimpleBoxSpatialFunction.txx"
#endif

#endif // end #ifndef __itkSimpleBoxSpatialFunction_h_
