/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.txx,v $
  Language:  C++
  Date:      $Date: 2008-02-15 12:09:28 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkScalarImageToGrayLevelCooccurrenceMatrixGenerator_txx
#define _itkScalarImageToGrayLevelCooccurrenceMatrixGenerator_txx

#include "itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.h"

#include "itkConstNeighborhoodIterator.h"
#include "vnl/vnl_math.h"


namespace itk {
namespace Statistics {

/**
 * ********************* Constructor ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::ScalarImageToGrayLevelCooccurrenceMatrixGenerator() :
m_NumberOfBinsPerAxis(itkGetStaticConstMacro(DefaultBinsPerAxis)), m_Normalize(false)
{
  //initialize parameters
  // constant for a coocurrence matrix.
  const unsigned int measurementVectorSize = 2;
  this->m_LowerBound.SetSize( measurementVectorSize );
  this->m_UpperBound.SetSize( measurementVectorSize );
  this->m_LowerBound.Fill( NumericTraits<PixelType>::min() );
  this->m_UpperBound.Fill( NumericTraits<PixelType>::max() + 1 );
  this->m_Min = NumericTraits<PixelType>::min();
  this->m_Max = NumericTraits<PixelType>::max();

} // end Constructor()


/**
 * ********************* Compute ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::Compute( void )
{
  // First, create an appropriate histogram with the right number of bins
  // and mins and maxes correct for the image type.
  if ( this->m_Output.IsNull() )
  {
    this->m_Output = HistogramType::New();
    this->m_Output->SetMeasurementVectorSize( 2 );
  }

  typename HistogramType::SizeType size;
  size.SetSize( 2 );
  size.Fill( this->m_NumberOfBinsPerAxis );
  // Initialize also calls SetToZero, which is good.
  this->m_Output->Initialize( size, this->m_LowerBound, this->m_UpperBound );

  // Next, find the minimum radius that encloses all the offsets.
  unsigned int minRadius = 0;
  typename OffsetVector::ConstIterator offsets;
  for ( offsets = m_Offsets->Begin(); offsets != m_Offsets->End(); offsets++ )
  {
    for ( unsigned int i = 0; i < offsets.Value().GetOffsetDimension(); i++ )
    {
      unsigned int distance = vnl_math_abs( offsets.Value()[ i ] );
      if ( distance > minRadius )
      {
        minRadius = distance;
      }
    }
  }

  RadiusType radius;
  radius.Fill( minRadius );

  // Region
  RegionType region = this->m_Input->GetRequestedRegion();
  if ( this->m_ComputeRegion.GetNumberOfPixels() != 0 )
  {
    region = this->m_ComputeRegion;
  }

  // Now fill in the histogram
  this->FillHistogram( radius, region );

  // Normalize the histogram if requested
  if ( m_Normalize )
  {
    this->NormalizeHistogram();
  }

} // end Compute()


/**
 * ********************* FillHistogram ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::FillHistogram( RadiusType radius, RegionType region )
{
  // Iterate over all of those pixels and offsets, adding each
  // co-occurrence pair to the histogram
  typedef ConstNeighborhoodIterator<ImageType> NeighborhoodIteratorType;
  NeighborhoodIteratorType neighborIt;
  neighborIt = NeighborhoodIteratorType( radius, m_Input, region );

  OffsetType zeroOffset; zeroOffset.Fill( 0 );
  MeasurementVectorType cooccur;
  cooccur.SetSize(2);

  for ( neighborIt.GoToBegin(); !neighborIt.IsAtEnd(); ++neighborIt )
  {
    const PixelType centerPixelIntensity = neighborIt.GetCenterPixel();
    // Don't put a pixel in the histogram if the value is out-of-bounds.
    if ( centerPixelIntensity < this->m_Min || centerPixelIntensity > this->m_Max )
    {
      continue;
    }

    typename OffsetVector::ConstIterator offsets;
    for ( offsets = m_Offsets->Begin(); offsets != m_Offsets->End(); offsets++ )
    {
      bool pixelInBounds;
      const PixelType pixelIntensity
        = neighborIt.GetPixel( offsets.Value(), pixelInBounds );

      // Don't put a pixel in the histogram if it's out-of-bounds.
      if ( !pixelInBounds )
      {
        continue;
      }

      // don't put a pixel in the histogram if it's value is out-of-bounds.
      if ( pixelIntensity < this->m_Min || pixelIntensity > this->m_Max )
      {
        continue;
      }

      // Now make both possible co-occurrence combinations and increment the
      // histogram with them.
      cooccur[ 0 ] = centerPixelIntensity;
      cooccur[ 1 ] = pixelIntensity;
      m_Output->IncreaseFrequency( cooccur, 1 );
      cooccur[ 1 ] = centerPixelIntensity;
      cooccur[ 0 ] = pixelIntensity;
      m_Output->IncreaseFrequency( cooccur, 1 );
    }
  }
} // end FillHistogram()


/**
 * ********************* NormalizeHistogram ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::NormalizeHistogram( void )
{
  typename HistogramType::Iterator hit( this->m_Output );
  typename HistogramType::TotalAbsoluteFrequencyType totalFrequency =
    m_Output->GetTotalFrequency();

  /** \todo: this won't work with the new statistics framework, since
  * frequency are always integer then... */
  for ( hit = m_Output->Begin(); hit != m_Output->End(); ++hit )
  {
    hit.SetFrequency( hit.GetFrequency() / totalFrequency );
  }

} // end NormalizeHistogram()


/**
 * ********************* SetPixelValueMinMax ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::SetPixelValueMinMax( PixelType min, PixelType max )
{
  this->m_Min = min;
  this->m_Max = max;
  this->m_LowerBound.Fill( min );
  this->m_UpperBound.Fill( max + 1 );
  this->Modified();

} // end SetPixelValueMinMax()


/**
 * ********************* PrintSelf ****************************
 */

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
  TImageType, THistogramFrequencyContainer >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  /** Call the superclass implementation. */
  Superclass::PrintSelf( os, indent );

  /** Print the member variables. */
  os << indent << "Input: "
    << this->m_Input.GetPointer() << std::endl;
  os << indent << "Output: "
    << this->m_Output.GetPointer() << std::endl;
  os << indent << "Min: "
    << this->m_Min << std::endl;
  os << indent << "Max: "
    << this->m_Max << std::endl;
  os << indent << "NumberOfBinsPerAxis: "
    << this->m_NumberOfBinsPerAxis << std::endl;
  os << indent << "LowerBound: "
    << this->m_LowerBound << std::endl;
  os << indent << "UpperBound: "
    << this->m_UpperBound << std::endl;
  os << indent << "Normalize: "
    << this->m_Normalize << std::endl;

  os << indent << "Offsets: ";
  for ( unsigned int i = 0; i < this->m_Offsets->Size(); ++i )
  {
    os << this->m_Offsets->GetElement( i ) << " ";
  }
  os << std::endl;

} // end PrintSelf()


} // end of namespace Statistics
} // end of namespace itk


#endif
