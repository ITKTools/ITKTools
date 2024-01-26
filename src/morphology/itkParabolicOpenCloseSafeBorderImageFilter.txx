#ifndef __itkParabolicOpenCloseSafeBorderImageFilter_txx
#define __itkParabolicOpenCloseSafeBorderImageFilter_txx

#include "itkProgressAccumulator.h"

namespace itk
{

template <typename TInputImage, bool doOpen, typename TOutputImage>
void
ParabolicOpenCloseSafeBorderImageFilter<TInputImage, doOpen, TOutputImage>
::GenerateData( void )
{

  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  typedef typename TInputImage::SizeType      SizeType;
  typedef typename TInputImage::SizeValueType SizeValueType;

  // Allocate the output
  this->AllocateOutputs();
  InputImageConstPointer inputImage;
  SizeValueType Bounds[ImageDimension];
  SizeType BoundsSize;
  if(this->m_SafeBorder)
    {
    // need to compute some image statistics and determine the padding
    // extent. This will almost certainly be an over estimate
    this->m_StatsFilt->SetInput(this->GetInput());
    this->m_StatsFilt->Update();
    InputPixelType range = this->m_StatsFilt->GetMaximum() - this->m_StatsFilt->GetMinimum();
    typename MorphFilterType::RadiusType Sigma = this->m_MorphFilt->GetScale();
    typename TInputImage::SpacingType spcing = this->GetInput()->GetSpacing();
    for (unsigned s = 0; s < ImageDimension;s++)
      {
      if( this->m_MorphFilt->GetUseImageSpacing())
      {
        RealType image_scale =spcing[s];
        Bounds[s] = (SizeValueType)ceil(sqrt(2*(Sigma[s]/(image_scale*image_scale))*range));
        BoundsSize[s] = Bounds[s];
      }
      else
      {
        Bounds[s] = (SizeValueType)ceil(sqrt(2*Sigma[s]*range));
        BoundsSize[s] = Bounds[s];
      }
      }

    this->m_PadFilt->SetPadLowerBound(Bounds);
    this->m_PadFilt->SetPadUpperBound(Bounds);
    this->m_PadFilt->SetConstant(NumericTraits<InputPixelType>::max());
    this->m_PadFilt->SetInput( this->GetInput());
    progress->RegisterInternalFilter( this->m_PadFilt, 0.1f );
    inputImage = this->m_PadFilt->GetOutput();
    }
  else
    {
    inputImage = this->GetInput();
    }

  this->m_MorphFilt->SetInput(inputImage);
  progress->RegisterInternalFilter( this->m_MorphFilt, 0.8f);

  if(this->m_SafeBorder)
    {
    // crop
    this->m_CropFilt->SetInput( this->m_MorphFilt->GetOutput());
    this->m_CropFilt->SetUpperBoundaryCropSize(BoundsSize);
    this->m_CropFilt->SetLowerBoundaryCropSize(BoundsSize);
    progress->RegisterInternalFilter( this->m_CropFilt, 0.1f );
    this->m_CropFilt->GraftOutput( this->GetOutput() );
    this->m_CropFilt->Update();
    this->GraftOutput( this->m_CropFilt->GetOutput() );
    }
  else
    {
    this->m_MorphFilt->GraftOutput( this->GetOutput() );
    this->m_MorphFilt->Update();
    this->GraftOutput( this->m_MorphFilt->GetOutput() );
    // std::cout << "Finished grafting" << std::endl;
    }

}


template<typename TInputImage, bool doOpen, typename TOutputImage>
void
ParabolicOpenCloseSafeBorderImageFilter<TInputImage, doOpen, TOutputImage>
::Modified() const
{
  Superclass::Modified();
  this->m_MorphFilt->Modified();
  this->m_PadFilt->Modified();
  this->m_CropFilt->Modified();
  this->m_StatsFilt->Modified();
}


///////////////////////////////////
template<typename TInputImage, bool doOpen, typename TOutputImage>
void
ParabolicOpenCloseSafeBorderImageFilter<TInputImage, doOpen, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  os << indent << "SafeBorder: " << this->m_SafeBorder << std::endl;
  if( this->GetUseImageSpacing() )
    {
    os << "Scale in world units: " << this->GetScale() << std::endl;
    }
  else
    {
    os << "Scale in voxels: " << this->GetScale() << std::endl;
    }
}
} //namespace itk

#endif
