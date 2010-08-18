#ifndef __itkGaussianInvariantsImageFilter_h
#define __itkGaussianInvariantsImageFilter_h

#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkHessianRecursiveGaussianImageFilter2.h"
#include "itkFixedArray.h"


namespace itk
{

/** \class GaussianInvariantsImageFilter
 * \brief Computes the
 *
 * \ingroup IntensityImageFilters
 * \ingroup Singlethreaded
 */

template < typename TInputImage,typename TOutputImage = TInputImage >
class ITK_EXPORT GaussianInvariantsImageFilter:
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  /** Standard class typedefs. */
  typedef GaussianInvariantsImageFilter                     Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( GaussianInvariantsImageFilter, ImageToImageFilter );

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Pixel Type of the input image */
  typedef TInputImage                                       InputImageType;
  typedef typename InputImageType::Pointer                  InputImagePointer;
  typedef typename InputImageType::ConstPointer             InputImageConstPointer;
  typedef typename InputImageType::PixelType                InputPixelType;
  typedef TOutputImage                                      OutputImageType;
  typedef typename OutputImageType::Pointer                 OutputImagePointer;
  typedef typename OutputImageType::PixelType               OutputPixelType;
  typedef typename NumericTraits<
    InputPixelType>::RealType                               RealType;
  typedef typename NumericTraits<
    InputPixelType>::ScalarRealType                         ScalarRealType;
  typedef Image< ScalarRealType,
    itkGetStaticConstMacro( ImageDimension ) >              RealImageType;

  /** Typedef's for gradient and Hessian computers. */
  typedef SmoothingRecursiveGaussianImageFilter2<
    InputImageType, RealImageType >                         DerivativeFilterType;
  typedef typename DerivativeFilterType::Pointer            DerivativeFilterPointer;
  typedef typename DerivativeFilterType::OrderType          OrderType;
  typedef HessianRecursiveGaussianImageFilter2<
    InputImageType >                                        HessianFilterType;
  typedef typename HessianFilterType::Pointer               HessianFilterPointer;
  typedef typename HessianFilterType::OutputImageType       HessianOutputImageType;
  typedef typename HessianOutputImageType::PixelType        HessianPixelType;

  /** Set Sigma value. Sigma is measured in the units of image spacing.  */
  typedef FixedArray< ScalarRealType,
    itkGetStaticConstMacro( ImageDimension ) >              SigmaType;
  virtual void SetSigma( const ScalarRealType sigma );
  virtual void SetSigma( const SigmaType sigma );
  itkGetMacro( Sigma, SigmaType );

  /** Define which normalization factor will be used for the Gaussians. */
  virtual void SetNormalizeAcrossScale( const bool arg );
  itkGetMacro( NormalizeAcrossScale, bool );

  /** Set which invariant is computed. */
  void SetInvariant( std::string arg );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( InputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<InputPixelType>) );
  /** End concept checking */
#endif

protected:
  GaussianInvariantsImageFilter();
  virtual ~GaussianInvariantsImageFilter() {};
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
  GaussianInvariantsImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Member variables. */
  bool        m_NormalizeAcrossScale;
  SigmaType   m_Sigma;
  std::string m_Invariant;

  std::vector< DerivativeFilterPointer >  m_DerivativeFilters;
  HessianFilterPointer                    m_HessianFilter;

}; // end class GaussianInvariantsImageFilter

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGaussianInvariantsImageFilter.txx"
#endif

#endif

