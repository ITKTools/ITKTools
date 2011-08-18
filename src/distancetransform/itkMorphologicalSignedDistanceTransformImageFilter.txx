#ifndef __itkMorphologicalSignedDistanceTransformImageFilter_txx
#define __itkMorphologicalSignedDistanceTransformImageFilter_txx

#include "itkMorphologicalSignedDistanceTransformImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk 
{
template <typename TInputImage, typename TOutputImage> 
MorphologicalSignedDistanceTransformImageFilter<TInputImage, TOutputImage>
::MorphologicalSignedDistanceTransformImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfRequiredInputs( 1 );

  this->m_Erode = ErodeType::New();
  this->m_Dilate = DilateType::New();
  this->m_Thresh = ThreshType::New();
  this->m_Helper = HelperType::New();
  this->m_Erode->SetScale(0.5);
  this->m_Dilate->SetScale(0.5);
  this->SetUseImageSpacing(true);
  this->SetInsideIsPositive(false);
  this->m_OutsideValue = 0;

}
template <typename TInputImage, typename TOutputImage> 
void
MorphologicalSignedDistanceTransformImageFilter<TInputImage, TOutputImage>
::Modified() const
{
  Superclass::Modified();
  this->m_Erode->Modified();
  this->m_Dilate->Modified();
  this->m_Thresh->Modified();
  this->m_Helper->Modified();
}

template <typename TInputImage, typename TOutputImage> 
void
MorphologicalSignedDistanceTransformImageFilter<TInputImage, TOutputImage>
::GenerateData( void )
{

  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  // these values are guesses at present - need to profile to get a
  // real idea
  progress->RegisterInternalFilter( this->m_Thresh, 0.1f);
  progress->RegisterInternalFilter( this->m_Erode, 0.4f);
  progress->RegisterInternalFilter( this->m_Dilate, 0.4f);
  progress->RegisterInternalFilter( this->m_Helper, 0.1f);

  this->AllocateOutputs();
  // figure out the maximum value of distance transform using the
  // image dimensions
  typename TOutputImage::SizeType sz = this->GetOutput()->GetRequestedRegion().GetSize();
  typename TOutputImage::SpacingType sp = this->GetOutput()->GetSpacing();

  double MaxDist = 0.0;
  if (this->GetUseImageSpacing()) 
    {
    for (unsigned k = 0; k < TOutputImage::ImageDimension; k++)
      {
      double thisdim = (sz[k] * sp[k]);
      MaxDist += thisdim*thisdim;
      }
    }
  else
    {
    for (unsigned k = 0; k < TOutputImage::ImageDimension; k++)
      {
      double thisdim = sz[k];
      MaxDist += thisdim*thisdim;
      }
    }

  this->m_Thresh->SetLowerThreshold( this->m_OutsideValue);
  this->m_Thresh->SetUpperThreshold( this->m_OutsideValue);
  if (this->GetInsideIsPositive())
    {
    this->m_Thresh->SetOutsideValue(MaxDist);
    this->m_Thresh->SetInsideValue(-MaxDist);
    }
  else
    {
    this->m_Thresh->SetOutsideValue(-MaxDist);
    this->m_Thresh->SetInsideValue(MaxDist);
    }

  this->m_Thresh->SetInput(this->GetInput());
  this->m_Erode->SetInput( this->m_Thresh->GetOutput());
  this->m_Dilate->SetInput( this->m_Thresh->GetOutput());
#if 1
  this->m_Helper->SetInput( this->m_Erode->GetOutput());
  this->m_Helper->SetInput2( this->m_Dilate->GetOutput());
  this->m_Helper->SetInput3( this->m_Thresh->GetOutput());
  this->m_Helper->SetVal(MaxDist);
  this->m_Helper->GraftOutput(this->GetOutput());
  this->m_Helper->Update();
  this->GraftOutput( this->m_Helper->GetOutput());
#else
  this->m_Dilate->GraftOutput(this->GetOutput());
  this->m_Dilate->Update();
  this->GraftOutput( this->m_Dilate->GetOutput());
#endif

}
template <typename TInputImage, typename TOutputImage> 
void
MorphologicalSignedDistanceTransformImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "Outside Value = " << (OutputPixelType)m_OutsideValue << std::endl;
  os << "ImageScale = " << this->m_Erode->GetUseImageSpacing() << std::endl;

}

} //namespace itk

#endif
