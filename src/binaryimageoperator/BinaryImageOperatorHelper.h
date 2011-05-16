/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __BinaryImageOperatorHelper_h
#define __BinaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkBinaryFunctors.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <vector>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------

/** Macros for easily instantiating the correct binary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::BinaryFunctorImageFilter<
 *   InputImage1Type,
 *   InputImage2Type,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixel1Type,InputPixel2Type,OutputPixelType> > PLUSFilterType;
 * binaryFilter = (PLUSFilterType::New()).GetPointer();
 * 
 */
#define InstantiateBinaryFilterNoArg( name ) \
  typedef itk::BinaryFunctorImageFilter< \
    InputImage1Type, InputImage2Type, OutputImageType, \
    itk::Functor::name<InputPixel1Type, InputPixel2Type, OutputPixelType> > name##FilterType; \
  if ( binaryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempBinaryFilter = name##FilterType::New(); \
    binaryFilter = tempBinaryFilter.GetPointer(); \
  }

#define InstantiateBinaryFilterWithArg( name ) \
  typedef itk::BinaryFunctorImageFilter< \
    InputImage1Type, InputImage2Type, OutputImageType, \
    itk::Functor::name<InputPixel1Type, InputPixel2Type, OutputPixelType> > name##FilterType; \
  if ( binaryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempBinaryFilter = name##FilterType::New(); \
    tempBinaryFilter->GetFunctor().SetArgument( argument ); \
    binaryFilter = tempBinaryFilter.GetPointer(); \
  }


#endif //#ifndef __BinaryImageOperatorHelper_h
