/**
 * author: Bryn Lloyd
 * date: September 2005
 */
#ifndef __itkConnectedComponentVectorImageFilter_h_
#define __itkConnectedComponentVectorImageFilter_h_

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkArray.h"
#include "itkVectorImage.h"

namespace itk
{

/**
 * \class ConnectedComponentVectorImageFilter
 * \brief Label the objects in a binary image
 *
 * ConnectedComponentVectorImageFilter labels the objects in a binary image.
 * Each distinct object is assigned a unique label. The filter makes
 * three passes through the image.  The first pass initialized the
 * output.  The second pass labels each foreground pixel such that all
 * the pixels associated with an object either have the same label or
 * have had their labels entered into a equivalency table.  The third
 * pass through the image flattens the equivalency table such that all
 * pixels for an object have the same label.
 *
 * The final object labels are in no particular order (and some object
 * labels may not be used on the final objects).  You can reorder the
 * labels such that object labels are consecutive and sorted based on
 * object size by passing the output of this filter to a
 * RelabelComponentImageFilter.
 *
 * \sa ImageToImageFilter
 */

template< class TInputImage, class TOutputImage >
class ITK_EXPORT ConnectedComponentVectorImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /**
   * Standard "Self" & Superclass typedef.
   */
  typedef ConnectedComponentVectorImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;

  /**
   * Types from the Superclass
   */
  typedef typename Superclass::InputImagePointer InputImagePointer;

  /**
   * Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same.
   */
  typedef typename TOutputImage::PixelType               OutputPixelType;
  typedef typename TInputImage::PixelType                  InputPixelType;

  /** are these needed ?*/
  typedef typename TOutputImage::InternalPixelType OutputInternalPixelType;
  typedef typename TInputImage::InternalPixelType     InputInternalPixelType;

  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /**
   * Image typedef support
   */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;

  /** Pointer Type for the output image. */
  typedef typename OutputImageType::Pointer OutputImagePointer;


  typedef   typename TInputImage::IndexType       IndexType;
  typedef   typename TInputImage::SizeType        SizeType;
  typedef   typename TOutputImage::RegionType     RegionType;
  typedef   std::list<IndexType>                  ListType;

  /**
   * Smart pointer typedef support
   */
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /**
   * Run-time type information (and related methods)
   */
  itkTypeMacro(ConnectedComponentVectorImageFilter, ImageToImageFilter);

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /**
   * Set/Get whether the connected components are defined strictly by
   * face connectivity or by face+edge+vertex connectivity.  Default is
   * FullyConnectedOff.  For objects that are 1 pixel wide, use
   * FullyConnectedOn.
   */
  itkSetMacro(FullyConnected, bool);
  itkGetConstReferenceMacro(FullyConnected, bool);
  itkBooleanMacro(FullyConnected);

protected:
  ConnectedComponentVectorImageFilter()
    {
    this->m_FullyConnected = true;
    }
  virtual ~ConnectedComponentVectorImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /**
   * Standard pipeline method.
   */
  void GenerateData();

  /** ConnectedComponentVectorImageFilter needs the entire input. Therefore
   * it must provide an implementation GenerateInputRequestedRegion().
   * \sa ProcessObject::GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion();

  /** ConnectedComponentVectorImageFilter will produce all of the output.
   * Therefore it must provide an implementation of
   * EnlargeOutputRequestedRegion().
   * \sa ProcessObject::EnlargeOutputRequestedRegion() */
  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

 /** Sort an int Array */
  InputPixelType SortArray(const InputPixelType indices);

private:
  ConnectedComponentVectorImageFilter(const Self&) {}
  bool m_FullyConnected;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConnectedComponentVectorImageFilter.txx"
#endif

#endif // end #ifndef __itkConnectedComponentVectorImageFilter_h_
