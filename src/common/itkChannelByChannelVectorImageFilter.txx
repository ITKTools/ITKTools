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

    FilterPointerType filter = FilterType::New();
    for(unsigned int inputId = 0; inputId < this->GetNumberOfInputs(); inputId++)
      {
      disassemblers[inputId]->SetIndex(channel);
      disassemblers[inputId]->Update();
      filter->SetInput(inputId, disassemblers[inputId]->GetOutput());
      }
    filter->Update();

    reasassembler->SetNthInput(channel, filter->GetOutput());
    }

  reasassembler->Update();

  this->GraftOutput(reasassembler->GetOutput());
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
