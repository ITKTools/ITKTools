/// author: Bryn Lloyd
#ifndef _itkConnectedComponentVectorImageFilter_txx_
#define _itkConnectedComponentVectorImageFilter_txx_

#include "itkConnectedComponentVectorImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkEquivalencyTable.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"



namespace itk
{
template< class TInputImage, class TOutputImage >
void
ConnectedComponentVectorImageFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  InputImagePointer input = const_cast<InputImageType *>(this->GetInput());

  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}




/**
 * Sort an int Array
 */
template <class TInputImage,class TOutputImage>
typename ConnectedComponentVectorImageFilter< TInputImage, TOutputImage >::InputPixelType
ConnectedComponentVectorImageFilter<TInputImage, TOutputImage>
::SortArray( const InputPixelType vec)
{
   /*
   int this->m_K = this->GetInput()->GetVectorLength();
   Array<int> sortedVec( this->m_K);
   for( unsigned int k=0; k<m_K; k++)
     sortedVec[k] = vec[k];

   /// Shell sort: "Numerical Recipes in C", Second Edition, page 332
   unsigned int i,j,inc;
   int v;
   inc=1;
   do {
     inc *= 3;
     inc++;
   } while (inc<=m_K);
   do {
     inc /= 3;
     for (i=inc+1; i<=m_K; i++ ) {
       v=sortedVec[ i ];
       j=i;
       while (sortedVec[j-inc] > v) {
         sortedVec[j] = sortedVec[j-inc];
         j -= inc;
         if(j <= inc) break;
       }
       sortedVec[j]=v;
     }
   } while (inc>1);

   */
   int m_K = this->GetInput()->GetVectorLength();
   InputPixelType sortedVec( this->m_K);
   std::vector<int> stdvec;
   stdvec.resize( this->m_K);
   for( unsigned int k=0; k<m_K; k++)
     stdvec[k] = vec[k];
   std::sort(stdvec.begin(), stdvec.end());
   for( unsigned int k=0; k<m_K; k++)
     sortedVec[k] = stdvec[k];

   return sortedVec;
}



template <class TInputImage, class TOutputImage>
void
ConnectedComponentVectorImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}


