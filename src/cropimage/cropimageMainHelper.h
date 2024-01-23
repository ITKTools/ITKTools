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
#ifndef __cropimageMainHelper_h_
#define __cropimageMainHelper_h_

#include <vector>


/*
 * ******************* CheckWhichInputOption *******************
 *
 * 1: supply two points with -pA and -pB
 * 2: supply a points and a size with -pA and -sz
 * 3: supply a lower and an upper bound with -lb and -ub
 */

bool CheckWhichInputOption(
  const bool pAGiven, const bool pBGiven, const bool szGiven,
  const bool lbGiven, const bool ubGiven, unsigned int & arg )
{
  if( pAGiven && pBGiven && !szGiven && !lbGiven && !ubGiven )
  {
    /** Two points given. */
    arg = 1;
    return true;
  }
  else if( pAGiven && !pBGiven && szGiven && !lbGiven && !ubGiven )
  {
    /** A point and a size given. */
    arg = 2;
    return true;
  }
  else if( !pAGiven && !pBGiven && !szGiven && lbGiven && ubGiven )
  {
    /** A lower and an upper bound given. */
    arg = 3;
    return true;
  }

  /** Return a value. */
  return false;

} // end CheckWhichInputOption()


/*
 * ******************* ProcessArgument *******************
 */

bool ProcessArgument( std::vector<int> & arg, const unsigned int dimension, const bool positive )
{
  /** Check if arg is of the right size. */
  if( arg.size() != dimension && arg.size() != 1 )
  {
    return false;
  }

  /** Create a vector arg2 of size dimension, with values:
   * - ( arg[0], ..., arg[0] ) if arg.size() == 1
   * - ( arg[0], ..., arg[dimension-1] ) if arg.size() == dimension
   */
  std::vector<int> arg2( dimension, arg[ 0 ] );
  if( arg.size() == dimension )
  {
    for( unsigned int i = 1; i < dimension; i++ )
    {
      arg2[ i ] = arg[ i ];
    }
  }

  /** Substitute arg2 for arg. */
  arg = arg2;

  /** Check for positive numbers. */
  if( !positive )
  {
    for( unsigned int i = 0; i < dimension; i++ )
    {
      if( arg[ i ] < 0 ) return false;
    }
  }

  /** Return a value. */
  return true;

} // end ProcessArgument()


/*
 * ******************* GetBox *******************
 */

void GetBox( std::vector<int> & pA, std::vector<int> & pB, unsigned int dimension )
{
  /** Get the outer points of the box. */
  std::vector<int> pa( dimension, 0 );
  std::vector<int> pb( dimension, 0 );
  for( unsigned int i = 0; i < dimension; i++ )
  {
    pa[ i ] = vnl_math_min( pA[ i ], pB[ i ] );
    pb[ i ] = std::max( pA[ i ], pB[ i ] );
  }

  /** Copy to the input variables. */
  pA = pa; pB = pb;

} // end GetBox()


/*
 * ******************* GetLowerBoundary *******************
 */

std::vector<int> GetLowerBoundary( const std::vector<int> & input1,
   const unsigned int dimension, const bool force, std::vector<unsigned long> & padLowerBound )
{
  /** Create output vector. */
  std::vector<int> lowerBoundary( input1 );
  padLowerBound.resize( dimension, 0 );
  if( !force ) return lowerBoundary;

  /** Fill output vector. */
  for( unsigned int i = 0; i < dimension; i++ )
  {
    if( input1[ i ] < 0 )
    {
      lowerBoundary[ i ] = 0;
      padLowerBound[ i ] = -input1[ i ];
    }
  }

  /** Return output. */
  return lowerBoundary;

} // end GetLowerBoundary()


/*
 * ******************* GetUpperBoundary *******************
 */

 std::vector<int> GetUpperBoundary( const std::vector<int> & input1,
   const std::vector<int> & input2, const std::vector<unsigned int> & imageSize,
   const unsigned int dimension, const unsigned int option,
   const bool force, std::vector<unsigned long> & padUpperBound )
{
  /** Create output vector. */
  std::vector<int> upperBoundary( dimension, 0 );
  padUpperBound.resize( dimension, 0 );

  /** Fill output vector. */
  if( option == 1 )
  {
    for( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = imageSize[ i ] - input2[ i ];
      if( imageSize[ i ] < input2[ i ] )
      {
        if( force )
        {
          upperBoundary[ i ] = 0;
          padUpperBound[ i ] = input2[ i ] - imageSize[ i ];
        }
        else
        {
          itkGenericExceptionMacro( << "out of bounds." );
        }
      }
      if( input1[ i ] == input2[ i ] )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  }
  else if( option == 2 )
  {
    for( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = imageSize[ i ] - input1[ i ] - input2[ i ];
      if( imageSize[ i ] < input1[ i ] + input2[ i ] )
      {
        if( force )
        {
          upperBoundary[ i ] = 0;
          padUpperBound[ i ] = input1[ i ] + input2[ i ] - imageSize[ i ];
        }
        else
        {
          itkGenericExceptionMacro( << "out of bounds." );
        }
      }
      if( input2[ i ] == 0 )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  }
  else if( option == 3 )
  {
    for( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = input2[ i ];
      if( input2[ i ] < 0 )
      {
        upperBoundary[ i ] = 0;
        padUpperBound[ i ] = -input2[ i ];
      }
      if( imageSize[ i ] < input1[ i ] + input2[ i ] ) // crossing
      {
        itkGenericExceptionMacro( << "out of bounds." );
      }
      if( input1[ i ] + input2[ i ] == imageSize[ i ] )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  } // end if

  /** Return output. */
  return upperBoundary;

} // end GetUpperBoundary()

#endif // end #ifndef __cropimageMainHelper_h_
