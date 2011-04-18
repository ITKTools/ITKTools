#ifndef _itkReshapeImageToImageFilter_h
#define _itkReshapeImageToImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegion.h"

namespace itk
{

/** \class ReshapeImageToImageFilter
 * \brief ReshapeImageToImageFilter
 *
 *
 * \ingroup ??
 */

template <class TInputImage >
class ITK_EXPORT ReshapeImageToImageFilter:
    public ImageToImageFilter< TInputImage, TInputImage >
{
public:

  /** Standard class typedefs. */
  typedef ReshapeImageToImageFilter         Self;
  typedef ImageToImageFilter<
    TInputImage, TInputImage >              Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ReshapeImageToImageFilter, ImageToImageFilter );

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension );

  /** Typedefs from Superclass. */
  typedef typename Superclass::InputImageType         ImageType;
  typedef typename Superclass::InputImagePointer      ImagePointer;
  typedef typename Superclass::InputImageConstPointer ImageConstPointer;
  typedef typename Superclass::InputImageRegionType   ImageRegionType;
  typedef typename Superclass::InputImagePixelType    ImagePixelType;

  typedef typename ImageRegionType::SizeType          SizeType;

  /***/
  itkSetMacro( OutputSize, SizeType );

  /** FlipImageFilter produces an image with different origin and
   * direction than the input image. As such, FlipImageFilter needs to
   * provide an implementation for GenerateOutputInformation() in
   * order to inform the pipeline execution model.  The original
   * documentation of this method is below.
   * \sa ProcessObject::GenerateOutputInformaton()
   */
  virtual void GenerateOutputInformation();

  /** FlipImageFilter needs different input requested region than the output
   * requested region.  As such, FlipImageFilter needs to provide an
   * implementation for GenerateInputRequestedRegion() in order to inform the
   * pipeline execution model.
   * \sa ProcessObject::GenerateInputRequestedRegion()
   */
  virtual void GenerateInputRequestedRegion();

protected:

  /** Constructor. */
  ReshapeImageToImageFilter();

  /** Destructor. */
  ~ReshapeImageToImageFilter(){};

  /** PrintSelf. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

  /** This filter must produce all of the outputs at once, as such it
   * must override the EnlargeOutputRequestedRegion method to enlarge the
   * output request region.
   */
  //virtual void EnlargeOutputRequestedRegion( DataObject * );

  /** Performs the image reshaping process. */
  virtual void GenerateData( void );
//   virtual void ThreadedGenerateData(
//     const OutputImageRegionType & outputRegionForThread,
//     int threadId
private:

  ReshapeImageToImageFilter( const Self& ); // purposely not implemented
  void operator=( const Self& );        // purposely not implemented

  /** Private variables. */
  SizeType  m_OutputSize;

}; // end class ReshapeImageToImageFilter


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkReshapeImageToImageFilter.txx"
#endif


#endif // end #ifndef _itkReshapeImageToImageFilter_h
