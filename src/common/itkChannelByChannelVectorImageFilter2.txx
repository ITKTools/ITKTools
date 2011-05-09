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
  //m_Filters.resize(); // number of channels can't be known at this point
}

/**
 * Set all the filters to the exact same filter.
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::SetAllFilters(typename TFilter::Pointer filter)
{
  // Create a filter for each channel - duplicating all of the settings of the input filter
  for(unsigned int channel = 0; channel < numberOfChannels; ++channel) // how to know the number of channels at this point?
  {
    m_Filters[channel] = dynamic_cast<FilterType*>(filter->CreateAnother().GetPointer());
  }
}

/**
 * Set the filter to operate on a particular channel.
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::SetFilter(unsigned int channel, typename TFilter::Pointer filter)
{
  // Duplicate the filter for the specified channel
  m_Filters[channel] = dynamic_cast<FilterType*>(filter->CreateAnother().GetPointer());
}

/**
 * Main computation method
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::GenerateData()
{
  // If no filters were specified, create a new, default one for each channel
  if(m_Filters.size() == 0)
  {
    for(unsigned int channel = 0; channel < numberOfChannels; ++channel) // how to know the number of channels at this point?
    {
      m_Filters[channel] = FilterType::New();
    }
  }
  
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
