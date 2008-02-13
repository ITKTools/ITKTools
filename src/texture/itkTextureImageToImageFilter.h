#ifndef _itkTextureImageToImageFilter_h
#define _itkTextureImageToImageFilter_h

#include "itkImageToImageFilter.h"

#include "itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.h"
#include "itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.h"


namespace itk
{

/** \class TextureImageToImageFilter
 * \brief TextureImageToImageFilter computes up to 8 texture measures
 * on an image.
 *
 * The operations this filter performes several steps:\n
 * - for each pixel a neighborhood around that pixel is copied to an image \n
 * - from this local image a co-occurrence matrix is constructed, using the
 *   itk::kScalarImageToGreyLevelCooccurrenceMatrixGenerator class \n
 * - from the co-oocurrence matrix several features are computed, using
 *   the itk::GreyLevelCooccurrenceMatrixTextureCoefficientsCalculator class \n
 * - each feature value is copied to the corresponding output image.
 *
 * This last class is based on several papers from Haralick and Conners:
 *
 * Haralick, R.M., K. Shanmugam and I. Dinstein. 1973.  Textural Features for 
 * Image Classification. IEEE Transactions on Systems, Man and Cybernetics. 
 * SMC-3(6):610-620.
 *
 * Haralick, R.M. 1979. Statistical and Structural Approaches to Texture. 
 * Proceedings of the IEEE, 67:786-804.
 *
 * R.W. Conners and C.A. Harlow. A Theoretical Comparison of Texture Algorithms. 
 * IEEE Transactions on Pattern Analysis and Machine Intelligence, 2:204-222, 1980.
 *
 * R.W. Conners, M.M. Trivedi, and C.A. Harlow. Segmentation of a High-Resolution
 * Urban Scene using Texture Operators. Computer Vision, Graphics and Image 
 * Processing, 25:273-310, 1984.
 *
 * \todo this class does not work properly for large offsets and small neighborhoods.
 * \ingroup ??
 */ 

template <class TInputImage, 
  class TOutputImage = Image< double, ::itk::GetImageDimension<TInputImage>::ImageDimension> >
class ITK_EXPORT TextureImageToImageFilter: 
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  /** Standard class typedefs. */
  typedef TextureImageToImageFilter         Self;
  typedef ImageToImageFilter<
    TInputImage, TOutputImage>              Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( TextureImageToImageFilter, ImageToImageFilter );

  /** Typedef's. */
  typedef TInputImage                               InputImageType;
  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename InputImageType::ConstPointer     InputImageConstPointer;
  typedef typename InputImageType::PixelType        InputImagePixelType;
  typedef typename InputImageType::RegionType       InputImageRegionType;
  typedef typename InputImageType::SizeType         InputImageSizeType;
  typedef TOutputImage                              OutputImageType;
  typedef typename OutputImageType::PixelType       OutputImagePixelType;
  typedef typename OutputImageType::Pointer         OutputImagePointer;

  /** Typedefs for texture calculation. */
  typedef Statistics::ScalarImageToGrayLevelCooccurrenceMatrixGenerator<
    InputImageType >                                CooccurrenceMatrixGeneratorType;
  typedef typename CooccurrenceMatrixGeneratorType
    ::HistogramType                                 HistogramType;
  typedef typename CooccurrenceMatrixGeneratorType
    ::OffsetType                                    OffsetType;
  typedef typename CooccurrenceMatrixGeneratorType
    ::OffsetVector                                  OffsetVector;
  typedef typename CooccurrenceMatrixGeneratorType
    ::OffsetVectorPointer                           OffsetVectorPointer;
  typedef typename CooccurrenceMatrixGeneratorType
    ::OffsetVectorConstPointer                      OffsetVectorConstPointer;

  typedef Statistics::GrayLevelCooccurrenceMatrixTextureCoefficientsCalculator<
    HistogramType >                                 TextureCalculatorType;

  /** Input Image dimension. */
  itkStaticConstMacro( InputImageDimension, unsigned int, TInputImage::ImageDimension );

  /** *****
   * Functions for this class.
   *  *****
   */

  /** Set the number of requested output texture features. */
  itkSetClampMacro( NumberOfRequestedOutputs, unsigned int, 1, 8 );

  /** Set the size of the neighborhood over which local texture is computed. */
  itkSetMacro( NeighborhoodRadius, unsigned int );

  /** *****
   * Functions that influence the co-occurrence matrix generation.
   *  *****
   */

  /** Set number of histogram bins along each axis */
  itkSetMacro( NumberOfHistogramBins, unsigned int );
  itkGetConstMacro( NumberOfHistogramBins, unsigned int );

  /** Set the offset or offsets over which the co-occurrence pairs will be computed.
   * Calling either of these methods clears the previous offsets.
   */
  virtual void SetOffsets( OffsetVector * vec );
  itkGetConstObjectMacro( Offsets, OffsetVector );

  /** Set some default offset scales. */
  virtual void SetOffsetScales( const std::vector<unsigned int> & offsetScales );
  virtual const std::vector<unsigned int> & GetOffsetScales( void ) const;

  /** Set the calculator to normalize the histogram. */
  itkSetMacro( NormalizeHistogram, bool );
  itkGetMacro( NormalizeHistogram, bool );
  itkBooleanMacro( NormalizeHistogram );

  /** Set the minimum and maximum pixel value that will be
   * placed in the histogram. By default the minimum and maximum
   * of the input image.
   */
  virtual void SetHistogramMinimum( InputImagePixelType min );
  virtual void SetHistogramMaximum( InputImagePixelType max );
  itkGetConstMacro( HistogramMinimum, InputImagePixelType );
  itkGetConstMacro( HistogramMaximum, InputImagePixelType );

protected:

  /** Constructor. */
  TextureImageToImageFilter();

  /** Destructor. */
  ~TextureImageToImageFilter(){};

  /** PrintSelf. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

  /** This filter must produce all of the outputs at once, as such it
   * must override the EnlargeOutputRequestedRegion method to enlarge the 
   * output request region.
   */
  virtual void EnlargeOutputRequestedRegion( DataObject * );

  /** Starts the image modelling process. */
  void GenerateData( void );

private:

  TextureImageToImageFilter( const Self& ); // purposely not implemented
  void operator=( const Self& );            // purposely not implemented

  /** Private function to control the output. */
  virtual void SetAndCreateOutputs( unsigned int n );

  /** Private function to compute sensible defaults. */
  virtual void ComputeDefaultOffsets( std::vector<unsigned int> scales );
  virtual void ComputeHistogramMinimumAndMaximum( void );

  /** Private variables to store results. */
  unsigned int              m_NumberOfRequestedOutputs;
  unsigned int              m_NeighborhoodRadius;

  /** Private variables for the offsets. */
  OffsetVectorPointer       m_Offsets;
  bool                      m_OffsetsSetManually;
  std::vector<unsigned int> m_OffsetScales;

  /** Private variables for the co-oocurence matrix (histogram). */
  unsigned int              m_NumberOfHistogramBins;
  InputImagePixelType       m_HistogramMinimum;
  InputImagePixelType       m_HistogramMaximum;
  bool                      m_HistogramMinimumSetManually;
  bool                      m_HistogramMaximumSetManually;
  bool                      m_NormalizeHistogram;
  
}; // end class TextureImageToImageFilter


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTextureImageToImageFilter.txx"
#endif


#endif // end #ifndef _itkTextureImageToImageFilter_h
