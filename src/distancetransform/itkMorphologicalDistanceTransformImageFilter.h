#ifndef __itkMorphologicalDistanceTransformImageFilter_h
#define __itkMorphologicalDistanceTransformImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkProgressReporter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkParabolicErodeImageFilter.h"
#include "itkSqrtImageFilter.h"

namespace itk
{
/**
 * \class MorphologicalDistanceTransformImageFilter
 * \brief Distance transform of a mask using parabolic morphological
 * methods
 * 
 * Morphological erosions using a parabolic structuring element can be
 * used to compute a distance transform of a mask by setting the
 * "Outside" value to 0 and the "inside" value to +infinity. The
 * output of the parabolic erosion are the square of the distance to
 * the nearest zero valued voxel. Thus we can compute the distance
 * transform by taking the sqrt of the erosion.
 *
 * The output pixel type needs to support values as large as the
 * square of the largest value of the distance - just use float to be
 * safe.
 * 
 * \author Richard Beare, Monash University, Department of Medicine,
 * Melbourne, Australia. <Richard.Beare@med.monash.edu.au>
 *
**/


template <typename TInputImage, typename TOutputImage= TInputImage>
class ITK_EXPORT MorphologicalDistanceTransformImageFilter:
    public ImageToImageFilter<TInputImage,
			      TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MorphologicalDistanceTransformImageFilter Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(MorphologicalDistanceTransformImageFilter, ImageToImageFilter);


  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef TOutputImage                                   OutputImageType;
  typedef typename TInputImage::PixelType                InputPixelType;
  typedef typename NumericTraits<InputPixelType>::RealType    RealType;
  typedef typename NumericTraits<InputPixelType>::ScalarRealType ScalarRealType;
  typedef typename TOutputImage::PixelType  OutputPixelType;

  /** Smart pointer typedef support.  */
  typedef typename TInputImage::Pointer  InputImagePointer;
  typedef typename TInputImage::ConstPointer  InputImageConstPointer;
 
  /** Image related typedefs. */
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** a type to represent the "kernel radius" */
  typedef typename itk::FixedArray<ScalarRealType, TInputImage::ImageDimension> RadiusType;
  virtual void Modified() const;

  /** this describes the input mask - default value 0 - we compute the
  distance from all voxels with value not equal to "OutsideValue" to
  the nearest voxel with value "OutsideValue" */
  itkSetMacro(OutsideValue, InputPixelType);
  itkGetConstReferenceMacro(OutsideValue, InputPixelType);

  
  
  /** Is the transform in world or voxel units - default is world */
  void SetUseImageSpacing(bool uis)
  {
    m_Erode->SetUseImageSpacing(uis);
  }


  const bool GetUseImageSpacing()
  {
    return m_Erode->GetUseImageSpacing();
  }

  itkSetMacro(SqrDist, bool);
  itkGetConstReferenceMacro(SqrDist, bool);
  itkBooleanMacro(SqrDist);


#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimension,
                  (Concept::SameDimension<itkGetStaticConstMacro(InputImageDimension),itkGetStaticConstMacro(OutputImageDimension)>));

  itkConceptMacro(Comparable,
		  (Concept::Comparable<InputPixelType>));

  /** End concept checking */
#endif



protected:
  MorphologicalDistanceTransformImageFilter();
  virtual ~MorphologicalDistanceTransformImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );
  
  // do everything in the output image type, which should have high precision
  typedef typename itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> ThreshType;
  typedef typename itk::ParabolicErodeImageFilter<OutputImageType, OutputImageType> ErodeType;
  typedef typename itk::SqrtImageFilter<OutputImageType, OutputImageType> SqrtType;

private:
  MorphologicalDistanceTransformImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputPixelType m_OutsideValue;
  typename ErodeType::Pointer m_Erode;
  typename ThreshType::Pointer m_Thresh;
  typename SqrtType::Pointer m_Sqrt;
  bool m_SqrDist;
};

} // namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMorphologicalDistanceTransformImageFilter.txx"
#endif


#endif
