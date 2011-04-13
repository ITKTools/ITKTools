#ifndef __itkNaryFunctors_h
#define __itkNaryFunctors_h

#include "itkNumericTraits.h"

namespace itk {

namespace Functor {

/** A macro to write a n-ary functor class . */
#define itkNaryFunctorMacroA( name, iniType, iniValue, start, operation, postprocess ) \
template< class TInput, class TOutput = TInput > \
class Nary##name \
{ \
public: \
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType; \
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType; \
  Nary##name() {}; \
  ~Nary##name() {}; \
  bool operator!=( const Nary##name & ) const{ return false; } \
  bool operator==( const Nary##name & other ) const{ return !(*this != other); } \
  inline TOutput operator()( const std::vector< TInput > & B ) const \
  { \
    iniType result = iniValue; \
    for ( unsigned int i = start; i < B.size(); i++ ) \
    { \
      result operation; \
    } \
    return static_cast< TOutput >( postprocess ); \
  } \
}

/** A macro to write a n-ary functor class, using an input argument. */
// #define itkNaryFunctorMacroB( name, initialize, start, operation, postprocess ) \
// template< class TInput, class TOutput = TInput > \
// class Nary##name \
// { \
// public: \
//   typedef typename NumericTraits< TInput >::AccumulateType AccumulateType; \
//   typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType; \
//   Nary##name() {}; \
//   ~Nary##name() {}; \
//   bool operator!=( const Nary##name & ) const{ return false; } \
//   bool operator==( const Nary##name & other ) const{ return !(*this != other); } \
//   inline TOutput operator()( const std::vector< TInput > & B ) const \
//   { \
//     iniType result = NumericTraits< iniType >::iniValue; \
//     for ( unsigned int i = start; i < B.size(); i++ ) \
//     { \
//       result operation; \
//     } \
//     return static_cast< TOutput >( postprocess ); \
//   } \
//   void SetArgument( double arg ){ this->m_Argument = arg; }; \
// private: \
//   double m_Argument; \
// }

/** Arithmetic functors. */
itkNaryFunctorMacroA( ADDITION, ScalarRealType, NumericTraits< ScalarRealType >::Zero,
                     0, += B[ i ], result );
itkNaryFunctorMacroA( MEAN, ScalarRealType, NumericTraits< ScalarRealType >::Zero,
                     0, += B[ i ], result / B.size() );
itkNaryFunctorMacroA( MINUS,    ScalarRealType, static_cast< ScalarRealType >( B[ 0 ] ),
                     1, -= B[ i ], result );
itkNaryFunctorMacroA( TIMES,    ScalarRealType, NumericTraits< ScalarRealType >::One,
                     0, *= B[ i ], result );
itkNaryFunctorMacroA( DIVIDE,   ScalarRealType, static_cast< ScalarRealType >( B[ 0 ] ),
                     1,  = B[ i ] != 0 ? result / B[ i ] : result, result );
//itkNaryFunctorMacroA( POWER, vcl_pow( A, B ) );
//itkNaryFunctorMacroB( WEIGHTEDADDITION, this->m_Argument * A + ( 1.0 - this->m_Argument ) * B );

itkNaryFunctorMacroA( MAXIMUM,  AccumulateType, static_cast< AccumulateType >( B[ 0 ] ),
                     1,  = vnl_math_max( result, B[ i ] ), result );
itkNaryFunctorMacroA( MINIMUM,  AccumulateType, static_cast< AccumulateType >( B[ 0 ] ),
                     1,  = vnl_math_min( result, B[ i ] ), result );

itkNaryFunctorMacroA( ABSOLUTEDIFFERENCE, ScalarRealType, static_cast< ScalarRealType >( B[ 0 ] ),
                     1, -= B[ i ], result > 0.0 ? result : -result );
itkNaryFunctorMacroA( NARYMAGNITUDE,      ScalarRealType, NumericTraits< ScalarRealType >::Zero,
                     0, += B[ i ] * B[ i ], vcl_sqrt( result ) );
//itkNaryFunctorMacro( SQUAREDDIFFERENCE, ( A - B ) * ( A - B ) );

/** Compose and join functors. */

/** Mask functors. */
// itkNaryFunctorMacroA( MASK,
//   B != NumericTraits< TInput1 >::Zero ? A : this->m_Argument );
// itkNaryFunctorMacroA( MASKNEGATED,
//   B != NumericTraits< TInput1 >::Zero ? this->m_Argument : A );

/** Nonsense functors. */
//itkNaryFunctorMacro( LOG, /= vcl_log( B[ i ] ) );

} // end namespace Functor


} // end namespace itk


#endif // end #ifndef __itkNaryFunctors_h
