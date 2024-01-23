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
  Module:    $RCSfile: itkHessianRecursiveGaussianImageFilter2.txx,v $
  Language:  C++
  Date:      $Date: 2008-03-14 09:45:07 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkHessianRecursiveGaussianImageFilter2_txx_
#define _itkHessianRecursiveGaussianImageFilter2_txx_

#include "itkHessianRecursiveGaussianImageFilter2.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressAccumulator.h"

namespace itk
{


/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::HessianRecursiveGaussianImageFilter2()
{
  /** Initialize variables. */
  this->m_NormalizeAcrossScale = false;

  /** Setup the smoothing filters. */
  for( unsigned int i = 0; i < NumberOfSmoothingFilters; i++ )
  {
    GaussianFilterPointer filter = GaussianFilterType::New();
    filter->SetOrder( GaussianOrderEnum::ZeroOrder );
    filter->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );
    filter->ReleaseDataFlagOn();
    this->m_SmoothingFilters.push_back( filter );
  }

  /** Setup the derivative filters. */
  this->m_DerivativeFilterA = DerivativeFilterAType::New();
  this->m_DerivativeFilterB = DerivativeFilterBType::New();

  this->m_DerivativeFilterA->SetOrder( DerivativeFilterAType::FirstOrder );
  this->m_DerivativeFilterA->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );

  this->m_DerivativeFilterB->SetOrder( DerivativeFilterBType::FirstOrder );
  this->m_DerivativeFilterB->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );

  this->m_DerivativeFilterA->SetInput( this->GetInput() );
  this->m_DerivativeFilterB->SetInput( this->m_DerivativeFilterA->GetOutput() );

  // Deal with the 2D case.
  if( NumberOfSmoothingFilters > 0 )
  {
    this->m_SmoothingFilters[ 0 ]->SetInput( this->m_DerivativeFilterB->GetOutput() );
  }

  for( unsigned int i = 1; i < NumberOfSmoothingFilters; i++ )
  {
    this->m_SmoothingFilters[ i ]->SetInput(
      this->m_SmoothingFilters[ i - 1 ]->GetOutput() );
  }

  this->m_ImageAdaptor = OutputImageAdaptorType::New();

  /** Initialize variables. */
  this->SetSigma( 1.0 );

} // end constructor


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetSigma( ScalarRealType sigma )
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
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetSigma( const SigmaType sigma )
{
  // for now only sigma[0] is used
  if( this->m_Sigma != sigma )
  {
    this->m_Sigma = sigma;
    this->Modified();

    for( unsigned int i = 0; i < NumberOfSmoothingFilters; i++ )
    {
      this->m_SmoothingFilters[ i ]->SetSigma( sigma[ 0 ] );
    }

    this->m_DerivativeFilterA->SetSigma( sigma[ 0 ] );
    this->m_DerivativeFilterB->SetSigma( sigma[ 0 ] );

    this->Modified();
  } // end if

} // end SetSigma()


/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( const bool arg )
{

  if( this->m_NormalizeAcrossScale != arg )
  {
    this->m_NormalizeAcrossScale = arg;
    this->Modified();

    /** Pass on the argument. */
    for( unsigned int i = 0; i < NumberOfSmoothingFilters; i++ )
    {
      this->m_SmoothingFilters[ i ]->SetNormalizeAcrossScale( arg );
    }

    this->m_DerivativeFilterA->SetNormalizeAcrossScale( arg );
    this->m_DerivativeFilterB->SetNormalizeAcrossScale( arg );

  } // end if

} // end SetNormalizeAcrossScale()


