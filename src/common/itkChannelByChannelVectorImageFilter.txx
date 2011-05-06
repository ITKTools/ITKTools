/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkChannelByChannelVectorImageFilter_txx
#define __itkChannelByChannelVectorImageFilter_txx

#include "itkChannelByChannelVectorImageFilter.h"

namespace itk
{
/**
 * Constructor
 */
template <class TInputImage, class TFilter, class TOutputImage>
ChannelByChannelVectorImageFilter<TInputImage, TFilter, TOutputImage>
::ChannelByChannelVectorImageFilter()
{

}


/**
 * Main computation method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TFilter, TOutputImage>
::GenerateData()
{
  // Create the disassembler
  typedef itk::VectorIndexSelectionCastImageFilter<TInputImage, InputScalarImageType> IndexSelectionType;
  typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
  indexSelectionFilter->SetInput(this->GetInput());

  // Create the re-assembler
  typedef itk::ImageToVectorImageFilter<InputScalarImageType> ImageToVectorImageFilterType;
  typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

  // Apply the filter to each channel
  for(unsigned int channel = 0; channel < this->GetInput()->GetNumberOfComponentsPerPixel(); channel++)
    {
    indexSelectionFilter->SetIndex(channel);
    indexSelectionFilter->Update();

    FilterPointerType filter = FilterType::New();
    filter->SetInput(indexSelectionFilter->GetOutput());
    filter->Update();

    imageToVectorImageFilter->SetNthInput(channel, filter->GetOutput());
    }

  imageToVectorImageFilter->Update();

  this->GraftOutput(imageToVectorImageFilter->GetOutput());
}
/**
 * PrintSelf Method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TFilter, TOutputImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // End namespace itk
#endif
