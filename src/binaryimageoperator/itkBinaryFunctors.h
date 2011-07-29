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
#ifndef __itkBinaryFunctors_h
#define __itkBinaryFunctors_h

#include "itkNumericTraits.h"

namespace itk {
namespace Functor {


/**
 * ***************** ADDITION ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class ADDITION
{
public:
  ADDITION() {};
  ~ADDITION() {};

  bool operator!=( const ADDITION & ) const
  { return false; }
  bool operator==( const ADDITION & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = A + B;
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class ADDITION


/**
 * ***************** WEIGHTEDADDITION ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class WEIGHTEDADDITION
{
public:
  WEIGHTEDADDITION() {};
  ~WEIGHTEDADDITION() {};

  bool operator!=( const WEIGHTEDADDITION & ) const
  { return false; }
  bool operator==( const WEIGHTEDADDITION & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & A, const TInput2 & B )
  {
    const double result1 = this->m_Argument * A + ( 1.0 - this->m_Argument ) * B;
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }

  void SetArgument( double arg )
  { this->m_Argument = arg; };

private:
  double m_Argument;
}; // end class WEIGHTEDADDITION


/**
 * ***************** MINUS ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class MINUS
{
public:
  MINUS() {};
  ~MINUS() {};

  bool operator!=( const MINUS & ) const
  { return false; }
  bool operator==( const MINUS & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = A - B;
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class MINUS


/**
 * ***************** TIMES ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class TIMES
{
public:
  TIMES() {};
  ~TIMES() {};

  bool operator!=( const TIMES & ) const
  { return false; }
  bool operator==( const TIMES & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = A * B;
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class TIMES


/**
 * ***************** DIVIDE ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class DIVIDE
{
public:
  DIVIDE() {};
  ~DIVIDE() {};

  bool operator!=( const DIVIDE & ) const
  { return false; }
  bool operator==( const DIVIDE & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    return static_cast<TOutput>( B != 0 ? A / B : NumericTraits<TOutput>::max() );
  }
}; // end class DIVIDE


/**
 * ***************** POWER ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class POWER
{
public:
  POWER() {};
  ~POWER() {};

  bool operator!=( const POWER & ) const
  { return false; }
  bool operator==( const POWER & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = vcl_pow( A, B );
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class POWER


/**
 * ***************** MAXIMUM ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class MAXIMUM
{
public:
  MAXIMUM() {};
  ~MAXIMUM() {};

  bool operator!=( const MAXIMUM & ) const
  { return false; }
  bool operator==( const MAXIMUM & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = vnl_math_max( A, B );
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class MAXIMUM


/**
 * ***************** MINIMUM ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class MINIMUM
{
public:
  MINIMUM() {};
  ~MINIMUM() {};

  bool operator!=( const MINIMUM & ) const
  { return false; }
  bool operator==( const MINIMUM & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    const double result1 = vnl_math_min( A, B );
    const double result2 = ( result1 < NumericTraits<TOutput>::max() )
      ? result1 : NumericTraits<TOutput>::max();
    const double result3 = ( result2 > NumericTraits<TOutput>::NonpositiveMin() )
      ? result2 : NumericTraits<TOutput>::NonpositiveMin();
    return static_cast<TOutput>( result3 );
  }
}; // end class MINIMUM


/**
 * ***************** ABSOLUTEDIFFERENCE ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class ABSOLUTEDIFFERENCE
{
public:
  ABSOLUTEDIFFERENCE() {};
  ~ABSOLUTEDIFFERENCE() {};

  bool operator!=( const ABSOLUTEDIFFERENCE & ) const
  { return false; }
  bool operator==( const ABSOLUTEDIFFERENCE & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    return static_cast<TOutput>( A - B > 0.0 ? A - B : B - A );
  }
}; // end class ABSOLUTEDIFFERENCE


/**
 * ***************** SQUAREDDIFFERENCE ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class SQUAREDDIFFERENCE
{
public:
  SQUAREDDIFFERENCE() {};
  ~SQUAREDDIFFERENCE() {};

  bool operator!=( const SQUAREDDIFFERENCE & ) const
  { return false; }
  bool operator==( const SQUAREDDIFFERENCE & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    return static_cast<TOutput>( ( A - B ) * ( A - B ) );
  }
}; // end class SQUAREDDIFFERENCE


/**
 * ***************** BINARYMAGNITUDE ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class BINARYMAGNITUDE
{
public:
  BINARYMAGNITUDE() {};
  ~BINARYMAGNITUDE() {};

  bool operator!=( const BINARYMAGNITUDE & ) const
  { return false; }
  bool operator==( const BINARYMAGNITUDE & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    return static_cast<TOutput>( vcl_sqrt( A * A + B * B ) );
  }
}; // end class BINARYMAGNITUDE


/**
 * ***************** MASK ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class MASK
{
public:
  MASK() {};
  ~MASK() {};

  bool operator!=( const MASK & ) const
  { return false; }
  bool operator==( const MASK & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & A, const TInput2 & B )
  {
    return static_cast<TOutput>( B != NumericTraits< TInput1 >::Zero ? A : this->m_Argument );
  }
  void SetArgument( double arg )
  { this->m_Argument = arg; };
private:
  double m_Argument;
}; // end class MASK


/**
 * ***************** MASKNEGATED ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class MASKNEGATED
{
public:
  MASKNEGATED() {};
  ~MASKNEGATED() {};

  bool operator!=( const MASKNEGATED & ) const
  { return false; }
  bool operator==( const MASKNEGATED & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & A, const TInput2 & B )
  {
    return static_cast<TOutput>( B != NumericTraits< TInput1 >::Zero ? this->m_Argument : A );
  }
  void SetArgument( double arg )
  { this->m_Argument = arg; };
private:
  double m_Argument;
}; // end class MASKNEGATED


/**
 * ***************** LOG ************************
 */

template< class TInput1, class TInput2, class TOutput = TInput1 >
class LOG
{
public:
  LOG() {};
  ~LOG() {};

  bool operator!=( const LOG & ) const
  { return false; }
  bool operator==( const LOG & other ) const
  { return !(*this != other); }

  inline TOutput operator()( const TInput1 & a, const TInput2 & b )
  {
    const double A = static_cast<double>( a );
    const double B = static_cast<double>( b );
    return static_cast<TOutput>( vcl_log( A ) / vcl_log( B ) );
  }
}; // end class LOG


} // end namespace Functor
} // end namespace itk

#endif // end #ifndef __itkBinaryFunctors_h
