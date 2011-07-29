#ifndef __itkUnaryFunctors_h
#define __itkUnaryFunctors_h

#include "vnl/vnl_math.h"

enum UnaryFunctorEnum {PLUS, RMINUS, LMINUS, TIMES, LDIVIDE, RDIVIDE, RMODINT, RMODDOUBLE, LMODINT, LMODDOUBLE, NLOG, RPOWER, LPOWER, NEG,
                       SIGNINT, SIGNDOUBLE, ABSINT, ABSDOUBLE, FLOOR, CEIL, ROUND, LN, LOG10, EXP, SIN, COS, TAN, ARCSIN, ARCCOS, ARCTAN};

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
    return static_cast<TOutput>( vcl_pow( static_cast<double>( A ), static_cast<double>( this->m_Argument ) ) );
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
    return static_cast<TOutput>( vcl_pow( static_cast<double>( this->m_Argument ), static_cast<double>( A ) ) );
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
    return static_cast<TOutput>( vcl_abs( static_cast<int>( A ) ) );
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
    return static_cast<TOutput>( vcl_abs( static_cast<double>( A ) ) );
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
    return static_cast<TOutput>( vcl_floor( static_cast<double>( A ) ) );
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
    return static_cast<TOutput>( vcl_sqrt( static_cast<double>( A ) ) );
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
    return static_cast<TOutput>( vcl_acos( static_cast<double>( A ) ) );
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
    return static_cast<TOutput>( vcl_atan( static_cast<double>( A ) ) );
  }

private:

};

} // end namespace Functor


} // end namespace itk



template< class TInputImage, class TOutputImage = TInputImage, class TArgument = typename TInputImage::PixelType >
struct UnaryFunctorFactory
{
  typename itk::InPlaceImageFilter<TInputImage, TOutputImage>::Pointer GetFilter(UnaryFunctorEnum filterType, std::string & strArgument )
  {
    // Convert the argument to the correct type
    std::stringstream ssArgument(strArgument);
    TArgument argument;
    ssArgument >> argument;

    if(filterType == PLUS)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
                                            itk::Functor::PLUS< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == RMINUS)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::RMINUS< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == LMINUS)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LMINUS< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == TIMES)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::TIMES< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == LDIVIDE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LDIVIDE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == RDIVIDE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::RDIVIDE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == RMODINT)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::RMODINT< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == RMODDOUBLE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::RMODDOUBLE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == LMODINT)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LMODINT< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == LMODDOUBLE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LMODDOUBLE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == NLOG)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::NLOG< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    /** In the following filters, the argument is always double */
    else if(filterType == RPOWER)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::RPOWER< double> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    else if(filterType == LPOWER)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LPOWER< double> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->GetFunctor().SetArgument( argument );
      return filter.GetPointer();
    }
    /** The following filters do not use the argument at all.*/
    else if(filterType == NEG)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::NEG< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == SIGNINT)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::SIGNINT< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == SIGNDOUBLE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::SIGNDOUBLE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ABSINT)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ABSINT< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ABSDOUBLE)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ABSDOUBLE< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == FLOOR)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::FLOOR< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == CEIL)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::CEIL< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ROUND)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ROUND< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == LN)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LN< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == LOG10)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::LOG10< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == EXP)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::EXP< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == SIN)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::SIN< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == COS)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::COS< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == TAN)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::TAN< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ARCSIN)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ARCSIN< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ARCCOS)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ARCCOS< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ARCTAN)
    {
      typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputImage,
            itk::Functor::ARCTAN< TArgument> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else
    {
      std::cerr << "Selected functor is not valid!" << std::endl;
      return NULL;
    }
  }
};

#endif //#ifndef __itklogicalFunctors_h
