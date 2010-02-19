/*=========================================================================

author: Bryn Lloyd, blloyd@bwh.harvard.edu
date: August, 2005
reference: following an approach described in:
           S. Warfield, "Fast k-NN classification for multichannel image data",
           Pattern Recognition Letters, 1995

=========================================================================*/
#ifndef _itkOrderKDistanceTransformImageFilter_txx
#define _itkOrderKDistanceTransformImageFilter_txx

#include <iostream>

#include "itkOrderKDistanceTransformImageFilter.h"
#include "itkReflectiveImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

/** This class is needed to compute the voronoi diagram */
#include "itkConnectedComponentVectorImageFilter.h"




namespace itk
{




/**
 *    Constructor
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::OrderKDistanceTransformImageFilter()
{

  m_SquaredDistance     = false; 
  m_InputIsBinary       = true; // for my purposes this should be true as default...
  m_UseImageSpacing     = true; // this also
  m_FullyConnected    = true;  /// should this be true or false?
  m_K                   = 5;

  this->SetNumberOfRequiredOutputs( 3 );

  KDistanceImagePointer kdistanceImage = KDistanceImageType::New();
  kdistanceImage->SetVectorLength(m_K);
  this->SetNthOutput( 0, kdistanceImage.GetPointer() );

  KIDImagePointer kidImage = KIDImageType::New();
  kidImage->SetVectorLength(m_K);
  this->SetNthOutput( 1, kidImage.GetPointer() );

  OutputImagePointer voronoiMap = OutputImageType::New();
  this->SetNthOutput( 2, voronoiMap.GetPointer() );

//  m_IndexLookUpTable.reserve(1000);
}

/**
 *  Set number K of closest object pixels to compute
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::SetK( unsigned int k)
{
  m_K = k;
  // set length of arrays in these images, to accomodate for k closest points
  KDistanceImagePointer kdistanceImage = this->GetKDistanceMap();
  kdistanceImage->SetVectorLength(m_K);
  KIDImagePointer kidImage = this->GetKclosestIDMap();
  kidImage->SetVectorLength(m_K);
}






/**
 *  Return the k-distance-image
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
TKDistanceImage *
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::GetKDistanceMap(void)
{
  return  dynamic_cast< KDistanceImageType * >(
    this->ProcessObject::GetOutput(0) );
}





/**
 *  Return the k-id-image
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
TKIDImage *
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::GetKclosestIDMap(void)
{
  return  dynamic_cast< KIDImageType * >(
    this->ProcessObject::GetOutput(1) );
}




/**
 *  Return Closest Points Map
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
TOutputImage*
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::GetVoronoiMap(void)
{
  return  dynamic_cast< OutputImageType * >(
    this->ProcessObject::GetOutput(2) );
}






/**
 *  Prepare data for computation
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::PrepareData(void)
{
  
  itkDebugMacro(<< "PrepareData Start");
  
  InputImagePointer  inputImage  =
    dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );


  typename OutputImageType::RegionType region  = inputImage->GetLargestPossibleRegion() ;
  
  // find the largest of the image dimensions
  //
  // This should include the image spacing!!!! (in DanielssonDM also!)
  //
  typename TInputImage::SizeType size = region.GetSize();
  typename TInputImage::SpacingType spacing = inputImage->GetSpacing();
  double maxLength = 0;
  if (m_InputIsBinary)
    {
    for( unsigned int dim=0; dim < TInputImage::ImageDimension; dim++)
      {
      if( maxLength < size[ dim ] )
        {
        maxLength = size[ dim ];
        }
      }
    }
  else
    {
    for( unsigned int dim=0; dim < TInputImage::ImageDimension; dim++)
      {
      if( maxLength < size[ dim ]*spacing[dim] )
        {
        maxLength = size[ dim ]*spacing[dim];
        }
      }
    }



  itkDebugMacro(<< "allocating memory for K Distance Image");

  // prepare the k-distance image, all distances are set to infinity = 2*maxLength
  KDistanceImagePointer kdistanceImage = GetKDistanceMap();

  kdistanceImage->SetLargestPossibleRegion(
    inputImage->GetLargestPossibleRegion() );

  kdistanceImage->SetBufferedRegion(
    inputImage->GetBufferedRegion() );

  kdistanceImage->SetRequestedRegion(
    inputImage->GetRequestedRegion() );

  kdistanceImage->Allocate();



  itkDebugMacro(<< "allocating memory for K ID Image");
  // prepare the k-distance image, all distances are set to infinity = 2*maxLength
  KIDImagePointer kidImage = GetKclosestIDMap();

  kidImage->SetLargestPossibleRegion(
    inputImage->GetLargestPossibleRegion() );

  kidImage->SetBufferedRegion(
    inputImage->GetBufferedRegion() );

  kidImage->SetRequestedRegion(
    inputImage->GetRequestedRegion() );

  kidImage->Allocate();




  itkDebugMacro(<< "PrepareData: initialize the k-distance map  and  k-id map");

  ImageRegionConstIteratorWithIndex< TInputImage >  it( inputImage,  region );

  typename KIDImageType::PixelType idObject( m_K );
  typename KIDImageType::PixelType idBackground( m_K );
  idObject.Fill(-1);
  idBackground.Fill(-1);

  typename KDistanceImageType::PixelType distanceObect( m_K );
  typename KDistanceImageType::PixelType distanceBackground( m_K );
  distanceObect.Fill(2*maxLength);
  distanceObect[0] = 0;
  distanceBackground.Fill(2*maxLength);

  it.GoToBegin();
  int npt = 1;
  if (m_InputIsBinary)
  {
    while( !it.IsAtEnd() )
      {
      IndexType index = it.GetIndex();
      if( it.Get() )
        {
        kdistanceImage->SetPixel(index, distanceObect);
        idObject[0] = npt++;
        kidImage->SetPixel(index, idObject);
        m_IndexLookUpTable.push_back(index);
        }
      else
        {
        kdistanceImage->SetPixel(index, distanceBackground);
        kidImage->SetPixel(index, idBackground);
        }
      ++it;
      }
    }
  else // Input is not binary
    {
    typedef SortingElement<typename InputImageType::PixelType, 
                                                  typename InputImageType::IndexType>  Element;
    std::vector<Element> indices;
    
    while( !it.IsAtEnd() )
      {
      IndexType index = it.GetIndex();
      if( it.Get()>0 )
        {
        kdistanceImage->SetPixel(index, distanceObect);
        idObject[0] = static_cast< typename KIDPixelType::ValueType >( it.Get() );
        kidImage->SetPixel(index, idObject);
        Element  el;
        el.element = it.Get();
        el.index = index;
        indices.push_back( el );
        }
      else
        {
        kdistanceImage->SetPixel(index, distanceBackground);
        kidImage->SetPixel(index, idBackground);
        }
      ++it;
      }
     std::sort( indices.begin(), indices.end() );
     for (unsigned int kk=0; kk<indices.size(); kk++)    {
        m_IndexLookUpTable.push_back( indices[kk].index );
        }
    } // End If: Input is binary

  itkDebugMacro(<< "PrepareData End");
}




/**
 *  Post processing for computing the Voronoi Map
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::ComputeVoronoiMap()
{
  itkDebugMacro( << "ComputeVoronoiMap Start");
    
  typedef typename itk::ConnectedComponentVectorImageFilter<KIDImageType, OutputImageType> ConnectedComponentFilterType;
  typename ConnectedComponentFilterType::Pointer connectedCompFilter = ConnectedComponentFilterType::New();
  connectedCompFilter->SetInput( this->GetKclosestIDMap() );
  
  connectedCompFilter->UpdateLargestPossibleRegion();
  
  this->GraftNthOutput( 2, connectedCompFilter->GetOutput() );
}





/**
 *  Locally update the distance.
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::UpdateLocalDistance(const IndexType& here,
                      const OffsetType& offset)
{

  IndexType  there            = here + offset;
  typename KDistanceImageType::PixelType kd = m_KDistanceImage->GetPixel(here);
  typename KIDImageType::PixelType       kid_here = m_KIDImage->GetPixel(here);
  typename KIDImageType::PixelType       kid_there = m_KIDImage->GetPixel(there);

  for (unsigned int j=0; j<m_K; j++)
    {
    // instead of this (using distance components), use ID image and m_IndexLookUpTable
    if (kid_there[j]>-1)
      {
      IndexType objectIndex = m_IndexLookUpTable[kid_there[j]-1];
      OffsetType offsetToObject = objectIndex - here;

      typename InputImageType::SpacingType spacing = Self::GetInput()->GetSpacing();

      double sqdist = 0.0;
      for( unsigned int i=0; i<InputImageDimension; i++ )
        {
        double v1 = static_cast< double >(  offsetToObject[i]  );

        if (m_UseImageSpacing)
          {
          double spacingComponent = static_cast< double >(spacing[i]);
          v1 *= spacingComponent;
          }

        sqdist +=  v1 * v1;
        }

      if ( !m_SquaredDistance ) {
        InsertSorted(std::sqrt(sqdist), kid_there[j], kd, kid_here );
        } 
      else {
        InsertSorted(sqdist, kid_there[j], kd, kid_here );
        }
      }
    }    

}



/**
 *  Add element (distance and index) to list of nearest neighbors.
 *  Inserts so distances are sorted.
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
bool
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::InsertSorted( KDistanceValueType dist, KIDValueType index, KDistancePixelType& distances, KIDPixelType& indices)
{

  // Test if distance is larger than largest distance
  if (dist >= distances[distances.GetSize()-1])
    return false; // did not insert an element


  // Test if id is already in list, must replace it and re-sort the list.
  bool isalreadyinlist = false;
  int posinlist;
  for (unsigned int k=0; k<m_K; k++)
  {
    if (index==indices[k])
      {
      // Test if the distance is actually smaller, if not do not add to list
      if (dist<distances[k])
        {
        isalreadyinlist = true;
        posinlist = k;
        break;
      }
      else
        {
        return false;
        }
      }
  }

  if (isalreadyinlist)
  {
    std::cout << "ID is already in list. This is an incorrect behaviour." <<std::endl;
    exit(1);
  }
  else
  {
    int insertpos = distances.GetSize();
    for (int i=0; i<distances.GetSize(); i++) {
      if (dist < distances[i]) {
        insertpos = i;
        break;
      }
    }
    if (insertpos<distances.GetSize()) {
      for (unsigned int k = distances.GetSize()-1; k>insertpos; k--) {
        distances[k] = distances[k-1];
        indices[k] = indices[k-1];
      }
      distances[insertpos] = dist;
      indices[insertpos] = index;
    }
  }
  return true; // did insert an element
}


/**
 *  Compute Distance and Voronoi maps
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::GenerateData()
{

  std::cerr << "to here inside 1";
  this->PrepareData();
  
  // Specify images and regions.

  InputImagePointer    inputimage             =  this->GetInput();

  m_KDistanceImage    =  this->GetKDistanceMap();
  m_KIDImage          =  this->GetKclosestIDMap();

  std::cerr << "to here inside 2";

  typename InputImageType::RegionType region  = inputimage->GetLargestPossibleRegion();
  
  itkDebugMacro (<< "Region to process: " << region);

  // Instantiate reflective iterator

  ReflectiveImageRegionConstIterator< const InputImageType > 
    it( inputimage, region );
  it.FillOffsets(1); // what exactly is happening here?

  it.GoToBegin();

  // Support progress methods/callbacks.

  // Each pixel is visited 2^InputImageDimension times, and the number
  // of visits per pixel needs to be computed for progress reporting.
  unsigned long visitsPerPixel = (1 << InputImageDimension);
  unsigned long updateVisits, i=0;
  updateVisits = region.GetNumberOfPixels() * visitsPerPixel / 10;
  if ( updateVisits < 1 )
    {
    updateVisits = 1;
    }
  const float updatePeriod = static_cast<float>(updateVisits) * 10.0;

  // Process image.
  
  OffsetType  offset;
  offset.Fill( 0 );

std::cerr << "to here inside 3";
  itkDebugMacro(<< "GenerateData: Computing distance transform");
  while( !it.IsAtEnd() )
    {
    if ( !(i % updateVisits ) )
      {
      this->UpdateProgress( (float) i / updatePeriod );
      }

    IndexType here = it.GetIndex();
    for(unsigned int dim=0; dim <OutputImageType::ImageDimension; dim++)
      {
      if( it.IsReflected(dim) )
        {
        offset[dim]++;
        UpdateLocalDistance( here, offset );
        offset[dim]=0;
        }
      else
        {
        offset[dim]--;
        UpdateLocalDistance( here, offset );
        offset[dim]=0;
        }
      }
    ++it;
    ++i;
    }

  itkDebugMacro(<< "GenerateData: ComputeVoronoiMap");
  std::cerr << "to here inside 4";
  this->ComputeVoronoiMap();
std::cerr << "to here inside 5";
} // end GenerateData()



template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void 
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if ( !this->GetInput() )
    {
    return;
    }

  // get pointers to the input and output
  typename InputImageType::Pointer  inputPtr  = 
    const_cast< TInputImage *>( this->GetInput() );

  // Request the entire input image
  typename InputImageType::RegionType inputRegion;
  inputRegion = inputPtr->GetLargestPossibleRegion();
  inputPtr->SetLargestPossibleRegion(inputRegion);
  inputPtr->SetRequestedRegion(inputRegion);

  return;
}



/**
 *  Print Self
 */
template <class TInputImage, class TOutputImage, class TKDistanceImage, class TKIDImage >
void 
OrderKDistanceTransformImageFilter<TInputImage, TOutputImage, TKDistanceImage, TKIDImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Order K Distance Transform: " << std::endl;
  os << indent << "Input Is Binary   : " << m_InputIsBinary << std::endl;
  os << indent << "Use Image Spacing : " << m_UseImageSpacing << std::endl;
  os << indent << "Squared Distance  : " << m_SquaredDistance << std::endl;

}



} // end namespace itk

#endif
