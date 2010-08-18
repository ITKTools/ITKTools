/*=========================================================================

author: Bryn Lloyd, blloyd@bwh.harvard.edu
date: August, 2005
reference: following an approach described in:
           S. Warfield, "Fast k-NN classification for multichannel image data",
           Pattern Recognition Letters, 1995

=========================================================================*/
#ifndef __itkOrderKDistanceTransformImageFilter_h
#define __itkOrderKDistanceTransformImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkImage.h"
#include "itkVectorImage.h"

namespace itk
{

/** \struct SortingElement
 *
 *  This struct is used to insert the identifiers into a sorted list (increasing distance).
 */
template<class T1, class T2=unsigned int>
struct SortingElement  {
    T1 element;
    T2 index;
    bool operator<(const SortingElement v) const {
      return element<v.element;
    }
    bool operator==(const SortingElement v) const {
      return (element==v.element && index==v.index);
    }
    void operator=(SortingElement v) {
      element = v.element;
      index = v.index;
    }
};



/** \class OrderKDistanceTransformImageFilter
*
* This class is parametrized over the type of the input image
* and the type of the output image.
*
* This filter computes the distance map of the input image
* as an approximation with pixel accuracy to the Euclidean distance.
*
* The input is assumed to contain numeric codes defining objects.
* The filter will produce as output the following images:
*
* - A voronoi partition using the same numeric codes as the input.
* - A distance map with the approximation to the euclidean distance.
*   from a particular pixel to the nearest object to this pixel
*   in the input image.
* - A vector map containing the component of the vector relating
*   the current pixel with the closest point of the closest object
*   to this pixel. Given that the components of the distance are
*   computed in "pixels", the vector is represented by an
*   itk::Offset.  That is, physical coordinates are not used.
*
* This filter is N-dimensional and known to be efficient
* in computational time.  The algorithm is the N-dimensional version
* of the 4SED algorithm given for two dimensions in:
*
* Danielsson, Per-Erik.  Euclidean Distance Mapping.  Computer
* Graphics and Image Processing 14, 227-248 (1980).
*
* \ingroup ImageFeatureExtraction
*
*/
template <class TInputImage,
                    class TOutputImage,
                    class TKDistanceImage=VectorImage<float, ::itk::GetImageDimension<TInputImage>::ImageDimension>,
                    class TKIDImage=VectorImage<int, ::itk::GetImageDimension<TInputImage>::ImageDimension> >
class ITK_EXPORT OrderKDistanceTransformImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef OrderKDistanceTransformImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( OrderKDistanceTransformImageFilter, ImageToImageFilter );

  /** The dimension of the input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);


  /** Type for input image. */
  typedef   TInputImage       InputImageType;

  /** Type for the voronoiMap image.  */
  typedef   TOutputImage      OutputImageType;

  /**  Type for the VectorImage<float, InputImageDimension> of distances to k closest object pixels */
  typedef TKDistanceImage  KDistanceImageType;

  /**  Type for the VectorImage<int, InputImageDimension> of IDs of k closest object pixels */
  typedef TKIDImage    KIDImageType;

    /** Pointer Type for input image. */
  typedef typename InputImageType::ConstPointer InputImagePointer;

  /** Pointer Type for the output image. */
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** Pointer Type for the k-distance image. */
  typedef typename KDistanceImageType::Pointer    KDistanceImagePointer;

/** Pointer Type for the i-ID image. */
  typedef typename KIDImageType::Pointer          KIDImagePointer;


  /** Vector Pixel types */
  typedef typename KIDImageType::PixelType KIDPixelType;
  typedef typename KIDPixelType::ValueType KIDValueType;
  typedef typename KDistanceImageType::PixelType KDistancePixelType;
  typedef typename KDistancePixelType::ValueType KDistanceValueType;


  typedef typename InputImageType::RegionType   RegionType;
  typedef typename RegionType::IndexType             IndexType;
  typedef typename RegionType::SizeType               SizeType;
  typedef typename InputImageType::OffsetType      OffsetType;





  /** Set if the distance should be squared. Default is false (non-squared distance) */
  itkSetMacro( SquaredDistance, bool );