//
//
//
template <typename TInputImage, typename TOutputImage>
void
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>::InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
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
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage >
::GenerateData( void )
{
  itkDebugMacro( << "HessianRecursiveGaussianImageFilter2 generating data " );

  // Create a process accumulator for tracking the progress of this
  // minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter( this );

  // Compute the contribution of each filter to the total progress.
  const double weight =
     1.0 / ( ImageDimension * ( ImageDimension * ( ImageDimension + 1 ) / 2 ) );

  for( unsigned int i = 0; i < NumberOfSmoothingFilters; i++ )
  {
    progress->RegisterInternalFilter( this->m_SmoothingFilters[ i ], weight );
  }
  progress->RegisterInternalFilter( this->m_DerivativeFilterA, weight );
  progress->RegisterInternalFilter( this->m_DerivativeFilterB, weight );

  const typename TInputImage::ConstPointer   inputImage( this->GetInput() );
  this->m_ImageAdaptor->SetImage( this->GetOutput() );
  this->m_ImageAdaptor->SetLargestPossibleRegion(
    inputImage->GetLargestPossibleRegion() );
  this->m_ImageAdaptor->SetBufferedRegion(
    inputImage->GetBufferedRegion() );
  this->m_ImageAdaptor->SetRequestedRegion(
    inputImage->GetRequestedRegion() );
  this->m_ImageAdaptor->Allocate();

  this->m_DerivativeFilterA->SetInput( inputImage );
  this->m_DerivativeFilterB->SetInput( this->m_DerivativeFilterA->GetOutput() );

  unsigned int element = 0;
  for( unsigned int dima = 0; dima < ImageDimension; dima++ )
  {
    for( unsigned int dimb = dima; dimb < ImageDimension; dimb++ )
    {
      // Manage the diagonal in a different way in order to avoid
      // applying a double smoothing to this direction, and missing
      // to smooth one of the other directions.
      if( dimb == dima )
      {
        this->m_DerivativeFilterA->SetOrder( DerivativeFilterAType::SecondOrder );
        this->m_DerivativeFilterB->SetOrder( DerivativeFilterBType::ZeroOrder );

        unsigned int i = 0;
        unsigned int j = 0;
        // find the direction for the first filter.
        while ( j < ImageDimension )
        {
          if( j != dima )
          {
            this->m_DerivativeFilterB->SetDirection( j );
            j++;
            break;
          }
          j++;
        }
        // find the direction for all the other filters
        while ( i < NumberOfSmoothingFilters )
        {
          while ( j < ImageDimension )
          {
            if( j != dima )
            {
              this->m_SmoothingFilters[ i ]->SetDirection( j );
              j++;
              break;
            }
            j++;
          }
          i++;
        }

        this->m_DerivativeFilterA->SetDirection( dima );
      }
      else
      {
        this->m_DerivativeFilterA->SetOrder( DerivativeFilterAType::FirstOrder );
        this->m_DerivativeFilterB->SetOrder( DerivativeFilterBType::FirstOrder );

        unsigned int i = 0;
        unsigned int j = 0;
        while ( i < NumberOfSmoothingFilters )
        {
          while ( j < ImageDimension )
          {
            if( j != dima && j != dimb )
            {
              this->m_SmoothingFilters[ i ]->SetDirection( j );
              j++;
              break;
            }
            j++;
          }
          i++;
        }

        this->m_DerivativeFilterA->SetDirection( dima );
        this->m_DerivativeFilterB->SetDirection( dimb );
      }

      typename RealImageType::Pointer derivativeImage;

      // Deal with the 2D case.
      if( NumberOfSmoothingFilters > 0 )
      {
        GaussianFilterPointer lastFilter = this->m_SmoothingFilters[ ImageDimension - 3 ];
        lastFilter->Update();
        derivativeImage = lastFilter->GetOutput();
      }
      else
      {
        this->m_DerivativeFilterB->Update();
        derivativeImage = this->m_DerivativeFilterB->GetOutput();
      }

      progress->ResetFilterProgressAndKeepAccumulatedProgress();

      // Copy the results to the corresponding component
      // on the output image of vectors
      this->m_ImageAdaptor->SelectNthElement( element++ );

      ImageRegionIteratorWithIndex< RealImageType > it(
        derivativeImage, derivativeImage->GetRequestedRegion() );

      ImageRegionIteratorWithIndex< OutputImageAdaptorType > ot(
        this->m_ImageAdaptor, this->m_ImageAdaptor->GetRequestedRegion() );

      const ScalarRealType spacingA = inputImage->GetSpacing()[ dima ];
      const ScalarRealType spacingB = inputImage->GetSpacing()[ dimb ];
      const ScalarRealType factor = spacingA * spacingB;

      it.GoToBegin();
      ot.GoToBegin();
      while( !it.IsAtEnd() )
      {
        ot.Set( it.Get() / factor );
        ++it;
        ++ot;
      }
    }
  }


} // end GenerateData()


template <typename TInputImage, typename TOutputImage>
void
HessianRecursiveGaussianImageFilter2<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "NormalizeAcrossScale: " << this->m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif
