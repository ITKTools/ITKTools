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
#ifndef __itkModifiedKrissianVesselnessFunctor_h
#define __itkModifiedKrissianVesselnessFunctor_h

#include "itkUnaryFunctorBase.h"
#include "vnl/vnl_math.h"

namespace itk
{
namespace Functor
{

/** \class ModifiedKrissianVesselnessFunctor
 * \brief Computes a measure of vesselness from the Hessian eigenvalues.
 *
 * Inspired by the paper:
 * \authors Krissian, K. and Malandain, G. and Ayache, N. and Vaillant, R. and Trousset, Y.
 *
 * \par Reference
 * Model Based Detection of Tubular Structures in 3D Images
 * Computer Vision and Image Understanding, vol. 80, no. 2, pp. 130 - 171, Nov. 2000.
 *
 * \sa FrangiVesselnessImageFilter
 * \ingroup IntensityImageFilters Multithreaded
 */

template< class TInput, class TOutput >
class ModifiedKrissianVesselnessFunctor
  : public UnaryFunctorBase< TInput, TOutput >
{
public:
  /** Standard class typedefs. */
  typedef ModifiedKrissianVesselnessFunctor    Self;
  typedef UnaryFunctorBase           Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** New macro for creation of through a smart pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ModifiedKrissianVesselnessFunctor, UnaryFunctorBase );

  /** Typedef's. */
  typedef typename NumericTraits<TOutput>::RealType RealType;

  /** This does the real computation */
  virtual TOutput Evaluate( const TInput & eigenValues ) const
  {
    RealType vesselness = NumericTraits<RealType>::Zero;

    RealType a1 = static_cast<RealType>( eigenValues[0] );
    RealType a2 = static_cast<RealType>( eigenValues[1] );
    RealType a3 = static_cast<RealType>( eigenValues[2] );

    RealType l1 = vnl_math_abs( a1 );
    RealType l2 = vnl_math_abs( a2 );
    RealType l3 = vnl_math_abs( a3 );

    // Sort the eigenvalues by their absolute value.
    // At the end of the sorting we should have
    // |l1| <= |l2| <= |l3|
    if( l2 > l3 )
    {
      std::swap(l2, l3);
      std::swap(a2, a3);
    }
    if( l1 > l2 )
    {
      std::swap(l1, l2);
      std::swap(a1, a2);
    }
    if( l2 > l3 )
    {
      std::swap(l2, l3);
      std::swap(a2, a3);
    }

    if( this->m_BrightObject )
    {
      if( a3 > NumericTraits<RealType>::Zero )
      {
        return static_cast<TOutput>( vesselness );
      }
    }
    else
    {
      if( a3 < NumericTraits<RealType>::Zero )
      {
        return static_cast<TOutput>( vesselness );
      }
    }

    // Avoid divisions by zero (or close to zero)
    if( l3 < vnl_math::eps )
    {
      return static_cast<TOutput>( vesselness );
    }

    vesselness = ( l2 / l3 ) * ( l2 + l3 );

    return static_cast<TOutput>( vesselness );
  } // end operator ()

  /** Set parameters */
  itkSetMacro( BrightObject, bool );

protected:
  /** Constructor */
  ModifiedKrissianVesselnessFunctor()
  {
    this->m_BrightObject = true;
  };
  virtual ~ModifiedKrissianVesselnessFunctor(){};

private:
  ModifiedKrissianVesselnessFunctor(const Self &);  // purposely not implemented
  void operator=(const Self &);                     // purposely not implemented

  /** Member variables. */
  bool    m_BrightObject;

}; // end class ModifiedKrissianVesselnessFunctor

} // end namespace itk::Functor
} // end namespace itk

#endif
