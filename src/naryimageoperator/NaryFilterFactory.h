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
#ifndef NaryFilterFactory_h_
#define NaryFilterFactory_h_

#include "itkInPlaceImageFilter.h"

enum NaryFilterEnum {ADDITION, MEAN, MINUS, TIMES, DIVIDE, MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE, NARYMAGNITUDE};

template <class TInputImage, class TOutputImage>
class NaryFilterFactory
{
public:
  typename itk::InPlaceImageFilter<TInputImage, TOutputImage>::Pointer GetFilter(NaryFilterEnum filterType)
  {
    if(filterType == ADDITION)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryADDITION<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MEAN)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryMEAN<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MINUS)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryMINUS<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == TIMES)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryTIMES<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == DIVIDE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryDIVIDE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MAXIMUM)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryMAXIMUM<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MINIMUM)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryMINIMUM<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ABSOLUTEDIFFERENCE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryABSOLUTEDIFFERENCE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NARYMAGNITUDE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
					  itk::Functor::NaryNARYMAGNITUDE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else
    {
      std::cerr << "Invalid filter type specified!" << std::endl;
      return NULL;
    }
  }
};

/** Macros for easily instantiating the correct binary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::NaryFunctorImageFilter<
 *   InputImageType,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixelType,OutputPixelType> > PLUSFilterType;
 * naryFilter = (PLUSFilterType::New()).GetPointer();
 *
 */
#define InstantiateNaryFilterNoArg( name ) \
  typedef itk::NaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::Nary##name<InputPixelType, OutputPixelType> > name##FilterType; \
  if( naryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempNaryFilter = name##FilterType::New(); \
    tempNaryFilter->InPlaceOn(); \
    naryFilter = tempNaryFilter.GetPointer(); \
  }

#endif
