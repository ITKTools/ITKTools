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
#ifndef __itkUnaryFunctors_h_
#define __itkUnaryFunctors_h_

#include "vnl/vnl_math.h"
#include "vnl/vnl_erf.h"
#include "itkNumericTraits.h"

/** All available unary operators. */
enum UnaryFunctorEnum{ PLUS, RMINUS, LMINUS, TIMES, LDIVIDE, RDIVIDE,
  RMODINT, RMODDOUBLE, LMODINT, LMODDOUBLE, NLOG, RPOWER, LPOWER, NEG,
  SIGNINT, SIGNDOUBLE, ABSINT, ABSDOUBLE, FLOOR, CEIL, ROUND,
  LN, LOG10, EXP, SIN, COS, TAN, ARCSIN, ARCCOS, ARCTAN,
  LINEAR, ERRFUNC, NORMCDF, QFUNC };

namespace itk {

namespace Functor {

/** Arithmetic functors which use m_Argument. */
template< class TInput, class TArgument=TInput, class TOutput=TInput >
class PLUS
{
public:
  PLUS() {};
  ~PLUS() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A + this->m_Argument );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class RMINUS
{
public:
  RMINUS() {};
  ~RMINUS() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A - this->m_Argument ); //==A+(-Arg)
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LMINUS
{
public:
  LMINUS() {};
  ~LMINUS() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( this->m_Argument - A );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class TIMES
{
public:
  TIMES() {};
  ~TIMES() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A * this->m_Argument );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class RDIVIDE
{
public:
  RDIVIDE() {};
  ~RDIVIDE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A / this->m_Argument );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LDIVIDE
{
public:
  LDIVIDE() {};
  ~LDIVIDE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( this->m_Argument / A );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class RMODINT
{
public:
  RMODINT() {};
  ~RMODINT() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( static_cast<int>( A ) % static_cast<int>( this->m_Argument ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class RMODDOUBLE
{
public:
  RMODDOUBLE() {};
  ~RMODDOUBLE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_fmod( static_cast<double>( A ), static_cast<double>( this->m_Argument ) ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LMODINT
{
public:
  LMODINT() {};
  ~LMODINT() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( static_cast<int>( this->m_Argument ) % static_cast<int>( A ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LMODDOUBLE
{
public:
  LMODDOUBLE() {};
  ~LMODDOUBLE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_fmod( static_cast<double>( this->m_Argument ), static_cast<double>( A ) ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class NLOG
{
public:
  NLOG() {};
  ~NLOG() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_log( static_cast<double>( A ) ) / vcl_log( static_cast<double>( this->m_Argument ) ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};


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


/** In the following classes, the argument is always double */

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class RPOWER
{
public:
  RPOWER() {};
  ~RPOWER() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::pow( static_cast<double>( A ), static_cast<double>( this->m_Argument ) ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LPOWER
{
public:
  LPOWER() {};
  ~LPOWER() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::pow( static_cast<double>( this->m_Argument ), static_cast<double>( A ) ) );
  }
  void SetArgument( TArgument arg ){ this->m_Argument = arg; };
private:
  TArgument m_Argument;
};

/** Funtions that don't use m_Argument. */

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class NEG
{
public:
  NEG() {};
  ~NEG() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( -A ); //==RMINUS 0-A
  }

private:

};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class SIGNINT
{
public:
  SIGNINT() {};
  ~SIGNINT() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vnl_math_sgn( A ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class SIGNDOUBLE
{
public:
  SIGNDOUBLE() {};
  ~SIGNDOUBLE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vnl_math_sgn( A ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ABSINT
{
public:
  ABSINT() {};
  ~ABSINT() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::abs( static_cast<int>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ABSDOUBLE
{
public:
  ABSDOUBLE() {};
  ~ABSDOUBLE() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::abs( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class FLOOR
{
public:
  FLOOR() {};
  ~FLOOR() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::floor( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class CEIL
{
public:
  CEIL() {};
  ~CEIL() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_ceil( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ROUND
{
public:
  ROUND() {};
  ~ROUND() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vnl_math_rnd( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class SQR
{
public:
  SQR() {};
  ~SQR() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vnl_math_sqr( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class SQRT
{
public:
  SQRT() {};
  ~SQRT() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::sqrt( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LN
{
public:
  LN() {};
  ~LN() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_log( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LOG10
{
public:
  LOG10() {};
  ~LOG10() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_log10( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class EXP
{
public:
  EXP() {};
  ~EXP() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_exp( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class SIN
{
public:
  SIN() {};
  ~SIN() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_sin( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class COS
{
public:
  COS() {};
  ~COS() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_cos( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class TAN
{
public:
  TAN() {};
  ~TAN() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_tan( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ARCSIN
{
public:
  ARCSIN() {};
  ~ARCSIN() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vcl_asin( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ARCCOS
{
public:
  ARCCOS() {};
  ~ARCCOS() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::acos( static_cast<double>( A ) ) );
  }

private:

};


template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ARCTAN
{
public:
  ARCTAN() {};
  ~ARCTAN() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( std::atan( static_cast<double>( A ) ) );
  }

private:

};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class ERRFUNC
{
public:
  ERRFUNC() {};
  ~ERRFUNC() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( vnl_erf( static_cast<double>( A ) ) );
  }
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class NORMCDF
{
public:
  NORMCDF() {};
  ~NORMCDF() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( 0.5 + 0.5 * vnl_erf( ( static_cast<double>( A ) - this->m_Argument1 ) *  this->m_Argument2 ) );
  }
  void SetArgument1( TArgument arg ){ this->m_Argument1 = arg; };
  void SetArgument2( TArgument arg ){ this->m_Argument2 = 1 / (arg * sqrt(2.0)); }; // Precompute the denominator for speed
private:
  TArgument m_Argument1;
  TArgument m_Argument2;
};

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class QFUNC
{
public:
  QFUNC() {};
  ~QFUNC() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( 0.5 - (0.5 * vnl_erf( ( static_cast<double>( A ) - this->m_Argument1 ) *  ( this->m_Argument2 ) ) ) );
  }
  void SetArgument1( TArgument arg ){ this->m_Argument1 = arg; };
  void SetArgument2( TArgument arg ){ this->m_Argument2 = 1 / (arg * sqrt(2.0)); }; // Precompute the denominator for speed
private:
  TArgument m_Argument1;
  TArgument m_Argument2;
};

/** More complicated functors. */

template< class TInput, class TArgument=TInput, class TOutput=TInput >
class LINEAR
{
public:
  LINEAR() {};
  ~LINEAR() {};
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( this->m_Argument1 * A + this->m_Argument2 );
  }
  void SetArgument1( TArgument arg ){ this->m_Argument1 = arg; };
  void SetArgument2( TArgument arg ){ this->m_Argument2 = arg; };
private:
  TArgument m_Argument1;
  TArgument m_Argument2;
};

} // end namespace Functor


} // end namespace itk



template< class TInputImage, class TOutputImage = TInputImage, class TArgument = typename TInputImage::PixelType >
struct UnaryFunctorFactory
{
  typename itk::InPlaceImageFilter<TInputImage, TOutputImage>::Pointer
    GetFilter( const UnaryFunctorEnum filterType, const std::vector<std::string> & strArguments )
  {
    //
    typedef typename TInputImage::PixelType   InputPixelType;
    typedef typename TOutputImage::PixelType  OutputPixelType;

    // Convert the argument to the correct type
    // NB: ">>" gives wrong results for (unsigned)char. Use the accumulate type
    // as intermediate type to avoid this. (then short will be used, and the
    // result is casted to char again).
    std::stringstream ssArgument( strArguments[ 0 ] );
    TArgument argument;
    typename itk::NumericTraits<TArgument>::AccumulateType tempArgument;
    ssArgument >> tempArgument;
    argument = static_cast<TArgument>( tempArgument );

    /** Support for second argument. */
    TArgument argument1, argument2;
    if( strArguments.size() == 2 )
    {
      argument1 = argument;

      std::stringstream ssArgument2( strArguments[ 1 ] );
      typename itk::NumericTraits<TArgument>::AccumulateType tempArgument2;
      ssArgument2 >> tempArgument2;
      argument2 = static_cast<TArgument>( tempArgument2 );
    }

    /** Create UnaryFunctorImageFilter with requested functor and set arguments. */
    if( filterType == PLUS )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::PLUS< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == RMINUS )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::RMINUS< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == LMINUS )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LMINUS< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == TIMES )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::TIMES< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == LDIVIDE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LDIVIDE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == RDIVIDE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::RDIVIDE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == RMODINT )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::RMODINT< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == RMODDOUBLE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::RMODDOUBLE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == LMODINT )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LMODINT< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == LMODDOUBLE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LMODDOUBLE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == NLOG )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::NLOG< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    /** In the following filters, the argument is always double */
    else if( filterType == RPOWER )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::RPOWER< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == LPOWER )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LPOWER< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if( filterType == ERRFUNC )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ERRFUNC< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == NORMCDF )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::NORMCDF< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument1( argument1 );
      filter->GetFunctor().SetArgument2( argument2 );
      return filter.GetPointer();
    }    
    else if( filterType == QFUNC )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::QFUNC< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument1( argument1 );
      filter->GetFunctor().SetArgument2( argument2 );
      return filter.GetPointer();
    }    

    /** The following filters do not use the argument at all.*/
    else if( filterType == NEG )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::NEG< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == SIGNINT )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::SIGNINT< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == SIGNDOUBLE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::SIGNDOUBLE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ABSINT )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ABSINT< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ABSDOUBLE )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ABSDOUBLE< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == FLOOR )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::FLOOR< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == CEIL )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::CEIL< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ROUND )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ROUND< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == LN )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LN< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == LOG10 )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LOG10< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == EXP )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::EXP< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == SIN )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::SIN< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == COS )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::COS< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == TAN )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::TAN< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ARCSIN )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ARCSIN< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ARCCOS )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ARCCOS< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == ARCTAN )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::ARCTAN< InputPixelType, TArgument, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if( filterType == LINEAR )
    {
      typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage,
        itk::Functor::LINEAR< InputPixelType, double, OutputPixelType > >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument1( argument1 );
      filter->GetFunctor().SetArgument2( argument2 );
      return filter.GetPointer();
    }
    else
    {
      std::cerr << "Selected functor is not valid!" << std::endl;
      return nullptr;
    }
  }
};

#endif //#ifndef __itkUnaryFunctors_h
