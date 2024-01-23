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
#ifndef __itkNaryFunctors_h_
#define __itkNaryFunctors_h_

#include "itkNumericTraits.h"

namespace itk {

namespace Functor {


template< class TInput, class TOutput = TInput >
class NaryADDITION
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryADDITION() {};
  ~NaryADDITION() {};
  bool operator!=( const NaryADDITION & ) const{ return false; }
  bool operator==( const NaryADDITION & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = NumericTraits< ScalarRealType >::Zero;
    for( unsigned int i = 0; i < B.size(); i++ )
    {
      result += B[ i ];
    }
    return static_cast< TOutput >( result );
  }
};

template< class TInput, class TOutput = TInput >
class NaryMEAN
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryMEAN() {};
  ~NaryMEAN() {};
  bool operator!=( const NaryMEAN & ) const{ return false; }
  bool operator==( const NaryMEAN & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = NumericTraits< ScalarRealType >::Zero;
    for( unsigned int i = 0; i < B.size(); i++ )
    {
      result += B[ i ];
    }
    return static_cast< TOutput >( result / B.size() );
  }
};

template< class TInput, class TOutput = TInput >
class NaryMINUS
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryMINUS() {};
  ~NaryMINUS() {};
  bool operator!=( const NaryMINUS & ) const{ return false; }
  bool operator==( const NaryMINUS & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = static_cast< ScalarRealType >( B[ 0 ] );
    for( unsigned int i = 1; i < B.size(); i++ )
    {
      result -= B[ i ];
    }
    return static_cast< TOutput >( result );
  }
};


template< class TInput, class TOutput = TInput >
class NaryTIMES
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryTIMES() {};
  ~NaryTIMES() {};
  bool operator!=( const NaryTIMES & ) const{ return false; }
  bool operator==( const NaryTIMES & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = NumericTraits< ScalarRealType >::One;
    for( unsigned int i = 0; i < B.size(); i++ )
    {
      result *= B[ i ];
    }
    return static_cast< TOutput >( result );
  }
};


template< class TInput, class TOutput = TInput >
class NaryDIVIDE
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryDIVIDE() {};
  ~NaryDIVIDE() {};
  bool operator!=( const NaryDIVIDE & ) const{ return false; }
  bool operator==( const NaryDIVIDE & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = static_cast< ScalarRealType >( B[ 0 ] );
    for( unsigned int i = 1; i < B.size(); i++ )
    {
      result = B[ i ] != 0 ? result / B[ i ] : result;
    }
    return static_cast< TOutput >( result );
  }
};

template< class TInput, class TOutput = TInput >
class NaryMAXIMUM
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryMAXIMUM() {};
  ~NaryMAXIMUM() {};
  bool operator!=( const NaryMAXIMUM & ) const{ return false; }
  bool operator==( const NaryMAXIMUM & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    AccumulateType result = static_cast< AccumulateType >( B[ 0 ] );
    for( unsigned int i = 1; i < B.size(); i++ )
    {
      result = std::max( result, B[ i ] );
    }
    return static_cast< TOutput >( result );
  }
};


template< class TInput, class TOutput = TInput >
class NaryMINIMUM
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryMINIMUM() {};
  ~NaryMINIMUM() {};
  bool operator!=( const NaryMINIMUM & ) const{ return false; }
  bool operator==( const NaryMINIMUM & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    AccumulateType result = static_cast< AccumulateType >( B[ 0 ] );
    for( unsigned int i = 1; i < B.size(); i++ )
    {
      result = vnl_math_min( result, B[ i ] );
    }
    return static_cast< TOutput >( result );
  }
};


template< class TInput, class TOutput = TInput >
class NaryABSOLUTEDIFFERENCE
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryABSOLUTEDIFFERENCE() {};
  ~NaryABSOLUTEDIFFERENCE() {};
  bool operator!=( const NaryABSOLUTEDIFFERENCE & ) const{ return false; }
  bool operator==( const NaryABSOLUTEDIFFERENCE & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = static_cast< ScalarRealType >( B[ 0 ] );
    for( unsigned int i = 1; i < B.size(); i++ )
    {
      result -= B[ i ];
    }
    return static_cast< TOutput >( result > 0.0 ? result : -result );
  }
};


template< class TInput, class TOutput = TInput >
class NaryNARYMAGNITUDE
{
public:
  typedef typename NumericTraits< TInput >::AccumulateType AccumulateType;
  typedef typename NumericTraits< TInput >::ScalarRealType ScalarRealType;
  NaryNARYMAGNITUDE() {};
  ~NaryNARYMAGNITUDE() {};
  bool operator!=( const NaryNARYMAGNITUDE & ) const{ return false; }
  bool operator==( const NaryNARYMAGNITUDE & other ) const{ return !(*this != other); }
  inline TOutput operator()( const std::vector< TInput > & B ) const
  {
    ScalarRealType result = NumericTraits< ScalarRealType >::Zero;
    for( unsigned int i = 0; i < B.size(); i++ )
    {
      result += B[ i ] * B[ i ];
    }
    return static_cast< TOutput >( std::sqrt( result ) );
  }
};

} // end namespace Functor


} // end namespace itk


#endif // end #ifndef __itkNaryFunctors_h_
