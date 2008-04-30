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

  /** Typedef's. */
  typedef TInputImage                               InputImageType;
  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename InputImageType::ConstPointer     InputImageConstPointer;
  typedef typename InputImageType::PixelType        InputImagePixelType;
  typedef ImageRegion< ImageDimension >             ImageRegionType;

  /***/
  itkSetObjectMacro( OutputRegion );

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
  virtual void EnlargeOutputRequestedRegion( DataObject * );

  /** Starts the image modelling process. */
  void GenerateData( void );

private:

  ReshapeImageToImageFilter( const Self& ); // purposely not implemented
  void operator=( const Self& );        // purposely not implemented
  
  /** Private variables. */
  ImageRegionType   m_OutputRegion;

}; // end class ReshapeImageToImageFilter


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkReshapeImageToImageFilter.txx"
#endif


#endif // end #ifndef _itkReshapeImageToImageFilter_h
