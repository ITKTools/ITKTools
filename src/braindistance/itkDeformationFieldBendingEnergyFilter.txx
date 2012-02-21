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
#ifndef __itkDeformationFieldBendingEnergyFilter_txx_
#define __itkDeformationFieldBendingEnergyFilter_txx_

#include "itkDeformationFieldBendingEnergyFilter.h"

#include "vnl/vnl_math.h"

namespace itk
{

template <typename TInputImage, typename TRealType, typename TOutputImage>
DeformationFieldBendingEnergyFilter<TInputImage, TRealType, TOutputImage>
::DeformationFieldBendingEnergyFilter()
{
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
TRealType
DeformationFieldBendingEnergyFilter< TInputImage, TRealType, TOutputImage >
::EvaluateAtNeighborhood(const ConstNeighborhoodIteratorType &it) const
{
  double bending = itk::NumericTraits<RealType>::Zero;
  /** diagonal terms: */
  const RealVectorType c2 = it.GetCenterPixel() *2.0;
  for( unsigned int i = 0; i < ImageDimension; ++i )
  {
    const RealVectorType p = it.GetNext(i);
    const RealVectorType q = it.GetPrevious(i);
    const RealVectorType pqc = p + q - c2;
    bending += pqc.GetSquaredNorm() *
      vcl_pow( this->m_HalfDerivativeWeights[ i ], static_cast<int>(4) );
  }
  /** off-diagonal: */
  for( unsigned int i = 0; i < ImageDimension; ++i )
  {
    for( unsigned int j = i+1; j < ImageDimension; ++j )
    {
      const RealVectorType p = it.GetPixel( it.GetCenterNeighborhoodIndex() + it.GetStride(i) + it.GetStride(j) );
      const RealVectorType q = it.GetPixel( it.GetCenterNeighborhoodIndex() - it.GetStride(i) - it.GetStride(j) );
      const RealVectorType r = it.GetPixel( it.GetCenterNeighborhoodIndex() + it.GetStride(i) - it.GetStride(j) );
      const RealVectorType s = it.GetPixel( it.GetCenterNeighborhoodIndex() - it.GetStride(i) + it.GetStride(j) );
      const RealVectorType pqrs = p + q - r - s;
      bending += 2.0 * pqrs.GetSquaredNorm() * vnl_math_sqr(
          this->m_HalfDerivativeWeights[ i ] * this->m_HalfDerivativeWeights[j] );
    }
  }

  return bending;
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
DeformationFieldBendingEnergyFilter< TInputImage, TRealType, TOutputImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif // end #ifndef __itkDeformationFieldBendingEnergyFilter_txx_
