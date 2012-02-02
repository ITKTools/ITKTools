#ifndef __itkHistogramEqualizationImageFilter_h
#define __itkHistogramEqualizationImageFilter_h

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
  typedef HistogramEqualizationImageFilter         Self;
  typedef ImageToImageFilter<TImage,TImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(HistogramEqualizationImageFilter, ImageToImageFilter);

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
  typedef Image< MaskPixelType, ImageDimension>   MaskImageType;
  typedef typename MaskImageType::Pointer         MaskImagePointer;

  /** Set/Get mask */
  itkSetObjectMacro( Mask, MaskImageType );
  itkGetObjectMacro( Mask, MaskImageType );

protected:
  HistogramEqualizationImageFilter();
  ~HistogramEqualizationImageFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;



  typedef itk::Array<OutputImagePixelType> LUTType;
  LUTType m_LUT;

  unsigned int m_NumberOfBins;
  InputImagePixelType m_Min;
  InputImagePixelType m_Max;
  double m_MeanFrequency;
  MaskImagePointer m_Mask;

  /** Initialize some accumulators before the threads run.
   * Create a LUT */
  void BeforeThreadedGenerateData ();

  /** Tally accumulated in threads. */
  void AfterThreadedGenerateData ();

  /** Multi-thread version GenerateData. Applies the LUT on the image. */
  void  ThreadedGenerateData (const OutputImageRegionType&
                              outputRegionForThread,
                              ThreadIdType threadId) ;
private:
  HistogramEqualizationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogramEqualizationImageFilter.hxx"
#endif

#endif

