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
#ifndef __itkChannelByChannelVectorImageFilter2_txx
#define __itkChannelByChannelVectorImageFilter2_txx

#include "itkChannelByChannelVectorImageFilter2.h"

namespace itk
{
/**
 * Constructor
 */
template <class TInputImage, class TFilter, class TOutputImage>
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::ChannelByChannelVectorImageFilter2()
{

}

/**
 * Main computation method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::SetFilter(typename TFilter::Pointer filter)
{
  m_Filter = filter;
}


/**
 * Main computation method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::GenerateData()
{
  // Typedefs
  typedef itk::VectorIndexSelectionCastImageFilter<TInputImage, InputScalarImageType> DisassemblerType;
  typedef itk::ImageToVectorImageFilter<InputScalarImageType> ReassemblerType;

  // Create the disassembler(s)
  std::vector<typename DisassemblerType::Pointer> disassemblers;

  for(unsigned int inputId = 0; inputId < this->GetNumberOfInputs(); inputId++)
  {
    typename DisassemblerType::Pointer disassembler = DisassemblerType::New();
    disassembler->SetInput(this->GetInput(inputId));
    disassemblers.push_back(disassembler);
  }

  // Create the re-assembler
  typename ReassemblerType::Pointer reasassembler = ReassemblerType::New();


  // Create a filter for each channel - duplicating all of the settings of the input filter
  std::vector<typename TFilter::Pointer> filters;
  filters[0] = m_Filter;

  for(unsigned int i = 1; i < this->GetInput()->GetNumberOfComponentsPerPixel(); i++)
    {
    filters[i] = dynamic_cast<FilterType*>(m_Filter->CreateAnother().GetPointer());
    }

  // Apply the filter to each channel
  for(unsigned int channel = 0; channel < this->GetInput()->GetNumberOfComponentsPerPixel(); channel++)
    {
    for(unsigned int inputId = 0; inputId < this->GetNumberOfInputs(); inputId++)
    {
      disassemblers[inputId]->SetIndex(channel);
      disassemblers[inputId]->Update();

      filters[channel]->SetInput(inputId, disassemblers[inputId]->GetOutput());
      filters[channel]->Update();
    }
    reasassembler->SetNthInput(channel, filters[channel]->GetOutput());
    }

  reasassembler->Update();

  this->GraftOutput(reasassembler->GetOutput());
}
/**
 * PrintSelf Method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // End namespace itk
#endif
