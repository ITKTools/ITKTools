#ifndef __itkUnaryLogicalFunctors_h
#define __itkUnaryLogicalFunctors_h

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
  typename itk::InPlaceImageFilter<TImage, TImage>::Pointer GetFilter(UnaryFunctorEnum filterType)
  {
    if(filterType == EQUAL)
    {
      typedef itk::UnaryFunctorImageFilter<TImage, TImage, 
					  itk::Functor::EQUAL<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NOT)
    {
      typedef itk::UnaryFunctorImageFilter<TImage, TImage, 
					  itk::Functor::NOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else
    {
      return NULL;
    }
  }
};
#endif