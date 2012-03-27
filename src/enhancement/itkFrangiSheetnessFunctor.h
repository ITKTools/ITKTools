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
#ifndef __itkFrangiSheetnessFunctor_h
#define __itkFrangiSheetnessFunctor_h

#include "itkUnaryFunctorBase.h"
#include "vnl/vnl_math.h"

namespace itk
{
namespace Functor
{

/** \class FrangiSheetnessFunctor
 * \brief Computes a measure of sheetness from the Hessian eigenvalues.
 *
 * The sheetness filter obtained directly from the original paper
 * of Alejandro F. Frangi by modifying the RA term.
 *
 * \authors Alejandro F. Frangi, Wiro J. Niessen, Koen L. Vincken, Max A. Viergever
 *
 * \par Reference
 * Multiscale Vessel Enhancement Filtering.
 * Medical Image Computing and Computer-Assisted Interventation MICCAI’98
 * Lecture Notes in Computer Science, 1998, Volume 1496/1998, 130-137,
 * DOI: 10.1007/BFb0056195
 *
 * \sa FrangiVesselnessImageFilter
 * \ingroup IntensityImageFilters Multithreaded
 */

template< class TInput, class TOutput >
class FrangiSheetnessFunctor
  : public UnaryFunctorBase< TInput, TOutput >
{
public:
  /** Standard class typedefs. */
  typedef FrangiSheetnessFunctor    Self;
  typedef UnaryFunctorBase           Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** New macro for creation of through a smart pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( FrangiSheetnessFunctor, UnaryFunctorBase );

  /** Typedef's. */
  typedef typename NumericTraits<TOutput>::RealType RealType;

  /** This does the real computation */
  virtual TOutput Evaluate( const TInput & eigenValues ) const
  {
    RealType sheetness = NumericTraits<RealType>::Zero;

    RealType a1 = static_cast<RealType>( eigenValues[0] );
    RealType a2 = static_cast<RealType>( eigenValues[1] );
    RealType a3 = static_cast<RealType>( eigenValues[2] );

    RealType l1 = vnl_math_abs( a1 );
    RealType l2 = vnl_math_abs( a2 );
    RealType l3 = vnl_math_abs( a3 );

    const RealType eigenValuesSum = a1 + a2 + a3;

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
      if( eigenValuesSum > NumericTraits<RealType>::Zero )
      {
        return static_cast<TOutput>( sheetness );
      }
    }
    else
    {
      if( eigenValuesSum < NumericTraits<RealType>::Zero )
      {
        return static_cast<TOutput>( sheetness );
      }
    }

    // Avoid divisions by zero (or close to zero)
    if( l2 < vnl_math::eps || l3 < vnl_math::eps )
    {
      return static_cast<TOutput>( sheetness );
    }

    const RealType Ra = l2 / l3;
    const RealType Rb = l1 / vcl_sqrt( l2 * l3 );
    const RealType S  = vcl_sqrt( l1 * l1 + l2 * l2 + l3 * l3 );

    sheetness  =         vcl_exp( - ( Ra * Ra ) / ( 2.0 * m_Alpha * m_Alpha ) );
    sheetness *=         vcl_exp( - ( Rb * Rb ) / ( 2.0 * m_Beta * m_Beta ) );
    sheetness *= ( 1.0 - vcl_exp( - ( S  * S  ) / ( 2.0 * m_C * m_C ) ) );

    return static_cast<TOutput>( sheetness );
  } // end operator ()

  /** Set parameters */
  itkSetClampMacro( Alpha, double, 0.0, NumericTraits<double>::max() );
  itkSetClampMacro( Beta, double, 0.0, NumericTraits<double>::max() );
  itkSetClampMacro( C, double, 0.0, NumericTraits<double>::max() );
  itkSetMacro( BrightObject, bool );

protected:
  /** Constructor */
  FrangiSheetnessFunctor()
  {
    this->m_Alpha = 0.5; // suggested value in the paper
    this->m_Beta = 0.5;  // suggested value in the paper
    this->m_C = 500.0;   // Depends on intensity range, 500 good for lung CT
    this->m_BrightObject = true;
  };
  virtual ~FrangiSheetnessFunctor(){};

private:
  FrangiSheetnessFunctor(const Self &);  //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** Member variables. */
  double  m_Alpha;
  double  m_Beta;
  double  m_C;
  bool    m_BrightObject;

}; // end class FrangiSheetnessFunctor

} // end namespace itk::Functor
} // end namespace itk

#endif
