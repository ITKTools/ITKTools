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
#ifndef __itkHistogramEqualizationImageFilter_h_
#define __itkHistogramEqualizationImageFilter_h_

#include "itkImageToImageFilter.h"
#include "itkArray.h"


namespace itk
{

/** \class HistogramEqualizationImageFilter
 * \brief Shift and scale the pixels in an image.
 *
 * HistogramEqualizationImageFilter applies a classic histogram equalization.
 * In contrast to the AdaptiveHistogramEqualizationImageFilter it is not adaptive
 * and therefore faster.
 *
 * \ingroup IntensityImageFilters
 *
 */
template <class TImage>
class HistogramEqualizationImageFilter:
    public ImageToImageFilter<TImage,TImage>
{
public:
  /** Standard class typedefs. */
  typedef HistogramEqualizationImageFilter    Self;
  typedef ImageToImageFilter<TImage,TImage>   Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( HistogramEqualizationImageFilter, ImageToImageFilter );

    /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TImage::ImageDimension ) ;

  /** Typedef to describe the input/output image types. */
  typedef TImage          InputImageType;
  typedef TImage          OutputImageType;

  /** Typedef to describe the output and input image region types. */
  typedef typename InputImageType::RegionType InputImageRegionType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  /** Typedef to describe the pointer to the input/output. */
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** Typedef to describe the type of pixel. */
  typedef typename InputImageType::PixelType InputImagePixelType;
  typedef typename OutputImageType::PixelType OutputImagePixelType;

  /** Typedef to describe the output and input image index and size types. */
  typedef typename InputImageType::IndexType InputImageIndexType;
  typedef typename InputImageType::SizeType InputImageSizeType;
  typedef typename InputImageType::OffsetType InputImageOffsetType;
  typedef typename OutputImageType::IndexType OutputImageIndexType;
  typedef typename OutputImageType::SizeType OutputImageSizeType;
  typedef typename OutputImageType::OffsetType OutputImageOffsetType;

  /** Typedefs for mask support */
  typedef unsigned char                           MaskPixelType;
  typedef Image<MaskPixelType, ImageDimension>    MaskImageType;
  typedef typename MaskImageType::Pointer         MaskImagePointer;

  /** Set/Get mask */
  itkSetObjectMacro( Mask, MaskImageType );
  itkGetObjectMacro( Mask, MaskImageType );

  //itkSetMacro( NumberOfBins, unsigned int );
  //itkGetConstReferenceMacro( NumberOfBins, unsigned int );

protected:
  HistogramEqualizationImageFilter();
  ~HistogramEqualizationImageFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef itk::Array<OutputImagePixelType> LUTType;
  LUTType m_LUT;

  unsigned int        m_NumberOfBins;
  InputImagePixelType m_Min;
  InputImagePixelType m_Max;
  double              m_MeanFrequency;
  MaskImagePointer    m_Mask;

  /** Initialize some accumulators before the threads run.
   * Create a LUT */
  virtual void BeforeThreadedGenerateData( void );

  /** Tally accumulated in threads. */
  virtual void AfterThreadedGenerateData( void );

  /** Multi-thread version GenerateData. Applies the LUT on the image. */
  virtual void ThreadedGenerateData(
    const OutputImageRegionType & outputRegionForThread,
    ThreadIdType threadId );

private:
  HistogramEqualizationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end class HistogramEqualizationImageFilter


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogramEqualizationImageFilter.hxx"
#endif

#endif
