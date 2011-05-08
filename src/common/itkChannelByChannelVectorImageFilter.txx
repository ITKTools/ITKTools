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
