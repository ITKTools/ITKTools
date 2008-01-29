#ifndef __itkUnaryFunctors_h
#define __itkUnaryFunctors_h

namespace itk {

namespace Functor {  

/** A Macro to write a unary functor class with SetArgument(). */
#define itkUnaryFunctorMacro(name,ops)\
template< class TInput, class TArgument=TInput, class TOutput=TInput >\
class name\
{\
public:\
  name() {};\
  ~name() {};\
  inline TOutput operator()( const TInput & A )\
  {\
    return static_cast<TOutput>( ops );\
  }\
	void SetArgument( TArgument arg ){ this->m_Argument = arg; };\
private:\
	TArgument m_Argument;\
}

/** Arithmetic functors which use m_Argument. */
itkUnaryFunctorMacro( PLUS,       A + this->m_Argument );
itkUnaryFunctorMacro( RMINUS,     A - this->m_Argument );//==A+(-Arg)
itkUnaryFunctorMacro( LMINUS,     this->m_Argument - A );
itkUnaryFunctorMacro( TIMES,      A * this->m_Argument );
itkUnaryFunctorMacro( RDIVIDE,    A / this->m_Argument );
itkUnaryFunctorMacro( LDIVIDE,    this->m_Argument / A );
itkUnaryFunctorMacro( RPOWER,     vcl_pow( static_cast<double>( A ), static_cast<double>( this->m_Argument ) ) );
itkUnaryFunctorMacro( LPOWER,     vcl_pow( static_cast<double>( this->m_Argument ), static_cast<double>( A ) ) );
itkUnaryFunctorMacro( RMODINT,    static_cast<int>( A ) % static_cast<int>( this->m_Argument ) );
itkUnaryFunctorMacro( RMODDOUBLE, vcl_fmod( static_cast<double>( A ), static_cast<double>( this->m_Argument ) ) );
itkUnaryFunctorMacro( LMODINT,    static_cast<int>( this->m_Argument ) % static_cast<int>( A ) );
itkUnaryFunctorMacro( LMODDOUBLE, vcl_fmod( static_cast<double>( this->m_Argument ), static_cast<double>( A ) ) );

itkUnaryFunctorMacro( EQUAL,      A == this->m_Argument );

/** Funtions that don't use m_Argument. */
itkUnaryFunctorMacro( NEG,        -A );//==RMINUS 0-A
itkUnaryFunctorMacro( SIGNINT,    vnl_math_sgn( A ) );
itkUnaryFunctorMacro( SIGNDOUBLE, vnl_math_sgn( A ) );
itkUnaryFunctorMacro( ABSINT,     vcl_abs( static_cast<int>( A ) ) );
itkUnaryFunctorMacro( ABSDOUBLE,  vcl_abs( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( FLOOR,      vcl_floor( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( CEIL,       vcl_ceil( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( ROUND,      vnl_math_rnd( static_cast<double>( A ) ) );

itkUnaryFunctorMacro( SQR,    vcl_sqr( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( SQRT,   vcl_sqrt( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( LN,     vcl_log( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( LOG10,  vcl_log10( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( NLOG,   vcl_log( static_cast<double>( A ) ) / vcl_log( static_cast<double>( this->m_Argument ) ) );
itkUnaryFunctorMacro( EXP,    vcl_exp( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( SIN,    vcl_sin( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( COS,    vcl_cos( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( TAN,    vcl_tan( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( ARCSIN, vcl_asin( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( ARCCOS, vcl_acos( static_cast<double>( A ) ) );
itkUnaryFunctorMacro( ARCTAN, vcl_atan( static_cast<double>( A ) ) );

} // end namespace Functor


} // end namespace itk


#endif //#ifndef __itklogicalFunctors_h