  /** Get the distance squared. */
  itkGetConstReferenceMacro( SquaredDistance, bool );

  /** Set On/Off if the distance is squared. */
  itkBooleanMacro( SquaredDistance );

  /** Set if the input is binary. If this variable is set, each
   * nonzero pixel in the input image will be given a unique numeric
   * code to be used by the Voronoi partition.  If the image is binary
   * but you are not interested in the Voronoi regions of the
   * different nonzero pixels, then you need not set this.  */
  itkSetMacro( InputIsBinary, bool );

  /** Get if the input is binary.  See SetInputIsBinary(). */
  itkGetConstReferenceMacro( InputIsBinary, bool );

  /** Set On/Off if the input is binary.  See SetInputIsBinary(). */
  itkBooleanMacro( InputIsBinary );

  /** Set boolean to control what kind of
   *  neighborhood is used to compute the voronoi diagram.
   *  FullyConnected= true is a 2D 8-neighborhood, or a 3D
   *  26-neighborhood.
   *  FullyConnected= false is a 2D 4-neighborhood, or a 3D
   *  6-neighborhood. */
  itkSetMacro( FullyConnected, bool );

  /** Get FullyConnected.  See SetFullyConnected(). */
  itkGetConstReferenceMacro( FullyConnected, bool );

  /** Set On/Off FullyConnected. See SetFullyConnected(). */
  itkBooleanMacro( FullyConnected );


  /** Set if image spacing should be used in computing distances. */
  itkSetMacro( UseImageSpacing, bool );

  /** Get whether spacing is used. */
  itkGetConstReferenceMacro( UseImageSpacing, bool );

  /** Set On/Off whether spacing is used. */
  itkBooleanMacro( UseImageSpacing );

  /** Set the number of closest neighbors to be computed. */
  void SetK( unsigned int K );

  /** Get the number of closest neighbors to be computed. */
  itkGetMacro( K, unsigned int );

  /** Get Voronoi Map
   * This map shows for each pixel what object is closest to it.
   * Each object should be labeled by a number (larger than 0),
   * so the map has a value for each pixel corresponding to the label
   * of the closest object.  */
  OutputImageType * GetVoronoiMap(void);

  /** Get vectorimage of distances to k-closest object pixels.
   * The distance map is shown as a gray
   * value image depending on the pixel type of the output image.
   * Regarding the source image, background should be dark (gray value
   * = 0) and object should have a gray value larger than 0.  The
   * minimal distance is calculated on the object frontier, and the
   * output image gives for each pixel its minimal distance from the
   * object (if there is more than one object the closest object is
   * considered). **/
  KDistanceImageType * GetKDistanceMap(void);

  /** Get VectorImage<int, Dimension> of IDs of k closest
   *  pixels (use together with m_IndexLookUpTable). */
  KIDImageType *       GetKclosestIDMap(void);

  /** inserts a new found closest object pixel in ascending order of distance */
  bool InsertSorted( KDistanceValueType dist, KIDValueType index, KDistancePixelType& distances, KIDPixelType& indices);


protected:
  OrderKDistanceTransformImageFilter();
  virtual ~OrderKDistanceTransformImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Compute Danielsson distance map and Voronoi Map. */
  void GenerateData();

  void GenerateInputRequestedRegion();

  /** Prepare data. */
  void PrepareData();

  /**  Compute Voronoi Map. */
  void ComputeVoronoiMap();

  /** Update distance map locally.  Used by GenerateData(). */
  void UpdateLocalDistance(const IndexType&,
                           const OffsetType&);



private:
  OrderKDistanceTransformImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool                  m_SquaredDistance;
  bool                  m_InputIsBinary;
  bool                  m_UseImageSpacing;
  bool                  m_FullyConnected;

  unsigned int    m_K;

  /** should the constructor or other method reserve memory for m_IndexLookUpTable? */
  std::vector<IndexType> m_IndexLookUpTable;

  KDistanceImagePointer m_KDistanceImage;
  KIDImagePointer             m_KIDImage;


}; // end of OrderKDistanceTransformImageFilter class

} //end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOrderKDistanceTransformImageFilter.txx"
#endif

#endif
