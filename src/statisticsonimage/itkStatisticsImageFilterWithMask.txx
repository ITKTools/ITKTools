/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsImageFilterWithMask.txx,v $
  Language:  C++
  Date:      $Date: 2006-07-18 12:47:09 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkStatisticsImageFilter_txx
#define _itkStatisticsImageFilter_txx

#include "itkStatisticsImageFilterWithMask.h"

#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"


namespace itk {

template<class TInputImage>
StatisticsImageFilter<TInputImage>
::StatisticsImageFilter(): m_ThreadSum(1), m_SumOfSquares(1), m_Count(1), m_ThreadMin(1), m_ThreadMax(1)
{
  // first output is a copy of the image, DataObject created by
  // superclass
  //
  // allocate the data objects for the outputs which are
  // just decorators around pixel types
  for ( int i = 1; i < 3; ++i )
  {
    typename PixelObjectType::Pointer output
      = static_cast<PixelObjectType*>( this->MakeOutput(i).GetPointer() );
    this->ProcessObject::SetNthOutput( i, output.GetPointer() );
  }
  // allocate the data objects for the outputs which are
  // just decorators around real types
  for ( int i = 3; i < 7; ++i )
  {
    typename RealObjectType::Pointer output
      = static_cast<RealObjectType*>( this->MakeOutput(i).GetPointer() );
    this->ProcessObject::SetNthOutput( i, output.GetPointer() );
  }

  this->GetMinimumOutput()->Set( NumericTraits<PixelType>::max() );
  this->GetMaximumOutput()->Set( NumericTraits<PixelType>::NonpositiveMin() );
  this->GetMeanOutput()->Set( NumericTraits<RealType>::max() );
  this->GetSigmaOutput()->Set( NumericTraits<RealType>::max() );
  this->GetVarianceOutput()->Set( NumericTraits<RealType>::max() );
  this->GetSumOutput()->Set( NumericTraits<RealType>::Zero );

  this->m_Mask = 0;
}


template<class TInputImage>
DataObject::Pointer
StatisticsImageFilter<TInputImage>
::MakeOutput(unsigned int output)
{
  switch ( output )
    {
   case 0:
      return static_cast<DataObject*>(TInputImage::New().GetPointer());
      break;
    case 1:
      return static_cast<DataObject*>(PixelObjectType::New().GetPointer());
      break;
    case 2:
      return static_cast<DataObject*>(PixelObjectType::New().GetPointer());
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      return static_cast<DataObject*>(RealObjectType::New().GetPointer());
      break;
    default:
      // might as well make an image
      return static_cast<DataObject*>(TInputImage::New().GetPointer());
      break;
    }
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::PixelObjectType*
StatisticsImageFilter<TInputImage>
::GetMinimumOutput()
{
  return static_cast<PixelObjectType*>(this->ProcessObject::GetOutput(1));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::PixelObjectType*
StatisticsImageFilter<TInputImage>
::GetMinimumOutput() const
{
  return static_cast<const PixelObjectType*>(this->ProcessObject::GetOutput(1));
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::PixelObjectType*
StatisticsImageFilter<TInputImage>
::GetMaximumOutput()
{
  return static_cast<PixelObjectType*>(this->ProcessObject::GetOutput(2));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::PixelObjectType*
StatisticsImageFilter<TInputImage>
::GetMaximumOutput() const
{
  return static_cast<const PixelObjectType*>(this->ProcessObject::GetOutput(2));
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetMeanOutput()
{
  return static_cast<RealObjectType*>(this->ProcessObject::GetOutput(3));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetMeanOutput() const
{
  return static_cast<const RealObjectType*>(this->ProcessObject::GetOutput(3));
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetSigmaOutput()
{
  return static_cast<RealObjectType*>(this->ProcessObject::GetOutput(4));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetSigmaOutput() const
{
  return static_cast<const RealObjectType*>(this->ProcessObject::GetOutput(4));
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetVarianceOutput()
{
  return static_cast<RealObjectType*>(this->ProcessObject::GetOutput(5));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetVarianceOutput() const
{
  return static_cast<const RealObjectType*>(this->ProcessObject::GetOutput(5));
}


template<class TInputImage>
typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetSumOutput()
{
  return static_cast<RealObjectType*>(this->ProcessObject::GetOutput(6));
}

template<class TInputImage>
const typename StatisticsImageFilter<TInputImage>::RealObjectType*
StatisticsImageFilter<TInputImage>
::GetSumOutput() const
{
  return static_cast<const RealObjectType*>(this->ProcessObject::GetOutput(6));
}

template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    InputImagePointer image =
      const_cast< typename Superclass::InputImageType * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::EnlargeOutputRequestedRegion(DataObject *data)
{
  Superclass::EnlargeOutputRequestedRegion(data);
  data->SetRequestedRegionToLargestPossibleRegion();
}


template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::AllocateOutputs()
{
  // Pass the input through as the output
  InputImagePointer image =
    const_cast< TInputImage * >( this->GetInput() );
  this->GraftOutput( image );

  // Nothing that needs to be allocated for the remaining outputs
}

template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::BeforeThreadedGenerateData( void )
{
  int numberOfThreads = this->GetNumberOfThreads();

  // Resize the thread temporaries
  m_Count.SetSize(numberOfThreads);
  m_SumOfSquares.SetSize(numberOfThreads);
  m_ThreadSum.SetSize(numberOfThreads);
  m_ThreadMin.SetSize(numberOfThreads);
  m_ThreadMax.SetSize(numberOfThreads);

  // Initialize the temporaries
  m_Count.Fill(NumericTraits<long>::Zero);
  m_ThreadSum.Fill(NumericTraits<RealType>::Zero);
  m_SumOfSquares.Fill(NumericTraits<RealType>::Zero);
  m_ThreadMin.Fill(NumericTraits<PixelType>::max());
  m_ThreadMax.Fill(NumericTraits<PixelType>::NonpositiveMin());

}

template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::AfterThreadedGenerateData( void )
{
  int i;
  long count;
  RealType sumOfSquares;

  int numberOfThreads = this->GetNumberOfThreads();

  PixelType minimum;
  PixelType maximum;
  RealType  mean;
  RealType  sigma;
  RealType  variance;
  RealType  sum;

  sum = sumOfSquares = NumericTraits<RealType>::Zero;
  count = 0;

  // Find the min/max over all threads and accumulate count, sum and
  // sum of squares
  minimum = NumericTraits<PixelType>::max();
  maximum = NumericTraits<PixelType>::NonpositiveMin();
  for( i = 0; i < numberOfThreads; i++)
    {
    count += m_Count[i];
    sum += m_ThreadSum[i];
    sumOfSquares += m_SumOfSquares[i];

    if (m_ThreadMin[i] < minimum)
      {
      minimum = m_ThreadMin[i];
      }
    if (m_ThreadMax[i] > maximum)
      {
      maximum = m_ThreadMax[i];
      }
    }
  // compute statistics
  mean = sum / static_cast<RealType>( count );

  // unbiased estimate
  variance = (sumOfSquares - (sum*sum / static_cast<RealType>(count)))
    / (static_cast<RealType>(count) - 1);
  // in case of numerical errors the variance might be <0.
  variance = vnl_math_max(0.0, variance);
  sigma = vcl_sqrt(variance);

  // Set the outputs
  this->GetMinimumOutput()->Set( minimum );
  this->GetMaximumOutput()->Set( maximum );
  this->GetMeanOutput()->Set( mean );
  this->GetSigmaOutput()->Set( sigma );
  this->GetVarianceOutput()->Set( variance );
  this->GetSumOutput()->Set( sum );
}

template<class TInputImage>
void
StatisticsImageFilter<TInputImage>
::ThreadedGenerateData( const RegionType& outputRegionForThread, int threadId )
{
  RealType realValue;
  PixelType value;
  // support progress methods/callbacks
  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

  if ( this->m_Mask.IsNull() )
  {
    ImageRegionConstIterator<TInputImage> it (this->GetInput(), outputRegionForThread);
    // do the work
    while (!it.IsAtEnd())
    {
        value = it.Get();
        realValue = static_cast<RealType>( value );
        if (value < m_ThreadMin[threadId])
        {
        m_ThreadMin[threadId] = value;
        }
        if (value > m_ThreadMax[threadId])
        {
        m_ThreadMax[threadId] = value;
        }

        m_ThreadSum[threadId] += realValue;
        m_SumOfSquares[threadId] += (realValue * realValue);
        m_Count[threadId]++;
        ++it;
        progress.CompletedPixel();
    } // end while
  } //end if
  else
  { //use a mask
    ImageRegionConstIterator< InputImageType > itIm(
      this->GetInput(), outputRegionForThread );
    ImageRegionConstIterator< MaskType > itMask(
      this->m_Mask, outputRegionForThread );
    itIm.GoToBegin();
    itMask.GoToBegin();
    // do the work
    while ( !itIm.IsAtEnd() )
    {
      if ( itMask.Value() )
        {
            value = itIm.Get();
            realValue = static_cast<RealType>( value );
            if (value < m_ThreadMin[threadId])
            {
            m_ThreadMin[threadId] = value;
            }
            if (value > m_ThreadMax[threadId])
            {
            m_ThreadMax[threadId] = value;
            }

            m_ThreadSum[threadId] += realValue;
            m_SumOfSquares[threadId] += (realValue * realValue);
            m_Count[threadId]++;
        }
        ++itIm; ++itMask;
        progress.CompletedPixel();
    } // end while
  } // end else

}

template <class TImage>
void
StatisticsImageFilter<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Minimum: "
     << static_cast<typename NumericTraits<PixelType>::PrintType>(this->GetMinimum()) << std::endl;
  os << indent << "Maximum: "
     << static_cast<typename NumericTraits<PixelType>::PrintType>(this->GetMaximum()) << std::endl;
  os << indent << "Sum: "      << this->GetSum() << std::endl;
  os << indent << "Mean: "     << this->GetMean() << std::endl;
  os << indent << "Sigma: "    << this->GetSigma() << std::endl;
  os << indent << "Variance: " << this->GetVariance() << std::endl;
}


}// end namespace itk
#endif
