/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOtsuThresholdWithMaskImageCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2009-01-26 21:45:54 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOtsuThresholdWithMaskImageCalculator_txx
#define __itkOtsuThresholdWithMaskImageCalculator_txx

#include "itkOtsuThresholdWithMaskImageCalculator.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template<class TInputImage>
OtsuThresholdWithMaskImageCalculator<TInputImage>
::OtsuThresholdWithMaskImageCalculator()
{
  this->m_Image = nullptr;
  this->m_MaskImage = nullptr;
  this->m_Threshold = NumericTraits<PixelType>::Zero;
  this->m_NumberOfHistogramBins = 128;
  this->m_RegionSetByUser = false;
}


/*
 * Compute the Otsu's threshold
 */
template<class TInputImage>
void
OtsuThresholdWithMaskImageCalculator<TInputImage>
::Compute( void )
{
  unsigned int j;

  if( !m_Image ) { return; }
  if( !m_RegionSetByUser )
  {
    this->m_Region = this->m_Image->GetRequestedRegion();
  }

  double totalPixels = (double) this->m_Region.GetNumberOfPixels();
  if( totalPixels == 0 ) { return; }
  totalPixels = 0.0;

  typedef ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  IteratorType iter( this->m_Image, this->m_Region );
  iter.GoToBegin();

  typedef ImageRegionConstIterator<MaskImageType> MaskIteratorType;
  MaskIteratorType itMask;
  if( this->m_MaskImage )
  {
    itMask = MaskIteratorType( this->m_MaskImage, this->m_Region );
    itMask.GoToBegin();
  }

  // compute image max and min
  PixelType imageMin = NumericTraits<PixelType>::max();
  PixelType imageMax = NumericTraits<PixelType>::NonpositiveMin();
  while ( !iter.IsAtEnd() )
  {
    if( this->m_MaskImage && itMask.Value() == 0 )
    {
      ++iter; ++itMask;
      continue;
    }

    PixelType current = iter.Value();
    imageMin = imageMin > current ? current : imageMin;
    imageMax = imageMax < current ? current : imageMax;

    ++iter;
    if( this->m_MaskImage ) ++itMask;
  }

  if( imageMin >= imageMax )
  {
    this->m_Threshold = imageMin;
    return;
  }

  // create a histogram
  std::vector<double> relativeFrequency;
  relativeFrequency.resize( this->m_NumberOfHistogramBins );
  for ( j = 0; j < this->m_NumberOfHistogramBins; j++ )
    {
    relativeFrequency[j] = 0.0;
    }

  double binMultiplier = (double) this->m_NumberOfHistogramBins /
    (double) ( imageMax - imageMin );

  iter.GoToBegin();
  if( this->m_MaskImage ) itMask.GoToBegin();
  while ( !iter.IsAtEnd() )
  {
    if( this->m_MaskImage && itMask.Value() == 0 )
    {
      ++iter; ++itMask;
      continue;
    }

    unsigned int binNumber;
    PixelType value = iter.Get();

    if( value == imageMin )
      {
      binNumber = 0;
      }
    else
      {
      binNumber = (unsigned int) vcl_ceil((value - imageMin) * binMultiplier ) - 1;
      if( binNumber == this->m_NumberOfHistogramBins ) // in case of rounding errors
        {
        binNumber -= 1;
        }
      }

    relativeFrequency[binNumber] += 1.0;
    totalPixels += 1.0;

    ++iter;
    if( this->m_MaskImage ) ++itMask;
  }

  // normalize the frequencies
  double totalMean = 0.0;
  for ( j = 0; j < this->m_NumberOfHistogramBins; j++ )
    {
    relativeFrequency[j] /= totalPixels;
    totalMean += (j+1) * relativeFrequency[j];
    }


  // compute Otsu's threshold by maximizing the between-class
  // variance
  double freqLeft = relativeFrequency[0];
  double meanLeft = 1.0;
  double meanRight = ( totalMean - freqLeft ) / ( 1.0 - freqLeft );

  double maxVarBetween = freqLeft * ( 1.0 - freqLeft ) *
    vnl_math_sqr( meanLeft - meanRight );
  int maxBinNumber = 0;

  double freqLeftOld = freqLeft;
  double meanLeftOld = meanLeft;

  for ( j = 1; j < this->m_NumberOfHistogramBins; j++ )
    {
    freqLeft += relativeFrequency[j];
    meanLeft = ( meanLeftOld * freqLeftOld +
                 (j+1) * relativeFrequency[j] ) / freqLeft;
    if(freqLeft == 1.0)
      {
      meanRight = 0.0;
      }
    else
      {
      meanRight = ( totalMean - meanLeft * freqLeft ) /
        ( 1.0 - freqLeft );
      }
    double varBetween = freqLeft * ( 1.0 - freqLeft ) *
      vnl_math_sqr( meanLeft - meanRight );

    if( varBetween > maxVarBetween )
      {
      maxVarBetween = varBetween;
      maxBinNumber = j;
      }

    // cache old values
    freqLeftOld = freqLeft;
    meanLeftOld = meanLeft;

    }

  this->m_Threshold = static_cast<PixelType>( imageMin +
                                        ( maxBinNumber + 1 ) / binMultiplier );
}

template<class TInputImage>
void
OtsuThresholdWithMaskImageCalculator<TInputImage>
::SetRegion( const RegionType & region )
{
  this->m_Region = region;
  this->m_RegionSetByUser = true;
}


template<class TInputImage>
void
OtsuThresholdWithMaskImageCalculator<TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Threshold: " << this->m_Threshold << std::endl;
  os << indent << "NumberOfHistogramBins: " << this->m_NumberOfHistogramBins << std::endl;
  os << indent << "Image: " << this->m_Image.GetPointer() << std::endl;
}

} // end namespace itk

#endif
