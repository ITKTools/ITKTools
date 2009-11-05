#ifndef __itkParabolicCloseImageFilter_h
#define __itkParabolicCloseImageFilter_h

#include "itkParabolicOpenCloseSafeBorderImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{
/**
 * \class ParabolicCloseImageFilter
 * \brief Class for morphological closing
 * operations  with parabolic structuring elements.
 *
 * This filter provides options for padded borders
 *
 * This filter is threaded.
 *
 * \sa itkParabolicOpenCloseImageFilter 
 *
 * \author Richard Beare, Department of Medicine, Monash University,
 * Australia.  <Richard.Beare@med.monash.edu.au>
**/

template <typename TInputImage,
          typename TOutputImage= TInputImage >
class ITK_EXPORT ParabolicCloseImageFilter:
    public ParabolicOpenCloseSafeBorderImageFilter<TInputImage, false,
					   TOutputImage>
{

public:
  /** Standard class typedefs. */
  typedef ParabolicCloseImageFilter  Self;
  typedef ParabolicOpenCloseSafeBorderImageFilter<TInputImage, false, TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ParabolicCloseImageFilter, ParabolicOpenCloseSafeBorderImageFilter);


  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef TOutputImage                                   OutputImageType;
  typedef typename TInputImage::PixelType                PixelType;
  typedef typename NumericTraits<PixelType>::RealType    RealType;
  typedef typename NumericTraits<PixelType>::ScalarRealType ScalarRealType;
  typedef typename TOutputImage::PixelType  OutputPixelType;

  /** Smart pointer typedef support.  */
  typedef typename TInputImage::Pointer  InputImagePointer;
  typedef typename TInputImage::ConstPointer  InputImageConstPointer;

  /** a type to represent the "kernel radius" */
  typedef typename itk::FixedArray<ScalarRealType, TInputImage::ImageDimension> RadiusType;

  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** Define the image type for internal computations
      RealType is usually 'double' in NumericTraits.
      Here we prefer float in order to save memory.  */


protected:
  ParabolicCloseImageFilter(){};
  virtual ~ParabolicCloseImageFilter() {};
//   void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  ParabolicCloseImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  
};

} // end namespace itk

#endif
