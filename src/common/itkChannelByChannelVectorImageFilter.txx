/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkChannelByChannelVectorImageFilter_txx
#define __itkChannelByChannelVectorImageFilter_txx

#include "itkChannelByChannelVectorImageFilter.h"

namespace itk
{
/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
ChannelByChannelVectorImageFilter<TInputImage, TOutputImage>
::ChannelByChannelVectorImageFilter2()
{
  m_SingleFilter = NULL;
}

/**
 * Set all the filters to the exact same filter.
 */
template <class TInputImage, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TOutputImage>
::SetAllFilters(typename TFilter::Pointer filter)
{
  // For now, just store this filter. It will be applied to each channel later.
  m_SingleFilter = dynamic_cast<FilterType*>(filter->CreateAnother().GetPointer());
}

/**
 * Set the filter to operate on a particular channel.
 */
template <class TInputImage, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TOutputImage>
::SetFilterForSingleChannel(unsigned int channel, typename TFilter::Pointer filter)
{
  // If necessary, expand the m_Filters vector and set the new elements to NULL
  if(m_Filters.size() - 1 < channel)
    {
    m_Filters.resize(channel, NULL);
    }

  // Store the filter for the specified channel
  m_Filters[channel] = filter;
}

/**
 * Main computation method
 */
template <class TInputImage, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TOutputImage>
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
      m_Filters[channel] = m_SingleFilter;
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

  // Create a vector to store the disassemblers. We will need one for each input to the filter.
  std::vector<typename DisassemblerType::Pointer> disassemblers;

  // Create a vector to store the outputs.
  std::vector<InputScalarImageType::Pointer> outputs;

  // Add each input image to its own disassembler
  for(unsigned int inputId = 0; inputId < this->GetNumberOfInputs(); inputId++)
    {
    typename DisassemblerType::Pointer disassembler = DisassemblerType::New();
    disassembler->SetInput(this->GetInput(inputId));
    disassemblers.push_back(disassembler);
    }

  // Create the re-assembler. Only one is needed.
  typename ReassemblerType::Pointer reasassembler = ReassemblerType::New();

  // Apply the filter to each channel
  for(unsigned int channel = 0; channel < this->GetInput()->GetNumberOfComponentsPerPixel(); channel++)
    {
    // Get the 'channel'th channel of every input and feed it to the filter
    for(unsigned int inputId = 0; inputId < this->GetNumberOfInputs(); inputId++)
      {
      disassemblers[inputId]->SetIndex(channel);
      disassemblers[inputId]->Update();

      filters[channel]->SetInput(inputId, disassemblers[inputId]->GetOutput());
      filters[channel]->Update();
      outputs.push_back(filters[channel]->GetOutput());
      filters[channel]->DisconnectPipeline();
      }

    // Add the 'channel'th output to the output vector image
    reasassembler->SetNthInput(channel, outputs[channel]);
    }

  reasassembler->Update();

  // Copy the output (reassembled) to the output of the filter.
  this->GraftOutput(reasassembler->GetOutput());
}
/**
 * PrintSelf Method
 */
template <class TInputImage, class TOutputImage>
void
ChannelByChannelVectorImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // End namespace itk
#endif
