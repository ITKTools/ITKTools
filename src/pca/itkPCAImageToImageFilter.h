#ifndef _itkPCAImageToImageFilter_h
#define _itkPCAImageToImageFilter_h

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"


namespace itk
{

/** \class PCAImageToImageFilter
 * \brief PCAImageToImageFilter performs a principal component analysis
 * (PCA) on a set of images.
 *
 * The user specifies the number of input feature images on which a PCA
 * is performed. Additionally, the number of desired largest principal
 * components needed can be specified, which is by default equal to the
 * maximum number of principal components, which is in turn equal to the
 * number of input feature images.
 *
 * The algorithm uses the VNL library to perform the eigen analysis and
 * to perform some matrix manipulations. This filter gives the same output
 * as the Matlab function princomp.
 *
 * \ingroup ??
 */

template <class TInputImage,
          class TOutputImage = Image< double, ::itk::GetImageDimension<TInputImage>::ImageDimension> >
class ITK_EXPORT PCAImageToImageFilter:
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  /** Standard class typedefs. */
  typedef PCAImageToImageFilter             Self;
  typedef ImageToImageFilter<
    TInputImage, TOutputImage>              Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( PCAImageToImageFilter, ImageToImageFilter );

  /** Typedef's. */
  typedef TInputImage                               InputImageType;
  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename InputImageType::ConstPointer     InputImageConstPointer;
  typedef typename InputImageType::PixelType        InputImagePixelType;
  typedef TOutputImage                              OutputImageType;
  typedef typename OutputImageType::PixelType       OutputImagePixelType;
  typedef typename OutputImageType::Pointer         OutputImagePointer;
  typedef ImageRegionIterator< TInputImage >        InputImageIterator;
  typedef ImageRegionConstIterator< TInputImage >   InputImageConstIterator;
  typedef ImageRegionIterator< TOutputImage >       OutputImageIterator;

  /** Input Image dimension. */
  itkStaticConstMacro( InputImageDimension, unsigned int, TInputImage::ImageDimension );

  /** Type definition for a vnl double matrix. */
  typedef vnl_matrix<double> MatrixOfDoubleType;

  /** Type definition for a vnl double vector. */
  typedef vnl_vector<double> VectorOfDoubleType;

  /** Set/Get the number of feature images in the input. */
  virtual void SetNumberOfFeatureImages( unsigned int n );
  itkGetConstMacro( NumberOfFeatureImages, unsigned int );

  /** Set/Get the number of required largest principal components.
   * The filter produces the required number of principal components,
   * but maximally the largest number of principal components.
   */
  virtual void SetNumberOfPrincipalComponentsRequired( unsigned int n );
  itkGetConstMacro( NumberOfPrincipalComponentsRequired, unsigned int );

  /** Get the eigen values. */
  itkGetConstReferenceMacro( EigenValues, VectorOfDoubleType );

  /** Get the normalised eigen values. */
  itkGetConstReferenceMacro( NormalisedEigenValues, VectorOfDoubleType );

  /** Get the eigen values. */
  itkGetConstReferenceMacro( EigenVectors, MatrixOfDoubleType );

protected:

  /** Constructor. */
  PCAImageToImageFilter();

  /** Destructor. */
  ~PCAImageToImageFilter(){};

  /** PrintSelf. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

  /** This filter must produce all of the outputs at once, as such it
   * must override the EnlargeOutputRequestedRegion method to enlarge the
   * output request region.
   */
  virtual void EnlargeOutputRequestedRegion( DataObject * );

  /** This filter requires all the input image at once, as such it
   * must override the GenerateInputRequestedRegion method. Additionally,
   * this filter assumes that the input images are at least the size as
   * the first input image.
   */
  virtual void GenerateInputRequestedRegion( void );

  /** Starts the image modelling process. */
  void GenerateData( void );

private:

  PCAImageToImageFilter( const Self& ); // purposely not implemented
  void operator=( const Self& );        // purposely not implemented

  /** Private functions to control the output. */
  virtual void SetAndCreateOutputs( unsigned int n );
  virtual void CheckNumberOfOutputs( void );

  /** Private functions to perform the PCA. */
  virtual void PerformPCA( void );
  virtual void CalculateMeanOfFeatureImages( void );
  virtual void CenterFeatureImages( void );
  virtual void CalculateCovarianceMatrix( void );
  virtual void PerformEigenAnalysis( void );

  /** Private variables to store results. */
  VectorOfDoubleType    m_MeanOfFeatureImages;
  MatrixOfDoubleType    m_CenteredFeatureImages;

  MatrixOfDoubleType    m_CovarianceMatrix;
  MatrixOfDoubleType    m_EigenVectors;
  VectorOfDoubleType    m_EigenValues;
  VectorOfDoubleType    m_NormalisedEigenValues;
  MatrixOfDoubleType    m_PrincipalComponents;

  unsigned int          m_NumberOfPixels;
  unsigned int          m_NumberOfFeatureImages;
  unsigned int          m_NumberOfPrincipalComponentsRequired;

}; // end class PCAImageToImageFilter


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPCAImageToImageFilter.txx"
#endif


#endif // end #ifndef _itkPCAImageToImageFilter_h
