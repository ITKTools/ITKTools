#ifndef __itkCartesianToSphericalCoordinateImageFilter_h
#define __itkCartesianToSphericalCoordinateImageFilter_h

#include "itkVector.h"
#include "itkImageToImageFilter.h"
#include "itkBSplineKernelFunction.h"
#include "itkInterpolateImageFunction.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

namespace itk
{

/** \class CartesianToSphericalCoordinateImageFilter
 * \brief Transform an image to spherical coordinates using a kind of ParzenWindow approach.
 *
 * This filter computes the spherical transform of a 3D image.
 * It uses a quite unconventional way. Instead of 'shooting rays' and
 * interpolating the xyz image it walks over the xyz image and computes
 * the contribution of each voxel to each r-theta-phi voxel in the output
 * image, using a linear parzen window. Multiple (random) samples per xyz voxel
 * may be taken to make sure that every r-theta-phi is filled with a sensible
 * value.
 *
 * Since this filter produces an image which is a different size than
 * its input, it needs to override several of the methods defined
 * in ProcessObject in order to properly manage the pipeline execution model.
 * In particular, this filter overrides
 * ProcessObject::GenerateInputRequestedRegion() and
 * ProcessObject::GenerateOutputInformation().
 *
 * \ingroup GeometricTransforms
 */
template < class TInputImage, class TOutputImage >
class CartesianToSphericalCoordinateImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef CartesianToSphericalCoordinateImageFilter     Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( CartesianToSphericalCoordinateImageFilter, ImageToImageFilter );

  /** Number of dimensions. */
  itkStaticConstMacro( ImageDimension, unsigned int, TOutputImage::ImageDimension );
  itkStaticConstMacro( InputImageDimension, unsigned int, TInputImage::ImageDimension );

  typedef TInputImage                           InputImageType;
  typedef TOutputImage                          OutputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename OutputImageType::Pointer     OutputImagePointer;

  typedef unsigned char                         MaskPixelType;
  typedef Image<MaskPixelType, ImageDimension>  MaskImageType;
  typedef typename MaskImageType::Pointer       MaskImagePointer;

  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename OutputImageType::RegionType  OutputImageRegionType;

  typedef typename OutputImageType::SizeType    SizeType;
  typedef typename OutputImageType::IndexType   IndexType;
  typedef ContinuousIndex<
    double,
    itkGetStaticConstMacro( InputImageDimension )> ContinuousIndexType;
  typedef typename OutputImageType::PointType   PointType;
  typedef typename OutputImageType::SpacingType SpacingType;
  typedef typename OutputImageType::PointType   OriginPointType;
  typedef Vector<
    double,
    itkGetStaticConstMacro( InputImageDimension )> VectorType;
  typedef typename PointType::CoordRepType      CoordRepType;

  typedef typename OutputImageType::PixelType   OutputPixelType;
  typedef typename InputImageType::PixelType    InputPixelType;
  typedef typename
    NumericTraits<InputPixelType>::RealType     InternalPixelType;
  typedef Image<
    InternalPixelType,
    itkGetStaticConstMacro( InputImageDimension )> InternalImageType;

  /** The random number generator used to generate random coordinates. */
  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;

  typedef InterpolateImageFunction<
    InputImageType, CoordRepType>               InterpolatorType;

  /** Set/Get an interpolator; not mandatory. Implicitly, nearest
   * neighbor interpolation is used if you don't set it.  */
  itkSetObjectMacro( Interpolator, InterpolatorType );
  itkGetObjectMacro( Interpolator, InterpolatorType );

  /** Set/Get a mask. Only that part of the input image is transformed
   * that has a nonzero mask-value.
   * It is assumed that the supplied mask image has the same size as the
   * the input image */
  itkSetObjectMacro( MaskImage, MaskImageType );
  itkGetObjectMacro( MaskImage, MaskImageType );

  /** Set/Get the size of the output image. [r, theta, phi]. */
  itkSetMacro( OutputSize, SizeType );
  itkGetConstReferenceMacro( OutputSize, SizeType );

  /** Set/Get the center of rotation */
  itkSetMacro( CenterOfRotation, PointType );
  itkGetConstReferenceMacro( CenterOfRotation, PointType );

  /** Set the maximum number of random samples per pixel */
  itkSetMacro( MaximumNumberOfSamplesPerVoxel, unsigned int );
  itkGetConstMacro( MaximumNumberOfSamplesPerVoxel, unsigned int );

  /** CartesianToSphericalCoordinateImageFilter produces an image which is a different size
   * than its input.  As such, it needs to provide an implementation
   * for GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below. \sa ProcessObject::GenerateOutputInformaton() */
  virtual void GenerateOutputInformation();

  /** CartesianToSphericalCoordinateImageFilter needs a different input requested region than
   * the output requested region.  As such, CartesianToSphericalCoordinateImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion()
   * in order to inform the pipeline execution model.
   * \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

  /** Get the random generator; useful to set its seed */
  itkGetObjectMacro( RandomGenerator, RandomGeneratorType );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( OutputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<OutputPixelType>) );
  /** End concept checking */
#endif

protected:
  CartesianToSphericalCoordinateImageFilter();
  ~CartesianToSphericalCoordinateImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef BSplineKernelFunction<1>            KernelType;
  typedef Matrix<double,
    itkGetStaticConstMacro( ImageDimension ), 2> ParzenWeightContainerType;

  /** Function that does the work */
  virtual void GenerateData( void );

  /** Generate a point randomly in a bounding box. */
  inline void GenerateRandomCoordinate(
    const PointType & inputPoint,
    PointType & randomPoint );

  typename RandomGeneratorType::Pointer m_RandomGenerator;

  SpacingType             m_OutputSpacing; // output image spacing
  SpacingType             m_InputSpacing; // input image spacing cached
  OriginPointType         m_OutputOrigin;  // output image origin
  IndexType               m_OutputStartIndex; // output image start index

private:
  CartesianToSphericalCoordinateImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  SizeType                m_OutputSize;       // Size of the output image
  PointType               m_CenterOfRotation;
  unsigned int            m_MaximumNumberOfSamplesPerVoxel;

  MaskImagePointer        m_MaskImage;
  typename InterpolatorType::Pointer m_Interpolator;

}; // end class


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCartesianToSphericalCoordinateImageFilter.txx"
#endif

#endif // end #ifndef __itkCartesianToSphericalCoordinateImageFilter_h