template< class TInputImage, class TOutputImage >
void
ConnectedComponentVectorImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
  itkDebugMacro( << "ComputeVoronoiMap Start");

  // create an equivalency table
  EquivalencyTable::Pointer eqTable = EquivalencyTable::New();

  typedef typename OutputImageType::PixelType OutputPixelType;
  OutputPixelType    label, originalLabel, neighborLabel;
  OutputPixelType    maxLabel = NumericTraits<OutputPixelType>::Zero;
  const OutputPixelType maxPossibleLabel=NumericTraits<OutputPixelType>::max();

  OutputImagePointer    output          =  this->GetOutput();
  typename InputImageType::ConstPointer input = this->GetInput();

  // Allocate the output and initialize to zeros
  this->AllocateOutputs();
  output->FillBuffer( NumericTraits<OutputPixelType>::Zero );

  // Set up the boundary condition to be zero padded (used on output image)
  ConstantBoundaryCondition<TOutputImage> BC;
  BC.SetConstant(NumericTraits<OutputPixelType>::Zero);


  // Neighborhood iterator.  Let's use a shaped neighborhood so we can
  // restrict the access to face connected neighbors. This iterator
  // will be applied to the output image
  typedef ConstShapedNeighborhoodIterator<TOutputImage> NeighborhoodVoronoiImageIteratorType;
  SizeType kernelRadius;
  kernelRadius.Fill(1);
  NeighborhoodVoronoiImageIteratorType nit(kernelRadius, output,
                               output->GetRequestedRegion());
  nit.OverrideBoundaryCondition(&BC); // assign the boundary condition



  // only activate the indices that are "previous" to the current
  // pixel and face connected (exclude the center pixel from the
  // neighborhood)
  //
  unsigned int d;
  typename NeighborhoodVoronoiImageIteratorType::OffsetType offset;

  if( !m_FullyConnected)
    {
    // only activate the "previous" neighbors that are face connected
    // to the current pixel. do not include the center pixel
    offset.Fill(0);
    for (d=0; d < InputImageType::ImageDimension; ++d)
      {
      offset[d] = -1;
      nit.ActivateOffset(offset);
      offset[d] = 0;
      }
    }
  else
    {
    // activate all "previous" neighbors that are face+edge+vertex
    // connected to the current pixel. do not include the center pixel
    unsigned int centerIndex = nit.GetCenterNeighborhoodIndex();
    for (d=0; d < centerIndex; d++)
      {
      offset = nit.GetOffset(d);
      nit.ActivateOffset(offset);
      }
    }

  // along with a neighborhood iterator on the input, use a standard
  // iterator on the input and output
  ImageRegionConstIteratorWithIndex<InputImageType> it(input,
                                                input->GetRequestedRegion());
  ImageRegionIterator<OutputImageType> oit(output,
                                             output->GetRequestedRegion());


  // Setup a progress reporter.  We have 3 stages to the algorithm so
  // pretend we have 3 times the number of pixels
  ProgressReporter progress(this, 0,
                           2*output->GetRequestedRegion().GetNumberOfPixels());

  // Mark the output image as either background or unlabeled
  output->FillBuffer( maxPossibleLabel );

  // iterate over the image, labeling the objects and defining
  // equivalence classes.  Use the neighborhood iterator to access the
  // "previous" neighbor pixels and an output iterator to access the
  // current pixel
  it.GoToBegin();
  nit.GoToBegin();
  oit.GoToBegin();

  long counter=1;
  while ( !oit.IsAtEnd() )
    {
    counter++;
    // Get the current pixel label
    label = oit.Get();
    typename InputImageType::PixelType ids_here = it.Get();
    typename NeighborhoodVoronoiImageIteratorType::IndexType index_here = it.GetIndex();
    originalLabel = label;

      // loop over the "previous" neighbors to find labels.  this loop
      // may establish one or more new equivalence classes
      typename NeighborhoodVoronoiImageIteratorType::ConstIterator sIt;
      for (sIt = nit.Begin(); !sIt.IsAtEnd();  ++sIt)
        {

        // get the label of the pixel previous to this one along a
        // particular dimension (neighbors activated in neighborhood iterator)
        neighborLabel = sIt.Get();
        typename NeighborhoodVoronoiImageIteratorType::IndexType index_there = index_here + sIt.GetNeighborhoodOffset();

        // if the previous pixel has a label, verify equivalence or
        // establish a new equivalence
        bool IDsarethesame = true;
        if(neighborLabel != NumericTraits<OutputPixelType>::Zero) {
          typename InputImageType::PixelType ids_there;
          if(input->GetRequestedRegion().IsInside(index_there) ) {
            ids_there= input->GetPixel(index_there);
          }
          typename InputImageType::PixelType sorted_here = SortArray(ids_here);
          typename InputImageType::PixelType sorted_there = SortArray(ids_there);
/*          for( unsigned int ii=0; ii<input->GetVectorLength(); ii++)
            std::cout << sorted_here[ii] << " " << sorted_there[ii] <<"\t";
            std::cout <<std::endl;*/


          for( unsigned int ii=0; ii<input->GetVectorLength(); ii++)  {
            if(sorted_here[ii]!=sorted_there[ii]) {
              IDsarethesame=false;
//              std::cout << "No match" << std::endl;
              break;
              }
            }
          }
        else {
          IDsarethesame=false;
          }
        if(IDsarethesame) //&& neighborLabel != NumericTraits<OutputPixelType>::Zero)
          {
          // if current pixel is unlabeled, copy the label from neighbor
          if(label == maxPossibleLabel)  /// && vec here == vec there?
            {
            // copy the label from the previous pixel
            label = neighborLabel;
            }
          // else if current pixel has a label that is not already
          // equivalent to the label of the previous pixel, then setup
          // a new equivalence.
          else if((label != neighborLabel) /// && vec here == vec there?
                   && (eqTable->RecursiveLookup(label)
                       != eqTable->RecursiveLookup(neighborLabel)))
            {
            eqTable->Add(label, neighborLabel);
            }
          }

        }
      // if none of the "previous" neighbors were set, then make a new label
      if(originalLabel == label)
        {
        // create a new entry label
        if(maxLabel == maxPossibleLabel)
          {
          itkWarningMacro(<< "ConnectedComponentVectorImageFilter::GenerateData: Number of labels exceeds number of available labels for the output type." );
          }
        else
          {
          ++maxLabel;
          }

        // assign the new label
        label = maxLabel;
        }

      // Finally, set the output pixel to whatever label we have
      if(label != originalLabel)
        {
        oit.Set( label );
        }

    // move the iterators
    ++nit;
    ++oit;
    ++it;
    progress.CompletedPixel();
    }

  // Flatten the equavalency table
  eqTable->Flatten();

  // remap the labels
  oit.GoToBegin();
  while ( !oit.IsAtEnd() )
    {
    label = oit.Get();
    // if pixel has a label, write out the final equivalence
    if(1) /// label != NumericTraits<OutputPixelType>::Zero)
      {
      oit.Set( eqTable->Lookup( label ) );
//      std::cout << label << " eq table label: " << eqTable->Lookup( label ) << std::endl;
      }
    ++oit;
    progress.CompletedPixel();
    }
}

template< class TInputImage, class TOutputImage >
void
ConnectedComponentVectorImageFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "FullyConnected: "  << this->m_FullyConnected << std::endl;
}

} // end namespace itk

#endif // end #ifndef _itkConnectedComponentVectorImageFilter_txx_
