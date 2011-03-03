#ifndef __itkMorphologicalSignedDistanceTransformImageFilter_h
#define __itkMorphologicalSignedDistanceTransformImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkProgressReporter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkParabolicErodeImageFilter.h"
#include "itkParabolicDilateImageFilter.h"
#include "itkMorphSDTHelperImageFilter.h"

namespace itk
{
/**
 * \class MorphologicalSignedDistanceTransformImageFilter
 * \brief Signed distance transform of a mask using parabolic morphological
 * methods
 * 
 * Morphological erosions using a parabolic structuring element can be
 * used to compute a distance transform of a mask by setting the
 * "Outside" value to 0 and the "inside" value to +infinity (or beyond
 * the maximum possible value). The
 * output of the parabolic erosion are the square of the distance to
 * the nearest zero valued voxel. Thus we can compute the distance
 * transform by taking the sqrt of the erosion.
 *
 * The output pixel type needs to support values as large as the
 * square of the largest value of the distance - just use float to be
 * safe.
 * 
 * The inside is considered to have negative distances. Use
 * InsideIsPositive(bool) to change.
 * 
 * There are also OutsideValue methods which can be used in similar
 * ways.
 *
 * Otherwise it is meant to have an interface to the other
 * DistanceTransforms filters.
 *
 * \author Richard Beare, Monash University, Department of Medicine,
 * Melbourne, Australia. <Richard.Beare@med.monash.edu.au>
 *
**/


template <typename TInputImage, typename TOutputImage= TInputImage>
class ITK_EXPORT MorphologicalSignedDistanceTransformImageFilter:
    public ImageToImageFilter<TInputImage,
			      TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MorphologicalSignedDistanceTransformImageFilter Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(MorphologicalSignedDistanceTransformImageFilter, ImageToImageFilter);


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
  
  /** Set On/Off whether spacing is used. */
  itkBooleanMacro( UseImageSpacing );

  /** Set if the inside represents positive values in the signed distance
   *  map. By convention ON pixels are treated as inside pixels.           */
  itkSetMacro( InsideIsPositive, bool );

  /** Get if the inside represents positive values in the signed distance map.
   *  See GetInsideIsPositive()  */
  itkGetConstReferenceMacro( InsideIsPositive, bool );

    /** Set if the inside represents positive values in the signed distance
     * map. By convention ON pixels are treated as inside pixels. Default is
     * true.                             */
  itkBooleanMacro( InsideIsPositive );
  /** Is the transform in world or voxel units - default is world */
  void SetUseImageSpacing(bool uis)
  {
    m_Erode->SetUseImageSpacing(uis);
    m_Dilate->SetUseImageSpacing(uis);
    this->Modified();
  }


  const bool GetUseImageSpacing()
  {
    return m_Erode->GetUseImageSpacing();
  }



#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimension,
                  (Concept::SameDimension<itkGetStaticConstMacro(InputImageDimension),itkGetStaticConstMacro(OutputImageDimension)>));

  itkConceptMacro(Comparable,
		  (Concept::Comparable<InputPixelType>));

  /** End concept checking */
#endif



protected:
  MorphologicalSignedDistanceTransformImageFilter();
  virtual ~MorphologicalSignedDistanceTransformImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );
  
  // do everything in the output image type, which should have high precision
  typedef typename itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> ThreshType;
  typedef typename itk::ParabolicErodeImageFilter<OutputImageType, OutputImageType> ErodeType;
  typedef typename itk::ParabolicDilateImageFilter<OutputImageType, OutputImageType> DilateType;
  typedef typename itk::MorphSDTHelperImageFilter<OutputImageType, OutputImageType> HelperType;

private:
  MorphologicalSignedDistanceTransformImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputPixelType m_OutsideValue;
  bool m_InsideIsPositive;
  typename ErodeType::Pointer m_Erode;
  typename DilateType::Pointer m_Dilate;
  typename ThreshType::Pointer m_Thresh;
  typename HelperType::Pointer m_Helper;
};

} // namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMorphologicalSignedDistanceTransformImageFilter.txx"
#endif


#endif
