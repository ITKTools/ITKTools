#ifndef __itkBinaryLogicalFunctors_h
#define __itkBinaryLogicalFunctors_h

#include "itkAndImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkXorImageFilter.h"

enum BinaryFunctorEnum {AND, OR, XOR, ANDNOT, ORNOT, NOT_XOR, NOT_OR, NOT_AND, DUMMY};

namespace itk {

namespace Functor {

 
template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class ANDNOT
{
public:
  ANDNOT() {};
  ~ANDNOT() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( A & (!B) );
  }
};

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class ORNOT
{
public:
  ORNOT() {};
  ~ORNOT() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( A | (!B) );
  }
};

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class NOT_XOR
{
public:
  NOT_XOR() {};
  ~NOT_XOR() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( !(A ^ B) );
  }
};

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class NOT_OR
{
public:
  NOT_OR() {};
  ~NOT_OR() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( !(A | B) );
  }
};

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class NOT_AND
{
public:
  NOT_AND() {};
  ~NOT_AND() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( !(A & B) );
  }
};

template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >
class DUMMY
{
public:
  DUMMY() {};
  ~DUMMY() {};
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  {
    return static_cast<TOutput>( true );
  }
};

} // end functor namespace

} // end itk namespace



template< class TImage > 
struct BinaryLogicalFunctorFactory
{
  typename itk::InPlaceImageFilter<TImage, TImage>::Pointer GetFilter(BinaryFunctorEnum filterType)
  {
    if(filterType == AND)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::AND<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == OR)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::OR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == XOR)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::XOR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ANDNOT)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::ANDNOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ORNOT)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::ORNOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NOT_XOR)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::NOT_XOR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NOT_OR)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::NOT_OR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NOT_AND)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::NOT_AND<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == DUMMY)
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
					  itk::Functor::DUMMY<typename TImage::PixelType> >  FilterType;
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