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
  Module:    $RCSfile: itkHessianRecursiveGaussianImageFilter2.h,v $
  Language:  C++
  Date:      $Date: 2008-03-14 09:45:07 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHessianRecursiveGaussianImageFilter2_h_
#define __itkHessianRecursiveGaussianImageFilter2_h_

#include "itkRecursiveGaussianImageFilter.h"
#include "itkNthElementImageAdaptor.h"
#include "itkImage.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkPixelTraits.h"
#include "itkCommand.h"
#include "itkFixedArray.h"


namespace itk
{

/** \class HessianRecursiveGaussianImageFilter2
 * \brief Computes the Hessian matrix of an image by convolution
 *        with the Second and Cross derivatives of a Gaussian.
 *
 * This filter is implemented using the recursive gaussian
 * filters
 *
 *
 * \ingroup GradientFilters
 * \ingroup Singlethreaded
 */

template <typename TInputImage,
  typename TOutputImage= Image< SymmetricSecondRankTensor<
  typename NumericTraits< typename TInputImage::PixelType>::RealType,
  ::itk::GetImageDimension<TInputImage>::ImageDimension >,
  ::itk::GetImageDimension<TInputImage>::ImageDimension > >
class ITK_EXPORT HessianRecursiveGaussianImageFilter2:
  public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef HessianRecursiveGaussianImageFilter2            Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>    Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Pixel Type of the input image */
  typedef TInputImage                                     InputImageType;
  typedef typename TInputImage::PixelType                 PixelType;
  typedef typename NumericTraits<PixelType>::RealType     RealType;
  typedef typename NumericTraits<PixelType>
    ::ScalarRealType                                      ScalarRealType;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int,
    GetImageDimension<TInputImage>::ImageDimension );

  /** Number of smoothing filters. */
  itkStaticConstMacro( NumberOfSmoothingFilters, unsigned int,
    GetImageDimension<TInputImage>::ImageDimension - 2 );

  /** Define the image type for internal computations
      RealType is usually 'double' in NumericTraits.
      Here we prefer float in order to save memory.  */

  typedef float                                       InternalRealType;
  typedef Image<
    InternalRealType,
    GetImageDimension<TInputImage>::ImageDimension >  RealImageType;

  /**  Output Image Nth Element Adaptor
   *  This adaptor allows to use conventional scalar
   *  smoothing filters to compute each one of the
   *  components of the gradient image pixels. */
  typedef NthElementImageAdaptor<
    TOutputImage, InternalRealType >                  OutputImageAdaptorType;
  typedef typename OutputImageAdaptorType::Pointer    OutputImageAdaptorPointer;

  /**  Smoothing filter type */
  typedef RecursiveGaussianImageFilter<
    RealImageType, RealImageType >                    GaussianFilterType;

  /**  Derivative filter type, it will be the first in the pipeline  */
  typedef RecursiveGaussianImageFilter<
    InputImageType, RealImageType >                   DerivativeFilterAType;
  typedef typename DerivativeFilterAType::Pointer     DerivativeFilterAPointer;
  typedef RecursiveGaussianImageFilter<
    RealImageType, RealImageType >                    DerivativeFilterBType;
  typedef typename DerivativeFilterBType::Pointer     DerivativeFilterBPointer;

  /**  Pointer to a gaussian filter.  */
  typedef typename GaussianFilterType::Pointer        GaussianFilterPointer;
  typedef std::vector< GaussianFilterPointer >        GaussianFiltersArray;

  /** Type of the output Image */
  typedef TOutputImage                                OutputImageType;
  typedef typename OutputImageType::Pointer           OutputImagePointer;
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef typename PixelTraits<OutputPixelType>
    ::ValueType                                       OutputComponentType;

  /** Set Sigma value. Sigma is measured in the units of image spacing.  */
  /** Set Sigma value. Sigma is measured in the units of image spacing.  */
  typedef FixedArray< ScalarRealType,
    itkGetStaticConstMacro( ImageDimension ) >        SigmaType;
  virtual void SetSigma( const ScalarRealType sigma );
  virtual void SetSigma( const SigmaType sigma );
  itkGetMacro( Sigma, SigmaType );

  /** Define which normalization factor will be used for the Gaussian */
  void SetNormalizeAcrossScale( const bool normalizeInScaleSpace );
  itkGetMacro( NormalizeAcrossScale, bool );

  /** HessianRecursiveGaussianImageFilter2 needs all of the input to produce an
   * output. Therefore, HessianRecursiveGaussianImageFilter2 needs to provide
   * an implementation for GenerateInputRequestedRegion in order to inform
   * the pipeline execution model.
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputHasNumericTraitsCheck,
                  (Concept::HasNumericTraits<PixelType>));
  itkConceptMacro(OutputHasPixelTraitsCheck,
                  (Concept::HasPixelTraits<OutputPixelType>));
  /** End concept checking */
#endif

protected:

  HessianRecursiveGaussianImageFilter2();
  virtual ~HessianRecursiveGaussianImageFilter2() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Generate Data */
  void GenerateData( void );

  // Override since the filter produces the entire dataset
  void EnlargeOutputRequestedRegion(DataObject *output);

private:

  HessianRecursiveGaussianImageFilter2(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  GaussianFiltersArray          m_SmoothingFilters;
  DerivativeFilterAPointer      m_DerivativeFilterA;
  DerivativeFilterBPointer      m_DerivativeFilterB;
  OutputImageAdaptorPointer     m_ImageAdaptor;
  SigmaType                     m_Sigma;

  /** Normalize the image across scale space */
  bool m_NormalizeAcrossScale;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHessianRecursiveGaussianImageFilter2.txx"
#endif

#endif
