/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSmoothingRecursiveGaussianImageFilter2.txx,v $
  Language:  C++
  Date:      $Date: 2007-04-25 13:52:19 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSmoothingRecursiveGaussianImageFilter2_txx
#define _itkSmoothingRecursiveGaussianImageFilter2_txx

#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressAccumulator.h"

namespace itk
{


/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SmoothingRecursiveGaussianImageFilter2()
{

  m_NormalizeAcrossScale = false;

  m_FirstSmoothingFilter = FirstGaussianFilterType::New();
  m_FirstSmoothingFilter->SetOrder( FirstGaussianFilterType::ZeroOrder );
  m_FirstSmoothingFilter->SetDirection( 0 );
  m_FirstSmoothingFilter->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
  m_FirstSmoothingFilter->ReleaseDataFlagOn();

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ] = InternalGaussianFilterType::New();
    m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::ZeroOrder );
    m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( m_NormalizeAcrossScale );
    m_SmoothingFilters[ i ]->SetDirection( i+1 );
    m_SmoothingFilters[ i ]->ReleaseDataFlagOn();
    }


  m_SmoothingFilters[0]->SetInput( m_FirstSmoothingFilter->GetOutput() );
  for( unsigned int i = 1; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetInput( 
      m_SmoothingFilters[i-1]->GetOutput() );
    }
  
  m_CastingFilter = CastingFilterType::New();
  m_CastingFilter->SetInput(m_SmoothingFilters[ImageDimension-2]->GetOutput());
  
  this->SetSigma( 1.0 );
}



/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void 
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetSigma( ScalarRealType sigma )
{

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetSigma( sigma );
    }
  m_FirstSmoothingFilter->SetSigma( sigma );

  this->Modified();

}



/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void 
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( bool normalize )
{

  m_NormalizeAcrossScale = normalize;

  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( normalize );
    }
  m_FirstSmoothingFilter->SetNormalizeAcrossScale( normalize );

  this->Modified();

}

/**
 * Set Order
 */
template <typename TInputImage, typename TOutputImage>
void 
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetOrder( const unsigned int order )
{
  if ( this->m_Order != order )
  {
    this->m_Order = order;
    this->Modified();

    for ( unsigned int i = 0; i < ImageDimension - 1; i++ )
    {
      if ( order == 0 )
      {
        m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::ZeroOrder );
      }
      else if ( order == 1 )
      {
        m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::FirstOrder );
      }
      else if ( order == 2 )
      {
        m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::SecondOrder );
      }
    }
    if ( order == 0 )
    {
      m_FirstSmoothingFilter->SetOrder( FirstGaussianFilterType::ZeroOrder );
    }
    else if ( order == 1 )
    {
      m_FirstSmoothingFilter->SetOrder( FirstGaussianFilterType::FirstOrder );
    }
    else if ( order == 2 )
    {
      m_FirstSmoothingFilter->SetOrder( FirstGaussianFilterType::SecondOrder );
    }
  }

} // end SetOrder()


//
//
//
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
    ::InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  if( image )
    {
    image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
    }
}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if (out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}

/**
 * Compute filter for Gaussian kernel
 */
template <typename TInputImage, typename TOutputImage >
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage >
::GenerateData(void)
{

  itkDebugMacro(<< "SmoothingRecursiveGaussianImageFilter generating data ");

  const typename TInputImage::ConstPointer   inputImage( this->GetInput() );

  const typename TInputImage::RegionType region = inputImage->GetRequestedRegion();
  const typename TInputImage::SizeType   size   = region.GetSize();
 
  for( unsigned int d=0; d < ImageDimension; d++)
    {
    if( size[d] < 4 )
      {
      itkExceptionMacro("The number of pixels along dimension " << d << " is less than 4. This filter requires a minimum of four pixels along the dimension to be processed.");
      }
    }


  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Register the filter with the with progress accumulator using
  // equal weight proportion
  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
    progress->RegisterInternalFilter(m_SmoothingFilters[i],1.0 / (ImageDimension));
    }

  progress->RegisterInternalFilter(m_FirstSmoothingFilter,1.0 / (ImageDimension));
  m_FirstSmoothingFilter->SetInput( inputImage );
  m_CastingFilter->Update();
  this->GraftOutput(m_CastingFilter->GetOutput());

}


template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "NormalizeAcrossScale: " << m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif
