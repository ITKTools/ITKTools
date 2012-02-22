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
#ifndef __itkNaryUnequalityTestImageFilter_h_
#define __itkNaryUnequalityTestImageFilter_h_

#include "itkNaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{

/** \class NaryUnequalityTestImageFilter
 * \brief Implements an operator for pixel-wise comparison of N images.
 *
 * This class is parametrized over the types of the
 * input images and the type of the output image.
 * Numeric conversions (castings) are done by the C++ defaults.
 *
 * The pixel type of the input image must have a valid definition of
 * the operator==. This condition is required because internally this filter
 * will perform the operation
 *
 *        pixel_from_image_{i} == pixel_from_image_{i+1}
 *
 * The total operation over one pixel will be
 *
 *  output_pixel = static_cast<OutputPixelType>(
 *    ! (input_0_pixel == input1_pixel == input_2_pixel == etc ) )
 *
 * If not all pixels are equal, the result is 1. If all pixels are equal, the resut = 0.
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */

namespace Functor {

template< class TInput, class TOutput >
class UnequalityTest
{
public:
  UnequalityTest() {}
  ~UnequalityTest() {}
  inline TOutput operator()( const std::vector< TInput > & B)
  {
    const TInput & ref = B[0];
    bool allequal = true;
    for( unsigned int i=1; i < B.size(); ++i )
    {
      allequal &= ( B[ i ] == ref );
    }
    return static_cast<TOutput>( !allequal );
  }
  bool operator== (const UnequalityTest&) const
  {
    return true;
  }
  bool operator!= (const UnequalityTest&) const
  {
    return false;
  }
};
}
template <class TInputImage, class TOutputImage>
class NaryUnequalityTestImageFilter : public
  NaryFunctorImageFilter< TInputImage, TOutputImage,
  Functor::UnequalityTest<typename TInputImage::PixelType, typename TOutputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef NaryUnequalityTestImageFilter  Self;
  typedef NaryFunctorImageFilter<
    TInputImage,
    TOutputImage,
    Functor::UnequalityTest<
      typename TInputImage::PixelType, typename TOutputImage::PixelType > >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(BooleanConvertibleToOutputCheck,
    (Concept::Convertible< bool, typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  NaryUnequalityTestImageFilter() {}
  virtual ~NaryUnequalityTestImageFilter() {}

private:
  NaryUnequalityTestImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif // end #ifndef __itkNaryUnequalityTestImageFilter_h_
