#ifndef __itkLogicalFunctors_h
#define __itkLogicalFunctors_h

namespace itk {

namespace Functor {  

/** Macro for unary logical filter. */
#define itkLogicalUnaryFunctorMacro( name, ops ) \
template< class TInput, class TArgument=TInput, class TOutput=TInput > \
class localName##name \
{ \
public: \
  localName##name() {}; \
  ~localName##name() {}; \
  inline TOutput operator()( const TInput & A ) \
  { \
    return static_cast<TOutput>( ops ); \
  } \
  void SetArgument( TArgument arg ){ this->m_Argument = arg; }; \
private: \
  TArgument m_Argument; \
}

/** Macro for binary logical filter. */
#define itkLogicalBinaryFunctorMacro( name, ops ) \
template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 > \
class localName##name \
{ \
public: \
  localName##name() {}; \
  ~localName##name() {}; \
  inline TOutput operator()( const TInput1 & A, const TInput2 & B) \
  { \
    return static_cast<TOutput>( ops ); \
  } \
}


/** Create logical filter definitions. */
itkLogicalUnaryFunctorMacro( NOT,       !A );
itkLogicalUnaryFunctorMacro( EQUAL,      A == this->m_Argument );

itkLogicalBinaryFunctorMacro( AND,       A & B );
itkLogicalBinaryFunctorMacro( OR,        A | B );
itkLogicalBinaryFunctorMacro( XOR,       A ^ B );
itkLogicalBinaryFunctorMacro( NOT_AND, !(A & B) );
itkLogicalBinaryFunctorMacro( NOT_OR , !(A | B) );
itkLogicalBinaryFunctorMacro( NOT_XOR, !(A ^ B) );
itkLogicalBinaryFunctorMacro( ANDNOT,    A & (!B) ); 
itkLogicalBinaryFunctorMacro( ORNOT,     A | (!B) ); 


} // end namespace Functor


} // end namespace itk


#endif //#ifndef __itklogicalFunctors_h
