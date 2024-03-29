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
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSmoothingRecursiveGaussianImageFilter2.h,v $
  Language:  C++
  Date:      $Date: 2007-05-10 09:37:46 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSmoothingRecursiveGaussianImageFilter2_h_
#define __itkSmoothingRecursiveGaussianImageFilter2_h_

#include "itkRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkPixelTraits.h"
#include "itkCommand.h"
#include "itkFixedArray.h"


namespace itk
{

/** \class SmoothingRecursiveGaussianImageFilter2
 * \brief Computes the smoothing of an image by convolution
 *        with the Gaussian kernels implemented as IIR filters.
 *
 * This filter is implemented using the recursive gaussian
 * filters. For multi-component images, the filter works on each
 * component independently.
 *
 *
 * \ingroup IntensityImageFilters
 * \ingroup Singlethreaded
 */
template <typename TInputImage,
          typename TOutputImage= TInputImage >
class ITK_EXPORT SmoothingRecursiveGaussianImageFilter2:
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SmoothingRecursiveGaussianImageFilter2            Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>      Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;


  /** Pixel Type of the input image */
  typedef TInputImage                                       InputImageType;
  typedef TOutputImage                                      OutputImageType;
  typedef typename TInputImage::PixelType                   PixelType;
  typedef typename NumericTraits<PixelType>::RealType       RealType;
  typedef typename NumericTraits<PixelType>::ScalarRealType ScalarRealType;

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Define the image type for internal computations
      RealType is usually 'double' in NumericTraits.
      Here we prefer float in order to save memory.  */
  typedef typename NumericTraits< PixelType >::FloatType    InternalRealType;
  typedef Image<InternalRealType,
    itkGetStaticConstMacro(ImageDimension) >      RealImageType;

  /**  The first in the pipeline  */
  typedef RecursiveGaussianImageFilter<
    InputImageType, RealImageType >               FirstGaussianFilterType;

  /**  Smoothing filter type */
  typedef RecursiveGaussianImageFilter<
    RealImageType, RealImageType >                InternalGaussianFilterType;

  /**  The last in the pipeline  */
  typedef CastImageFilter<
    RealImageType, OutputImageType >              CastingFilterType;

  /**  Pointer to a gaussian filter.  */
  typedef typename InternalGaussianFilterType::Pointer    InternalGaussianFilterPointer;

  /**  Pointer to the first gaussian filter.  */
  typedef typename FirstGaussianFilterType::Pointer       FirstGaussianFilterPointer;

  /**  Pointer to the last filter, casting  */
  typedef typename CastingFilterType::Pointer             CastingFilterPointer;

  /**  Pointer to the Output Image */
  typedef typename OutputImageType::Pointer               OutputImagePointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Set Sigma value. Sigma is measured in the units of image spacing.  */
  typedef FixedArray< ScalarRealType,
    itkGetStaticConstMacro(ImageDimension) >              SigmaType;
  virtual void SetSigma( const ScalarRealType sigma );
  virtual void SetSigma( const SigmaType sigma );
  itkGetMacro( Sigma, SigmaType );

  /** Define which normalization factor will be used for the Gaussian. */
  virtual void SetNormalizeAcrossScale( const bool arg );
  itkGetMacro( NormalizeAcrossScale, bool );

  /** Set/Get the Order of the Gaussian to convolve with.
      \li ZeroOrder is equivalent to convolving with a Gaussian.  This
      is the default.
      \li FirstOrder is equivalent to convolving with the first derivative of a Gaussian.
      \li SecondOrder is equivalent to convolving with the second derivative of a Gaussian.
    */
  typedef FixedArray< unsigned int,
    itkGetStaticConstMacro(ImageDimension) >              OrderType;
  virtual void SetOrder( const unsigned int order );
  virtual void SetOrder( const OrderType order );
  itkGetMacro( Order, OrderType );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( InputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<PixelType>) );
  /** End concept checking */
#endif

protected:
  SmoothingRecursiveGaussianImageFilter2();
  virtual ~SmoothingRecursiveGaussianImageFilter2() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Generate Data */
  void GenerateData( void );

  /** SmoothingRecursiveGaussianImageFilter needs all of the input to produce an
   * output. Therefore, SmoothingRecursiveGaussianImageFilter needs to provide
   * an implementation for GenerateInputRequestedRegion in order to inform
   * the pipeline execution model.
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw( InvalidRequestedRegionError );

  // Override since the filter produces the entire dataset
  void EnlargeOutputRequestedRegion( DataObject *output );

private:
  SmoothingRecursiveGaussianImageFilter2(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InternalGaussianFilterPointer         m_SmoothingFilters[ImageDimension-1];
  FirstGaussianFilterPointer            m_FirstSmoothingFilter;
  CastingFilterPointer                  m_CastingFilter;

  /** Normalize the image across scale space */
  bool m_NormalizeAcrossScale;

  OrderType m_Order;
  SigmaType m_Sigma;

}; // end class SmoothingRecursiveGaussianImageFilter2

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSmoothingRecursiveGaussianImageFilter2.txx"
#endif

#endif
