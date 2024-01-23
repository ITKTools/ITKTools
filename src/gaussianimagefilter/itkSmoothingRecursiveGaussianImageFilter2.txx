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
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSmoothingRecursiveGaussianImageFilter2.txx,v $
  Language:  C++
  Date:      $Date: 2008-03-14 09:45:07 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSmoothingRecursiveGaussianImageFilter2_txx_
#define _itkSmoothingRecursiveGaussianImageFilter2_txx_

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
  /** Initialize variables. */
  this->m_NormalizeAcrossScale = false;

  /** Setup the first smoothing filter. */
  this->m_FirstSmoothingFilter = FirstGaussianFilterType::New();
  this->m_FirstSmoothingFilter->SetOrder( GaussianOrderEnum::ZeroOrder );
  this->m_FirstSmoothingFilter->SetDirection( 0 );
  this->m_FirstSmoothingFilter->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );
  this->m_FirstSmoothingFilter->ReleaseDataFlagOn();

  /** Setup the remaining smoothing filters. */
  for( unsigned int i = 0; i < ImageDimension - 1; i++ )
  {
    this->m_SmoothingFilters[ i ] = InternalGaussianFilterType::New();
    this->m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::ZeroOrder );
    this->m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );
    this->m_SmoothingFilters[ i ]->SetDirection( i + 1 );
    this->m_SmoothingFilters[ i ]->ReleaseDataFlagOn();
  }

  /** Connect the pipeline. */
  this->m_SmoothingFilters[ 0 ]->SetInput( this->m_FirstSmoothingFilter->GetOutput() );
  for( unsigned int i = 1; i < ImageDimension - 1; i++ )
  {
    this->m_SmoothingFilters[ i ]->SetInput(
      this->m_SmoothingFilters[ i - 1 ]->GetOutput() );
  }

  /** And finally a cast to the desired output type. */
  this->m_CastingFilter = CastingFilterType::New();
  this->m_CastingFilter->SetInput(
    this->m_SmoothingFilters[ ImageDimension - 2 ]->GetOutput() );

  /** Initialize variables. */
  this->SetSigma( 1.0 );
  this->SetOrder( 0 );

} // end Constructor


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetSigma( const ScalarRealType sigma )
{
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma );
  this->SetSigma( sigmaFA );
} // end SetSigma()


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetSigma( const SigmaType sigma )
{
  if( this->m_Sigma != sigma )
  {
    this->m_Sigma = sigma;
    this->Modified();

    /** Pass on the sigma. */
    this->m_FirstSmoothingFilter->SetSigma( sigma[ 0 ] );
    for( unsigned int i = 0; i < ImageDimension - 1; i++ )
    {
      this->m_SmoothingFilters[ i ]->SetSigma( sigma[ i + 1 ] );
    }
  } // end if

} // end SetSigma()


/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( const bool arg )
{
  if( this->m_NormalizeAcrossScale != arg )
  {
    this->m_NormalizeAcrossScale = arg;
    this->Modified();

    /** Pass on the argument. */
    this->m_FirstSmoothingFilter->SetNormalizeAcrossScale( arg );
    for( unsigned int i = 0; i < ImageDimension - 1; i++ )
    {
      this->m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( arg );
    }
  } // end if

} // end SetNormalizeAcrossScale()


/**
 * Set value of Order
 */
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetOrder( const unsigned int order )
{
  OrderType orderFA;
  orderFA.Fill( order );
  this->SetOrder( orderFA );
} // end SetOrder()


/**
 * Set Order
 */
template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetOrder( const OrderType order )
{
  if( this->m_Order != order )
  {
    this->m_Order = order;
    this->Modified();

    /** Set the order for the first smoothing filter. */
    if( order[ 0 ] == 0 )
    {
      this->m_FirstSmoothingFilter->SetOrder( GaussianOrderEnum::ZeroOrder );
    }
    else if( order[ 0 ] == 1 )
    {
      this->m_FirstSmoothingFilter->SetOrder( GaussianOrderEnum::FirstOrder );
    }
    else if( order[ 0 ] == 2 )
    {
      this->m_FirstSmoothingFilter->SetOrder( GaussianOrderEnum::SecondOrder );
    }
    //else warning??

    /** Set the order for the last smoothing filters. */
    for( unsigned int i = 0; i < ImageDimension - 1; i++ )
    {
      if( order[ i + 1 ] == 0 )
      {
        this->m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::ZeroOrder );
      }
      else if( order[ i + 1 ] == 1 )
      {
        this->m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::FirstOrder );
      }
      else if( order[ i + 1 ] == 2 )
      {
        this->m_SmoothingFilters[ i ]->SetOrder( InternalGaussianFilterType::SecondOrder );
      }
      //else warning??
    } // end for

  } // end if

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

  if(out)
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
::GenerateData( void )
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
    progress->RegisterInternalFilter( this->m_SmoothingFilters[ i ],1.0 / (ImageDimension));
    }

  progress->RegisterInternalFilter( this->m_FirstSmoothingFilter,1.0 / (ImageDimension));
  this->m_FirstSmoothingFilter->SetInput( inputImage );
  this->m_CastingFilter->Update();
  this->GraftOutput( this->m_CastingFilter->GetOutput());

}


template <typename TInputImage, typename TOutputImage>
void
SmoothingRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "NormalizeAcrossScale: " << this->m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif
