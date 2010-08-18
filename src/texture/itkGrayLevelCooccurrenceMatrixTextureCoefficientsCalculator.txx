/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2008-03-14 09:44:02 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator_txx
#define _itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator_txx

#include "itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.h"

#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

namespace itk {
namespace Statistics {


/**
 * ********************* Constructor ****************************
 */

template< class THistogram >
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator()
{
  this->m_Histogram = 0;
  this->ResetFeatureValues();

} // end Constructor()

/**
 * ********************* ResetFeatureValues ****************************
 */

template< class THistogram >
void
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::ResetFeatureValues( void )
{
  this->m_Energy = 0.0;
  this->m_Entropy = 0.0;
  this->m_Correlation = 0.0;
  this->m_InverseDifferenceMoment = 0.0;
  this->m_Inertia = 0.0;
  this->m_ClusterShade = 0.0;
  this->m_ClusterProminence = 0.0;
  this->m_HaralickCorrelation = 0.0;

} // end ResetFeatureValues()


/**
 * ********************* Compute ****************************
 */

template< class THistogram >
void
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::Compute( void )
{
  /** Reset the feature values. */
  this->ResetFeatureValues();

  /** Get the total histogram frequency and size. */
  double totalFrequency = this->m_Histogram->GetTotalFrequency();
  typename HistogramType::SizeValueType binsPerAxis = this->m_Histogram->GetSize( 0 );

  /** Temporary variables. */
  double pixelSum_0, pixelSum_1, pixelSum_00, pixelSum_01, pixelSum_11,
    pixelSum_000, pixelSum_001, pixelSum_011, pixelSum_111,
    pixelSum_0000, pixelSum_0001, pixelSum_0011, pixelSum_0111, pixelSum_1111;
  pixelSum_0 = pixelSum_1
    = pixelSum_00 = pixelSum_01 = pixelSum_11
    = pixelSum_000 = pixelSum_001 = pixelSum_011 = pixelSum_111
    = pixelSum_0000 = pixelSum_0001 = pixelSum_0011
    = pixelSum_0111 = pixelSum_1111 = 0.0;
  std::vector<double> marginalSums( binsPerAxis, 0.0 );

  /** Walk over the histogram. Only once instead of 4! */
  double log2 = vcl_log(2.);
  HistogramConstIterator hit( this->m_Histogram );
  for ( hit = this->m_Histogram->Begin(); hit != this->m_Histogram->End(); ++hit )
  {
    /** Get the frequency of this histogram entry. */
    AbsoluteFrequencyType frequencyCount = hit.GetFrequency();

    /** No use doing these calculations if we're just multiplying by zero. */
    if ( frequencyCount == 0 ) continue;

    /** Normalize frequency and get the index of this histogram entry. */
    double frequency = static_cast<double>(frequencyCount) / totalFrequency;
    IndexType index = this->m_Histogram->GetIndex( hit.GetInstanceIdentifier() );

    /** Compute values that are needed later for the feature computation. */
    pixelSum_0     += index[ 0 ] * frequency;
    pixelSum_1     += index[ 1 ] * frequency;
    pixelSum_00    += index[ 0 ] * index[ 0 ] * frequency;
    pixelSum_01    += index[ 0 ] * index[ 1 ] * frequency;
    pixelSum_11    += index[ 1 ] * index[ 1 ] * frequency;
    pixelSum_000   += index[ 0 ] * index[ 0 ] * index[ 0 ] * frequency;
    pixelSum_001   += index[ 0 ] * index[ 0 ] * index[ 1 ] * frequency;
    pixelSum_011   += index[ 0 ] * index[ 1 ] * index[ 1 ] * frequency;
    pixelSum_111   += index[ 1 ] * index[ 1 ] * index[ 1 ] * frequency;
    pixelSum_0000  += index[ 0 ] * index[ 0 ] * index[ 0 ] * index[ 0 ] * frequency;
    pixelSum_0001  += index[ 0 ] * index[ 0 ] * index[ 0 ] * index[ 1 ] * frequency;
    pixelSum_0011  += index[ 0 ] * index[ 0 ] * index[ 1 ] * index[ 1 ] * frequency;
    pixelSum_0111  += index[ 0 ] * index[ 1 ] * index[ 1 ] * index[ 1 ] * frequency;
    pixelSum_1111  += index[ 1 ] * index[ 1 ] * index[ 1 ] * index[ 1 ] * frequency;

    /** For the HaralickCorrelation. */
    marginalSums[ index[ 0 ] ] += frequency;

    /** Compute the features that can be computed in this loop immediately. */
    this->m_Energy += frequency * frequency;
    this->m_Entropy -= ( frequency > 0.0001 ) ? frequency * vcl_log( frequency ) / log2 : 0.0;
    this->m_InverseDifferenceMoment += frequency /
      ( 1.0 + ( index[ 0 ] - index[ 1 ] ) * ( index[ 0 ] - index[ 1 ] ) );
    this->m_Inertia += ( index[ 0 ] - index[ 1 ] ) * ( index[ 0 ] - index[ 1 ] ) * frequency;
    this->m_HaralickCorrelation += index[ 0 ] * index[ 1 ] * frequency;
  }

  /** Compute intermediate values. */
  double pixelMean = pixelSum_0;
  double pixelMean2 = pixelMean * pixelMean;
  double pixelMean3 = pixelMean2 * pixelMean;
  double pixelMean4 = pixelMean3 * pixelMean;
  double pixelVariance = pixelSum_00 - pixelMean * pixelMean; //( N -1 ) ????

  /** Compute marginal mean and variance needed for the HaralickCorrelation. */
  double marginalMean = 0.0;
  double marginalSquareMean = 0.0;
  for ( unsigned int i = 0; i < binsPerAxis; ++i )
  {
    marginalMean += marginalSums[ i ];
    marginalSquareMean += marginalSums[ i ] * marginalSums[ i ];
  }
  double marginalVariance = marginalSquareMean - marginalMean * marginalMean / binsPerAxis;
  marginalVariance /= binsPerAxis; // why not (binsPerAxis-1)?
  marginalMean /= binsPerAxis;

  /** Compute the remaining features. */
  this->m_Correlation = ( pixelSum_01 - pixelMean * pixelMean )
    / ( pixelVariance * pixelVariance );

  this->m_ClusterShade =
    + 16 * pixelMean3
    -  6 * pixelMean * pixelSum_00
    +      pixelSum_000
    - 12 * pixelMean * pixelSum_01
    +  3 * pixelSum_001
    -  6 * pixelMean * pixelSum_11
    +  3 * pixelSum_011
    + pixelSum_111;

  this->m_ClusterProminence =
    - 48 * pixelMean4
    + 24 * pixelMean2 * pixelSum_00
    -  8 * pixelMean  * pixelSum_000
    +      pixelSum_0000
    + 48 * pixelMean2 * pixelSum_01
    - 24 * pixelMean  * pixelSum_001
    +  4 * pixelSum_0001
    + 24 * pixelMean2 * pixelSum_11
    - 24 * pixelMean  * pixelSum_011
    +  6 * pixelSum_0011
    -  8 * pixelMean  * pixelSum_111
    +  4 * pixelSum_0111
    +      pixelSum_1111;

  this->m_HaralickCorrelation -= marginalMean * marginalMean;
  this->m_HaralickCorrelation /= marginalVariance;

} // end Compute()


/**
 * ********************* GetFeature ****************************
 */

template< class THistogram >
double
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::GetFeature( TextureFeatureName feature )
{
  switch( feature )
  {
  case Energy:
    return this->GetEnergy();
  case Entropy:
    return this->GetEntropy();
  case Correlation:
    return this->GetCorrelation();
  case InverseDifferenceMoment:
    return this->GetInverseDifferenceMoment();
  case Inertia:
    return this->GetInertia();
  case ClusterShade:
    return this->GetClusterShade();
  case ClusterProminence:
    return this->GetClusterProminence();
  case HaralickCorrelation:
    return this->GetHaralickCorrelation();
  default:
    return 0;
  }

} // end GetFeature()


/**
 * ********************* GetFeature ****************************
 */

template< class THistogram >
double
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::GetFeature( unsigned int feature )
{
  if ( feature == 0 ) return this->GetEnergy();
  else if ( feature == 1 ) return this->GetEntropy();
  else if ( feature == 2 ) return this->GetCorrelation();
  else if ( feature == 3 ) return this->GetInverseDifferenceMoment();
  else if ( feature == 4 ) return this->GetInertia();
  else if ( feature == 5 ) return this->GetClusterShade();
  else if ( feature == 6 ) return this->GetClusterProminence();
  else if ( feature == 7 ) return this->GetHaralickCorrelation();
  else return 0.0;

} // end GetFeature()


/**
 * ********************* PrintSelf ****************************
 */

template< class THistogram >
void
GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator< THistogram >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  /** Call the superclass implementation. */
  Superclass::PrintSelf( os, indent );

  /** Print the member variables. */

} // end PrintSelf()


} // end of namespace Statistics
} // end of namespace itk


#endif
