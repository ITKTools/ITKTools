#ifndef __itkLogicalFunctors_h
#define __itkLogicalFunctors_h

namespace itk {

namespace Functor {  
  
#define itkLogicalFunctorMacro(name,ops)\
template< class TInput1, class TInput2=TInput1, class TOutput=TInput1 >\
class name\
{\
public:\
  name() {};\
  ~name() {};\
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)\
  {\
    return static_cast<TOutput>( ops );\
  }\
}

itkLogicalFunctorMacro( AND, A & B );
itkLogicalFunctorMacro(  OR, A | B );
itkLogicalFunctorMacro( XOR, A ^ B );
itkLogicalFunctorMacro( NOT_AND, !(A & B) );
itkLogicalFunctorMacro( NOT_OR , !(A | B) );
itkLogicalFunctorMacro( NOT_XOR, !(A ^ B) );
itkLogicalFunctorMacro( ANDNOT, A & (!B) ); 
itkLogicalFunctorMacro(  ORNOT, A | (!B) ); 


} // end namespace Functor


} // end namespace itk


#endif //#ifndef __itklogicalFunctors_h
