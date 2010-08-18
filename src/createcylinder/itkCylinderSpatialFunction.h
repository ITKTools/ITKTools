/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCylinderSpatialFunction.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:24:12 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCylinderSpatialFunction_h
#define __itkCylinderSpatialFunction_h

#include "itkInteriorExteriorSpatialFunction.h"

namespace itk
{

/** \class CylinderSpatialFunction
 * \brief Spatial function implementation of a sphere
 *
 * Implements a function that returns 0 for points inside or on the surface
 * of a sphere, 1 for points outside the sphere
 *
 * \ingroup SpatialFunctions
 */
template <unsigned int VImageDimension=3,typename TInput=Point<double,VImageDimension> >
class ITK_EXPORT CylinderSpatialFunction
: public InteriorExteriorSpatialFunction<VImageDimension,TInput>
{
public:
  /** Standard class typedefs. */
  typedef CylinderSpatialFunction<VImageDimension,TInput>           Self;
  typedef InteriorExteriorSpatialFunction<VImageDimension,TInput> Superclass;
  typedef SmartPointer<Self>                                      Pointer;
  typedef SmartPointer<const Self>                                ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(CylinderSpatialFunction,InteriorExteriorSpatialFunction);

  /** Input type for the function. */
  typedef typename Superclass::InputType InputType;

  /** Output type for the function. */
  typedef typename Superclass::OutputType OutputType;

  /** Evaluates the function at a given position */
  OutputType Evaluate(const InputType& position) const;

  /** Get and set the center of the sphere. */
  itkGetConstMacro( Center, InputType);
  itkSetMacro( Center, InputType);

  /** Get and set the radius of the sphere */
  itkGetConstMacro( Radius, double);
  itkSetMacro( Radius, double);

protected:
  CylinderSpatialFunction();
  virtual ~CylinderSpatialFunction();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  CylinderSpatialFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The center of the sphere (of the same type as Input). */
  InputType m_Center;

  /** The radius of the sphere. */
  double m_Radius;

};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_CylinderSpatialFunction(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT CylinderSpatialFunction< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef CylinderSpatialFunction< ITK_TEMPLATE_2 x > \
                                           CylinderSpatialFunction##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkCylinderSpatialFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkCylinderSpatialFunction.txx"
#endif

#endif
