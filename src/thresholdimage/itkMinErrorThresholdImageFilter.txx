/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMinErrorThresholdImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2010-05-04 13:18:45 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMinErrorThresholdImageFilter_txx
#define _itkMinErrorThresholdImageFilter_txx
#include "itkMinErrorThresholdImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkMinErrorThresholdImageCalculator.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
MinErrorThresholdImageFilter<TInputImage, TOutputImage>
::MinErrorThresholdImageFilter()
{
  this->m_OutsideValue   = NumericTraits<OutputPixelType>::max();
  this->m_InsideValue    = NumericTraits<OutputPixelType>::Zero;
  this->m_Threshold      = NumericTraits<InputPixelType>::Zero;
  this->m_NumberOfHistogramBins = 128;
  this->m_AlphaLeft = 0.0;
  this->m_AlphaRight= 0.0;
  this->m_PriorLeft = 0.0;
  this->m_PriorRight= 0.0;
  this->m_MixtureType= 1;
}

template<class TInputImage, class TOutputImage>
void
MinErrorThresholdImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Compute the MinError Threshold for the input image
  typename MinErrorThresholdImageCalculator<TInputImage>::Pointer MinError =
    MinErrorThresholdImageCalculator<TInputImage>::New();
  MinError->SetImage (this->GetInput());
  MinError->SetNumberOfHistogramBins ( this->m_NumberOfHistogramBins);
  if( this->m_MixtureType == 1)
	MinError->UseGaussianMixture(false);
  else
  	MinError->UseGaussianMixture(true);
  MinError->Compute();

  //Get the threshold and the estimated mixture parameters
  this->m_Threshold = MinError->GetThreshold();
  this->m_AlphaLeft = MinError->GetAlphaLeft();
  this->m_AlphaRight = MinError->GetAlphaRight();
  this->m_PriorLeft = MinError->GetPriorLeft();
  this->m_PriorRight= MinError->GetPriorRight();
  if( this->m_MixtureType==1)
    {
    this->m_StdLeft= MinError->GetStdLeft();
    this->m_StdRight= MinError->GetStdRight();
    }

  //Once we have the threshold, binarize the image using "BinaryThresholdImageFilter"
  typename BinaryThresholdImageFilter<TInputImage,TOutputImage>::Pointer threshold =
    BinaryThresholdImageFilter<TInputImage,TOutputImage>::New();

  //progress->RegisterInternalFilter(threshold,.5f);
  //threshold->GraftOutput (this->GetOutput());
  threshold->SetInput (this->GetInput());
  threshold->SetLowerThreshold(NumericTraits<InputPixelType>::NonpositiveMin());
  threshold->SetUpperThreshold(MinError->GetThreshold());
  threshold->SetInsideValue ( this->m_InsideValue);
  threshold->SetOutsideValue ( this->m_OutsideValue);
  threshold->Update();

  this->GraftOutput(threshold->GetOutput());
}

template<class TInputImage, class TOutputImage>
void
MinErrorThresholdImageFilter<TInputImage, TOutputImage>
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
MinErrorThresholdImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  //Superclass::PrintSelf(os,indent);
  os << indent << "Computed Threshold : "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>( this->m_Threshold) << std::endl;
  os << indent << "Estimated Mixture Parameters : "<<std::endl;
  os << indent << "Background Mean="<<m_AlphaLeft<<"  Foreground Mean="<<m_AlphaRight<<std::endl;
  os << indent << "Background Prior="<<m_PriorLeft<<"  Foreground Prior="<<m_PriorRight<<std::endl;
  if( this->m_MixtureType==1)
    os << indent << "Background Stdv="<<m_StdLeft<<"  Foreground Stdv="<<m_StdRight<<std::endl;

}


}// end namespace itk
#endif
