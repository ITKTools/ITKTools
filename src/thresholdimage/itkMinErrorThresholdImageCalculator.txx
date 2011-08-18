/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMinErrorThresholdImageCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2010-05-04 13:18:45 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMinErrorThresholdImageCalculator_txx
#define _itkMinErrorThresholdImageCalculator_txx

#include "itkMinErrorThresholdImageCalculator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkMinimumMaximumImageCalculator.h"

#include "vnl/vnl_math.h"
#include <limits>

namespace itk
{

/*
 * Constructor
 */
template<class TInputImage>
MinErrorThresholdImageCalculator<TInputImage>
::MinErrorThresholdImageCalculator()
{
  this->m_Image = NULL;
  this->m_Threshold = NumericTraits<PixelType>::Zero;
  this->m_NumberOfHistogramBins = 128;
  this->m_RegionSetByUser = false;
  this->m_AlphaLeft = 0.0;
  this->m_AlphaRight= 0.0;
  this->m_PriorLeft = 0.0;
  this->m_PriorRight= 0.0;
  this->m_StdLeft = 0.0;
  this->m_StdRight = 0.0;
  this->m_UseGaussian = 0;
  this->m_usePoisson = 1;
}


/*
 * Set the mixture type
 */
template<class TInputImage>
void
MinErrorThresholdImageCalculator<TInputImage>
::UseGaussianMixture(bool TYPE)
{
  if(TYPE)
    {
    this->m_UseGaussian = 1;
    this->m_usePoisson = 0;
    }
  else
    {
    this->m_UseGaussian = 0;
    this->m_usePoisson = 1;
    }
}

/*
 * Compute the MinError's threshold
 */
template<class TInputImage>
void
MinErrorThresholdImageCalculator<TInputImage>
::Compute( void )
{

  unsigned int j, i;

  if ( !m_Image ) { return; }
  if( !m_RegionSetByUser )
    {
    this->m_Region = this->m_Image->GetRequestedRegion();
    }

  double totalPixels = (double) this->m_Region.GetNumberOfPixels();
  if ( totalPixels == 0 ) { return; }


  // compute image max and min
  typedef MinimumMaximumImageCalculator<TInputImage> RangeCalculator;
  typename RangeCalculator::Pointer rangeCalculator = RangeCalculator::New();
  rangeCalculator->SetImage( this->m_Image );
  rangeCalculator->Compute();

  PixelType imageMin = rangeCalculator->GetMinimum();
  PixelType imageMax = rangeCalculator->GetMaximum();

  if ( imageMin >= imageMax )
    {
    this->m_Threshold = imageMin;
    return;
    }

  // create the histogram and the error functions
  std::vector<double> relativeFrequency;
  std::vector<double> errorFunctionPois;
  std::vector<double> errorFunctionGaus;

  relativeFrequency.resize( this->m_NumberOfHistogramBins );
  errorFunctionPois.resize( this->m_NumberOfHistogramBins );
  errorFunctionGaus.resize( this->m_NumberOfHistogramBins );
  for ( j = 0; j < this->m_NumberOfHistogramBins; j++ )
    {
    relativeFrequency[j] = errorFunctionPois[j] = errorFunctionGaus[j] = 0.0;
    }

  double binMultiplier = (double) this->m_NumberOfHistogramBins /
    (double) ( imageMax - imageMin );

  typedef ImageRegionConstIteratorWithIndex<TInputImage> Iterator;
  Iterator iter( this->m_Image, this->m_Region );

  while ( !iter.IsAtEnd() )
    {
    unsigned int binNumber;
    PixelType value = iter.Get();

    if ( value == imageMin )
      {
      binNumber = 0;
      }
    else
      {
      binNumber = (unsigned int) vcl_ceil((value - imageMin) * binMultiplier ) - 1;
      if ( binNumber == this->m_NumberOfHistogramBins ) // in case of rounding errors
        {
        binNumber -= 1;
        }
      }

    relativeFrequency[binNumber] += 1.0;
    ++iter;

    }

  // normalize the histogram
  double totalMean = 0.0;
  for ( j = 0; j < this->m_NumberOfHistogramBins; j++ )
    {
    relativeFrequency[j] /= totalPixels;
    totalMean += (j+1) * relativeFrequency[j];
    }


  // compute MinError threshold that minimizes the error criterion function
  //Initialize mixture parameters to zeros
  double priorRight = 0.0;
  double priorLeft = 0.0;
  double meanLeft = 0.0;
  double meanRight = 0.0;
  double varLeft = 0.0;
  double varRight = 0.0;
  double stdLeft = 0.0;
  double stdRight = 0.0;

  for ( j = 1; j < this->m_NumberOfHistogramBins-1; j++ )
    {
    //compute the current parameters for left (background) mixture component
    priorLeft = 0.0; //Prior Probability
    meanLeft = 0.0; //mean
    varLeft = 0.0; //variance
    for ( i=0; i<=j; i++)
      {
      priorLeft+=relativeFrequency[i];
      meanLeft+=(i*relativeFrequency[i]);
      }
    meanLeft/=priorLeft;
    for ( i=0; i<=j; i++)
      {
      varLeft+=(vnl_math_sqr(i-meanLeft)*relativeFrequency[i]);
      }
    varLeft/=priorLeft;
    stdLeft = vcl_sqrt(varLeft); //standard deviation

    //compute the current parameters for right (foreground) mixture component
    priorRight = 0.0; //Prior Probability
    meanRight = 0.0; //mean
    varRight = 0.0; //variance
    for ( i=j+1; i<m_NumberOfHistogramBins; i++)
      {
      priorRight+=relativeFrequency[i];
      meanRight+=(i*relativeFrequency[i]);
      }
    meanRight/=priorRight;
    for ( i=j+1; i<m_NumberOfHistogramBins; i++)
      {
      varRight+=(vnl_math_sqr(i-meanRight)*relativeFrequency[i]);
      }
    varRight/=priorRight;
    stdRight = vcl_sqrt(varRight); //standard deviation
    //Make sure you don't end up with zero values for the parameters
    priorLeft += std::numeric_limits<long double>::epsilon();
    priorRight += std::numeric_limits<long double>::epsilon();
    stdLeft += std::numeric_limits<long double>::epsilon();
    stdRight += std::numeric_limits<long double>::epsilon();

    //compute the values of the error functions at the current threshold (j)
    errorFunctionPois[j] = totalMean-priorLeft*(vcl_log(priorLeft)+meanLeft*vcl_log(meanLeft))-priorRight*(vcl_log(priorRight)+meanRight*vcl_log(meanRight));
    errorFunctionGaus[j] = 1+2*(priorLeft*vcl_log(stdLeft)+priorRight*vcl_log(stdRight))-2*(priorLeft*vcl_log(priorLeft)+priorRight*vcl_log(priorRight));
    }

  //find the threshold value that minimizes each of the error functions
  i = 1;
  if( this->m_UseGaussian)
    {
    for ( j = 2; j < this->m_NumberOfHistogramBins-1; j++ )
      {
      if(errorFunctionGaus[j]<errorFunctionGaus[i])
         i = j;
      }
    }
  else
    {
    for ( j = 2; j < this->m_NumberOfHistogramBins-1; j++ )
      {
      if(errorFunctionPois[j]<errorFunctionPois[i])
         i = j;
      }
    }

   //Finally, compute the threshold
    this->m_Threshold = static_cast<PixelType>( imageMin +
                                        (i+1) / binMultiplier );

  //estimate the parameters of the resulting mixture
  for (j=0; j<=i; j++)
    {
    this->m_PriorLeft+=relativeFrequency[j];
    this->m_AlphaLeft+=(j*relativeFrequency[j]);
    }
  this->m_AlphaLeft/= this->m_PriorLeft;

  for ( j=i+1; j<m_NumberOfHistogramBins; j++)
    {
    this->m_PriorRight+=relativeFrequency[j];
    this->m_AlphaRight+=(j*relativeFrequency[j]);
    }
  this->m_AlphaRight/= this->m_PriorRight;


  if( this->m_UseGaussian)
    {
    varLeft = 0.0;
    for (j=0; j<=i; j++)
      {
      varLeft+=(vnl_math_sqr(j-m_AlphaLeft)*relativeFrequency[j]);
      }
    varLeft/=m_PriorLeft;
    this->m_StdLeft=vcl_sqrt(varLeft);

    varRight = 0.0;
    for ( j=i+1; j<m_NumberOfHistogramBins; j++)
      {
      varRight+=(vnl_math_sqr(j-m_AlphaRight)*relativeFrequency[j]);
      }
    varRight/=m_PriorRight;
    this->m_StdRight=vcl_sqrt(varRight);
    }
  this->m_AlphaLeft= imageMin + ( this->m_AlphaLeft+1) / binMultiplier ;
  this->m_AlphaRight= imageMin + ( this->m_AlphaRight+1) / binMultiplier ;
  this->m_StdLeft/= binMultiplier ;
  this->m_StdRight/= binMultiplier ;
}

template<class TInputImage>
void
MinErrorThresholdImageCalculator<TInputImage>
::SetRegion( const RegionType & region )
{
  this->m_Region = region;
  this->m_RegionSetByUser = true;
}


template<class TInputImage>
void
MinErrorThresholdImageCalculator<TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Threshold: " << this->m_Threshold << std::endl;
  os << indent << "Image: " << this->m_Image.GetPointer() << std::endl;
}

} // end namespace itk

#endif
