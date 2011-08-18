/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOtsuThresholdWithMaskImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009-01-26 21:45:56 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOtsuThresholdWithMaskImageFilter_txx
#define __itkOtsuThresholdWithMaskImageFilter_txx
#include "itkOtsuThresholdWithMaskImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkOtsuThresholdWithMaskImageCalculator.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
OtsuThresholdWithMaskImageFilter<TInputImage, TOutputImage>
::OtsuThresholdWithMaskImageFilter()
{
  this->m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  this->m_InsideValue    = NumericTraits<OutputPixelType>::max();
  this->m_Threshold      = NumericTraits<InputPixelType>::Zero;
  this->m_NumberOfHistogramBins = 128;
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdWithMaskImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Compute the Otsu Threshold for the input image
  typename OtsuThresholdWithMaskImageCalculator<TInputImage>::Pointer otsu =
    OtsuThresholdWithMaskImageCalculator<TInputImage>::New();
  otsu->SetImage( this->GetInput() );
  otsu->SetMaskImage( this->GetMaskImage() );
  otsu->SetNumberOfHistogramBins ( this->m_NumberOfHistogramBins);
  otsu->Compute();
  this->m_Threshold = otsu->GetThreshold();

  typename BinaryThresholdImageFilter<TInputImage,TOutputImage>::Pointer threshold =
    BinaryThresholdImageFilter<TInputImage,TOutputImage>::New();

  progress->RegisterInternalFilter(threshold,.5f);
  threshold->GraftOutput (this->GetOutput());
  threshold->SetInput (this->GetInput());
  threshold->SetLowerThreshold(NumericTraits<InputPixelType>::NonpositiveMin());
  threshold->SetUpperThreshold(otsu->GetThreshold());
  threshold->SetInsideValue ( this->m_InsideValue);
  threshold->SetOutsideValue ( this->m_OutsideValue);
  threshold->Update();

  this->GraftOutput(threshold->GetOutput());
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdWithMaskImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  TInputImage * input = const_cast<TInputImage *>(this->GetInput());
  if( input )
    {
    input->SetRequestedRegionToLargestPossibleRegion();
    }
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdWithMaskImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "OutsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>( this->m_OutsideValue) << std::endl;
  os << indent << "InsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>( this->m_InsideValue) << std::endl;
  os << indent << "NumberOfHistogramBins: "
     << this->m_NumberOfHistogramBins << std::endl;
  os << indent << "Threshold (computed): "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>( this->m_Threshold) << std::endl;

}


}// end namespace itk
#endif
