/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDeformationFieldBendingEnergyFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-14 19:20:32 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDeformationFieldBendingEnergyFilter_txx
#define __itkDeformationFieldBendingEnergyFilter_txx

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
  for (unsigned int i = 0; i < ImageDimension; ++i)
  {
    const RealVectorType p = it.GetNext(i);
    const RealVectorType q = it.GetPrevious(i);
    const RealVectorType pqc = p + q - c2;
    bending += pqc.GetSquaredNorm() *
      vcl_pow( this->m_HalfDerivativeWeights[i], static_cast<int>(4) );
  }
  /** off-diagonal: */
  for (unsigned int i = 0; i < ImageDimension; ++i)
  {
    for (unsigned int j = i+1; j < ImageDimension; ++j)
    {
      const RealVectorType p = it.GetPixel( it.GetCenterNeighborhoodIndex() + it.GetStride(i) + it.GetStride(j) );
      const RealVectorType q = it.GetPixel( it.GetCenterNeighborhoodIndex() - it.GetStride(i) - it.GetStride(j) );
      const RealVectorType r = it.GetPixel( it.GetCenterNeighborhoodIndex() + it.GetStride(i) - it.GetStride(j) );
      const RealVectorType s = it.GetPixel( it.GetCenterNeighborhoodIndex() - it.GetStride(i) + it.GetStride(j) );
      const RealVectorType pqrs = p + q - r - s;
      bending += 2.0 * pqrs.GetSquaredNorm() * vnl_math_sqr(
          this->m_HalfDerivativeWeights[i] * this->m_HalfDerivativeWeights[j] );
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

#endif
