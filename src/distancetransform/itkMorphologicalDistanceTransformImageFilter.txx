#ifndef __itkMorphologicalDistanceTransformImageFilter_txx
#define __itkMorphologicalDistanceTransformImageFilter_txx

#include "itkMorphologicalDistanceTransformImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk 
{
template <typename TInputImage, typename TOutputImage> 
MorphologicalDistanceTransformImageFilter<TInputImage, TOutputImage>
::MorphologicalDistanceTransformImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfRequiredInputs( 1 );

  this->m_Erode = ErodeType::New();
  this->m_Thresh = ThreshType::New();
  this->m_Sqrt = SqrtType::New();
  this->m_OutsideValue = 0;
  this->m_Erode->SetScale(0.5);
  this->SetUseImageSpacing(true);
  this->m_SqrDist = false;
}

template <typename TInputImage, typename TOutputImage> 
void
MorphologicalDistanceTransformImageFilter<TInputImage, TOutputImage>
::Modified() const
{
  Superclass::Modified();
  this->m_Erode->Modified();
  this->m_Thresh->Modified();
  this->m_Sqrt->Modified();
  
}

template <typename TInputImage, typename TOutputImage> 
void
MorphologicalDistanceTransformImageFilter<TInputImage, TOutputImage>
::GenerateData( void )
{

  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  // these values are guesses at present - need to profile to get a
  // real idea
  progress->RegisterInternalFilter( this->m_Thresh, 0.1f);
  progress->RegisterInternalFilter( this->m_Erode, 0.8f);
  progress->RegisterInternalFilter( this->m_Sqrt, 0.1f);

  //std::cout << "DT" << std::endl;

  double MaxDist = 0.0;
  typename TOutputImage::SpacingType sp = this->GetOutput()->GetSpacing();
  typename TOutputImage::SizeType sz = this->GetOutput()->GetLargestPossibleRegion().GetSize();
  if(this->GetUseImageSpacing()) 
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

//   double Wt = 0.0;
//   if(this->GetUseImageSpacing()) 
//     {
//     for (unsigned k = 0; k < TOutputImage::ImageDimension; k++)
//       {
//       Wt += sp[k] * sp[k];
//       }
//     }
//   else
//     {
//     for (unsigned k = 0; k < TOutputImage::ImageDimension; k++)
//       {
//       Wt += 1.0;
//       }
//     }
//   Wt = sqrt(Wt);
  this->AllocateOutputs();

  this->m_Thresh->SetLowerThreshold( this->m_OutsideValue);
  this->m_Thresh->SetUpperThreshold( this->m_OutsideValue);
  this->m_Thresh->SetOutsideValue(MaxDist);
  this->m_Thresh->SetInsideValue(0);

  this->m_Thresh->SetInput(this->GetInput());
  this->m_Erode->SetInput( this->m_Thresh->GetOutput());
  
  if( this->m_SqrDist)
    {
    this->m_Erode->GraftOutput(this->GetOutput());
    this->m_Erode->Update();
    this->GraftOutput( this->m_Erode->GetOutput());
    }
  else
    {
    this->m_Sqrt->SetInput( this->m_Erode->GetOutput());
    this->m_Sqrt->GraftOutput(this->GetOutput());
    this->m_Sqrt->Update();
    this->GraftOutput( this->m_Sqrt->GetOutput());
    }
  
}
template <typename TInputImage, typename TOutputImage> 
void
MorphologicalDistanceTransformImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "Outside Value = " << (OutputPixelType)m_OutsideValue << std::endl;
  os << "ImageScale = " << this->m_Erode->GetUseImageSpacing() << std::endl;

}

} //namespace itk

#endif
