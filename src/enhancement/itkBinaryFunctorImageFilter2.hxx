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
#ifndef __itkBinaryFunctorImageFilter2_hxx
#define __itkBinaryFunctorImageFilter2_hxx

#include "itkBinaryFunctorImageFilter2.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage1, class TInputImage2, class TOutputImage >
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::BinaryFunctorImageFilter2()
{
  this->SetNumberOfRequiredInputs(2);
  this->InPlaceOff();
}

/**
 * Connect one of the operands for pixel-wise addition
 */
template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput1(const TInputImage1 *image1)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput( 0, const_cast< TInputImage1 * >( image1 ) );
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput1(const DecoratedInput1ImagePixelType * input1)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput( 0, const_cast< DecoratedInput1ImagePixelType * >( input1 ) );
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput1(const Input1ImagePixelType & input1)
{
  itkDebugMacro("setting input1 to " << input1);
  typename DecoratedInput1ImagePixelType::Pointer newInput = DecoratedInput1ImagePixelType::New();
  newInput->Set(input1);
  this->SetInput1(newInput);
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetConstant1(const Input1ImagePixelType & input1)
{
  this->SetInput1(input1);
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
const typename BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >::Input1ImagePixelType &
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::GetConstant1() const
{
  itkDebugMacro("Getting constant 1");
  const DecoratedInput1ImagePixelType *input = dynamic_cast< const DecoratedInput1ImagePixelType * >(
      this->ProcessObject::GetInput(0) );
  if( input == NULL )
    {
    itkExceptionMacro(<<"Constant 1 is not set");
    }
  return input->Get();
}


/**
 * Connect one of the operands for pixel-wise addition
 */
template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput2(const TInputImage2 *image2)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput( 1, const_cast< TInputImage2 * >( image2 ) );
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput2(const DecoratedInput2ImagePixelType * input2)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput( 1, const_cast< DecoratedInput2ImagePixelType * >( input2 ) );
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetInput2(const Input2ImagePixelType & input2)
{
  itkDebugMacro("setting input2 to " << input2);
  typename DecoratedInput2ImagePixelType::Pointer newInput = DecoratedInput2ImagePixelType::New();
  newInput->Set(input2);
  this->SetInput2(newInput);
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::SetConstant2(const Input2ImagePixelType & input2)
{
  this->SetInput2(input2);
}

template< class TInputImage1, class TInputImage2, class TOutputImage >
const typename BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >::Input2ImagePixelType &
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::GetConstant2() const
{
  itkDebugMacro("Getting constant 2");
  const DecoratedInput2ImagePixelType *input = dynamic_cast< const DecoratedInput2ImagePixelType * >(
      this->ProcessObject::GetInput(1) );
  if( input == NULL )
    {
    itkExceptionMacro(<<"Constant 2 is not set");
    }
  return input->Get();
}


template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::GenerateOutputInformation()
{
  const DataObject * input = NULL;
  Input1ImagePointer inputPtr1 =
    dynamic_cast< const TInputImage1 * >( ProcessObject::GetInput(0) );
  Input2ImagePointer inputPtr2 =
    dynamic_cast< const TInputImage2 * >( ProcessObject::GetInput(1) );

  if ( this->GetNumberOfInputs() >= 2 )
    {
    if( inputPtr1 )
      {
      input = inputPtr1;
      }
    else if( inputPtr2 )
      {
      input = inputPtr2;
      }
    else
      {
      return;
      }

    for ( unsigned int idx = 0; idx < this->GetNumberOfOutputs(); ++idx )
      {
      DataObject * output = this->GetOutput(idx);
      if ( output )
        {
        output->CopyInformation(input);
        }
      }
    }
}

/**
 * ThreadedGenerateData Performs the pixel-wise addition
 */
template< class TInputImage1, class TInputImage2, class TOutputImage >
void
BinaryFunctorImageFilter2< TInputImage1, TInputImage2, TOutputImage >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       ThreadIdType threadId)
{
  // We use dynamic_cast since inputs are stored as DataObjects.  The
  // ImageToImageFilter::GetInput(int) always returns a pointer to a
  // TInputImage1 so it cannot be used for the second input.
  Input1ImagePointer inputPtr1 =
    dynamic_cast< const TInputImage1 * >( ProcessObject::GetInput(0) );
  Input2ImagePointer inputPtr2 =
    dynamic_cast< const TInputImage2 * >( ProcessObject::GetInput(1) );
  OutputImagePointer outputPtr = this->GetOutput(0);

  if( inputPtr1 && inputPtr2 )
    {
    ImageRegionConstIterator< TInputImage1 > inputIt1(inputPtr1, outputRegionForThread);
    ImageRegionConstIterator< TInputImage2 > inputIt2(inputPtr2, outputRegionForThread);

    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);

    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt1.GoToBegin();
    inputIt2.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt1.IsAtEnd() )
      {
      outputIt.Set( m_Functor->Evaluate( inputIt1.Get(), inputIt2.Get() ) );
      ++inputIt2;
      ++inputIt1;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else if( inputPtr1 )
    {
    ImageRegionConstIterator< TInputImage1 > inputIt1(inputPtr1, outputRegionForThread);
    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);
    const Input2ImagePixelType & input2Value = this->GetConstant2();
    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt1.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt1.IsAtEnd() )
      {
      outputIt.Set( m_Functor->Evaluate( inputIt1.Get(), input2Value ) );
      ++inputIt1;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else if( inputPtr2 )
    {
    ImageRegionConstIterator< TInputImage2 > inputIt2(inputPtr2, outputRegionForThread);
    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);
    const Input1ImagePixelType & input1Value = this->GetConstant1();
    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt2.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt2.IsAtEnd() )
      {
      outputIt.Set( m_Functor->Evaluate( input1Value, inputIt2.Get() ) );
      ++inputIt2;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else
    {
    itkGenericExceptionMacro(<<"At most one of the inputs can be a constant.");
    }
}
} // end namespace itk

#endif
