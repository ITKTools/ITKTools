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
#ifndef __itkUnaryLogicalFunctors_h_
#define __itkUnaryLogicalFunctors_h_

#include "itkNotImageFilter.h"

enum UnaryFunctorEnum {EQUAL, NOT};

namespace itk {

namespace Functor {
  
template< class TInput, class TArgument=TInput, class TOutput=TInput > 
class EQUAL
{
public:
  EQUAL() {};
  ~EQUAL() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A == this->m_Argument );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

} // end functor namespace

} // end itk namespace


template< class TImage > 
struct UnaryLogicalFunctorFactory
{
  typename itk::InPlaceImageFilter<TImage, TImage>::Pointer
    GetFilter( UnaryFunctorEnum filterType, typename TImage::PixelType argument )
  {
    if( filterType == EQUAL )
    {
      typedef itk::UnaryFunctorImageFilter<TImage, TImage, 
					  itk::Functor::EQUAL<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument(argument);
      return filter.GetPointer();
    }
    else if( filterType == NOT )
    {
      typedef itk::UnaryFunctorImageFilter<TImage, TImage, 
					  itk::Functor::NOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      // 'argument' not used for this filter
      return filter.GetPointer();
    }
    else
    {
      return NULL;
    }
  }
}; // end struct UnaryLogicalFunctorFactory

#endif
