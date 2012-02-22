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
#ifndef __itkBinaryLogicalFunctors_h_
#define __itkBinaryLogicalFunctors_h_

#include "itkAndImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkXorImageFilter.h"


enum BinaryFunctorEnum {AND, OR, XOR, ANDNOT, ORNOT, NOT_XOR, NOT_OR, NOT_AND, DUMMY};
std::ostream & operator<<( std::ostream& os, const BinaryFunctorEnum & functor )
{
  switch( functor )
  {
  case AND:     os << "AND";      break;
  case OR:      os << "OR";       break;
  case XOR:     os << "XOR";      break;
  case ANDNOT:  os << "ANDNOT";   break;
  case ORNOT:   os << "ORNOT";    break;
  case NOT_XOR: os << "NOT_XOR";  break;
  case NOT_OR:  os << "NOT_OR";   break;
  case NOT_AND: os << "NOT_AND";  break;
  case DUMMY:   os << "DUMMY";    break;
  }
  return os;
}

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
  typename itk::InPlaceImageFilter<TImage, TImage>::Pointer
    GetFilter( BinaryFunctorEnum filterType )
  {
    if( filterType == AND )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::AND<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == OR )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::OR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == XOR )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::XOR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ANDNOT )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::ANDNOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ORNOT )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::ORNOT<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == NOT_XOR )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::NOT_XOR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == NOT_OR )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::NOT_OR<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == NOT_AND )
    {
      typedef itk::BinaryFunctorImageFilter<TImage, TImage, TImage,
        itk::Functor::NOT_AND<typename TImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == DUMMY )
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

