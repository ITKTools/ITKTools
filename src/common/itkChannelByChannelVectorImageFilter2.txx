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
  m_SingleFilter = NULL;
}

/**
 * Set all the filters to the exact same filter.
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::SetAllFilters(typename TFilter::Pointer filter)
{
  // For now, just store this filter. It will be applied to each channel later.
  m_SingleFilter = dynamic_cast<FilterType*>(filter->CreateAnother().GetPointer());
}

/**
 * Set the filter to operate on a particular channel.
 */
template <class TInputImage, class TFilter, class TOutputImage>
void
ChannelByChannelVectorImageFilter2<TInputImage, TFilter, TOutputImage>
::SetFilter(unsigned int channel, typename TFilter::Pointer filter)
{
  // If necessary, expand the m_Filters vector and set the new elements to NULL
  if(m_Filters.size() - 1 < channel)
    {
    m_Filters.resize(channel, NULL);
    }

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
  // One of two conditions must be true:
  // 1) The number of channels in the input matches the size of the m_Filters vector
  // 2) m_SingleFilter is set
  
  // Case 1 - the number of channels in the input matches the size of m_Filters
  bool valid = false;
  if(m_Filters.size() == this->GetInput()->GetNumberOfComponentsPerPixel())
  {
    // If all filters have been set, we can proceed with case 1
    valid = true;

    // Ensure every filter has been set
    for(unsigned int channel = 0; channel < numberOfChannels; ++channel)
      {
      if(!m_Filters[channel])
	{
	valid = false;
	}
      }
  }
  
  // Case 2 - m_SingleFilter is set
  if(m_SingleFilter)
    {
    if(valid) // if case 1 was true, the filter does not know which method to use!
      {
      std::cerr << "You must set EITHER m_SingleFilter OR all of the filters (one per channel)" << std::endl;
      return;
      }
    // Create a filter for each channel - duplicating all of the settings of the input filter
    for(unsigned int channel = 0; channel < numberOfChannels; ++channel) // how to know the number of channels at this point?
      {
      m_Filters[channel] = dynamic_cast<FilterType*>(filter->CreateAnother().GetPointer());
      }
    valid = true;
    }
  
  if(!valid)
    {
    std::cerr << "Neither method was specified!" << std::endl;
    return;
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
