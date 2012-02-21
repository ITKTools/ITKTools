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
#ifndef __CommandLineArgumentHelper_hxx_
#define __CommandLineArgumentHelper_hxx_

/**
 * *************** ConvertImageInformationToITKTypes ***********************
 */

template<unsigned int Dimension>
void ConvertImageInformationToITKTypes(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::SizeType      & sizeITK,
  typename itk::ImageBase<Dimension>::SpacingType   & spacingITK,
  typename itk::ImageBase<Dimension>::PointType     & originITK,
  typename itk::ImageBase<Dimension>::DirectionType & directionITK )
{
  typedef itk::ImageBase<Dimension> ImageBaseType;
  typedef typename ImageBaseType::SizeValueType      SizeValueType;

  for( unsigned int i = 0; i < Dimension; i++ )
  {
    sizeITK[ i ] = static_cast<SizeValueType>( size[ i ] );
    spacingITK[ i ] = spacing[ i ];
    originITK[ i ] = origin[ i ];
  }

  for( unsigned int i = 0; i < Dimension; i++ )
  {
    for( unsigned int j = 0; j < Dimension; j++ )
    {
      directionITK[ i ][ j ] = direction[ j + i * Dimension ];
    }
  }

} // end ConvertImageInformationToITKTypes()


/**
 * *************** ConvertVectorToSize ***********************
 */

template< unsigned int Dimension>
void ConvertVectorToSize(
  const std::vector<unsigned int> & size,
  typename itk::ImageBase<Dimension>::SizeType sizeITK )
{
  typedef itk::ImageBase<Dimension> ImageBaseType;
  typedef typename ImageBaseType::SizeValueType SizeValueType;
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    sizeITK[ i ] = static_cast<SizeValueType>( size[ i ] );
  }
} // end ConvertVectorToSize()


/**
 * *************** ConvertVectorToSpacing ***********************
 */

template< unsigned int Dimension>
void ConvertVectorToSpacing(
  const std::vector<double> & spacing,
  typename itk::ImageBase<Dimension>::SpacingType spacingITK )
{
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    spacingITK[ i ] = spacing[ i ];
  }
} // end ConvertVectorToSpacing()


/**
 * *************** ConvertVectorToOrigin ***********************
 */

template< unsigned int Dimension>
void ConvertVectorToOrigin(
  const std::vector<double> & origin,
  typename itk::ImageBase<Dimension>::PointType originITK )
{
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    originITK[ i ] = origin[ i ];
  }
} // end ConvertVectorToOrigin()


/**
 * ***************** ConvertVectorToDirection ************************
 */

template< unsigned int Dimension>
void ConvertVectorToDirection(
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::DirectionType directionITK )
{
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    for( unsigned int j = 0; j < Dimension; j++ )
    {
      directionITK[ i ][ j ] = direction[ j + i * Dimension ];
    }
  }
} // end ConvertVectorToDirection()

#endif // end #ifndef __CommandLineArgumentHelper_hxx_
