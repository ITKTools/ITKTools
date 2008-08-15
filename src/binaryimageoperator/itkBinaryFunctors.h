#ifndef __itkBinaryFunctors_h
#define __itkBinaryFunctors_h

#include "itkNumericTraits.h"

namespace itk {

namespace Functor {

/** A macro to write a binary functor class, using constraints. */
#define itkBinaryFunctorMacroC( name, operation ) \
template< class TInput1, class TInput2, class TOutput = TInput1 > \
class name \
{ \
public: \
  name() {}; \
  ~name() {}; \
  bool operator!=( const name & ) const{ return false; } \
  bool operator==( const name & other ) const{ return !(*this != other); } \
  inline TOutput operator()( const TInput1 & a, const TInput2 & b ) \
  {\
    const double A = static_cast<double>( a ); \
    const double B = static_cast<double>( b ); \
    const double result1 = operation; \
    const double result2 = ( result1 < NumericTraits<TOutput>::max() ) ? result1 : NumericTraits<TOutput>::max(); \
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() ) ? result2 : NumericTraits<TOutput>::NonpositiveMin(); \
    return static_cast<TOutput>( result3 ); \
  } \
}

/** A macro to write a binary functor class, no constraints. */
#define itkBinaryFunctorMacro( name, operation ) \
template< class TInput1, class TInput2, class TOutput = TInput1 > \
class name \
{ \
public: \
  name() {}; \
  ~name() {}; \
  bool operator!=( const name & ) const{ return false; } \
  bool operator==( const name & other ) const{ return !(*this != other); } \
  inline TOutput operator()( const TInput1 & a, const TInput2 & b ) \
  {\
    const double A = static_cast<double>( a ); \
    const double B = static_cast<double>( b ); \
    return static_cast<TOutput>( operation ); \
  } \
}

/** A macro to write a binary functor class, using constraints and an input argument. */
#define itkBinaryFunctorMacroCA( name, operation ) \
template< class TInput1, class TInput2, class TOutput = TInput1 > \
class name \
{ \
public: \
  name() {}; \
  ~name() {}; \
  bool operator!=( const name & ) const{ return false; } \
  bool operator==( const name & other ) const{ return !(*this != other); } \
  inline TOutput operator()( const TInput1 & A, const TInput2 & B ) \
  {\
    const double result1 = operation; \
    const double result2 = ( result1 < NumericTraits<TOutput>::max() ) ? result1 : NumericTraits<TOutput>::max(); \
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() ) ? result2 : NumericTraits<TOutput>::NonpositiveMin(); \
    return static_cast<TOutput>( result3 ); \
  } \
  void SetArgument( double arg ){ this->m_Argument = arg; }; \
private: \
  double m_Argument; \
}

/** A macro to write a binary functor class, using an input argument. */
#define itkBinaryFunctorMacroA( name, operation ) \
template< class TInput1, class TInput2, class TOutput = TInput1 > \
class name \
{ \
public: \
  name() {}; \
  ~name() {}; \
  bool operator!=( const name & ) const{ return false; } \
  bool operator==( const name & other ) const{ return !(*this != other); } \
  inline TOutput operator()( const TInput1 & A, const TInput2 & B ) \
  {\
    return static_cast<TOutput>( operation ); \
  } \
  void SetArgument( double arg ){ this->m_Argument = arg; }; \
private: \
  double m_Argument; \
}

/** Arithmetic functors. */
itkBinaryFunctorMacroC( ADDITION, A + B );
itkBinaryFunctorMacroCA( WEIGHTEDADDITION, this->m_Argument * A + ( 1.0 - this->m_Argument ) * B );
itkBinaryFunctorMacroC( MINUS, A - B );
itkBinaryFunctorMacroC( TIMES, A * B );
itkBinaryFunctorMacro( DIVIDE, B != 0 ? A / B : NumericTraits<TOutput>::max() );
itkBinaryFunctorMacroC( POWER, vcl_pow( A, B ) );

itkBinaryFunctorMacroC( MAXIMUM, vnl_math_max( A, B ) );
itkBinaryFunctorMacroC( MINIMUM, vnl_math_min( A, B ) );

itkBinaryFunctorMacro( ABSOLUTEDIFFERENCE, A - B > 0.0 ? A - B : B - A );
itkBinaryFunctorMacro( SQUAREDDIFFERENCE, ( A - B ) * ( A - B ) );
itkBinaryFunctorMacro( BINARYMAGNITUDE, vcl_sqrt( A * A + B * B ) );

/** Compose and join functors. */

/** Mask functors. */
itkBinaryFunctorMacroA( MASK,
  B != NumericTraits< TInput1 >::Zero ? A : this->m_Argument );
itkBinaryFunctorMacroA( MASKNEGATED,
  B != NumericTraits< TInput1 >::Zero ? this->m_Argument : A );

/** Nonsense functors. */
//itkBinaryFunctorMacro( MODULO, A % B );
// log_B( A )
itkBinaryFunctorMacro( LOG, vcl_log( A ) / vcl_log( B ) );
// sqrt_B( A )
//?itkBinaryFunctorMacroC( ROOT, vcl_( A ) / vcl_log( B );


} // end namespace Functor


} // end namespace itk


#endif // end #ifndef __itkBinaryFunctors_h
