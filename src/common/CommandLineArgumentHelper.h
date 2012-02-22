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
#ifndef __CommandLineArgumentHelper_h_
#define __CommandLineArgumentHelper_h_

#include <vector>
#include "itkImageBase.h"


/** Convert std::vector to itk types. */
template<unsigned int Dimension>
void ConvertImageInformationToITKTypes(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::SizeType      & sizeITK,
  typename itk::ImageBase<Dimension>::SpacingType   & spacingITK,
  typename itk::ImageBase<Dimension>::PointType     & originITK,
  typename itk::ImageBase<Dimension>::DirectionType & directionITK );

/** Convert std::vector to an itk::Size type. */
template< unsigned int Dimension>
void ConvertVectorToSize(
  const std::vector<unsigned int> & size,
  typename itk::ImageBase<Dimension>::SizeType sizeITK );

/** Convert std::vector to an ITK spacing type. */
template< unsigned int Dimension>
void ConvertVectorToSpacing(
  const std::vector<double> & spacing,
  typename itk::ImageBase<Dimension>::SpacingType spacingITK );

/** Convert std::vector to an ITK point type. */
template< unsigned int Dimension>
void ConvertVectorToOrigin(
  const std::vector<double> & origin,
  typename itk::ImageBase<Dimension>::PointType originITK );

/** Convert std::vector to an ITK direction type. */
template< unsigned int Dimension>
void ConvertVectorToDirection(
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::DirectionType directionITK );

#include "CommandLineArgumentHelper.hxx"

#endif // end #ifndef __CommandLineArgumentHelper_h_
