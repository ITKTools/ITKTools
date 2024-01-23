/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef _itkHistogramEqualizationImageFilter_hxx_
#define _itkHistogramEqualizationImageFilter_hxx_

#include "itkHistogramEqualizationImageFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"


namespace itk {

template<class TImage>
HistogramEqualizationImageFilter<TImage>
::HistogramEqualizationImageFilter()
{
  this->m_Min = itk::NumericTraits<InputImagePixelType>::max();
  this->m_Max = itk::NumericTraits<InputImagePixelType>::NonpositiveMin();
  this->m_MeanFrequency = 1.0;
  this->m_NumberOfBins = 1;
}

template<class TImage>
HistogramEqualizationImageFilter<TImage>
::~HistogramEqualizationImageFilter()
{
}

template<class TImage>
void
HistogramEqualizationImageFilter<TImage>
::BeforeThreadedGenerateData( void )
{
  typedef ImageRegionConstIterator<InputImageType>   ImageIteratorType;
  typedef ImageRegionConstIterator<MaskImageType>    MaskIteratorType;

  /** Use a mask or not */
  bool useMask = false;
  if( this->GetMask() )
  {
    useMask = true;
  }

  /** Compute minimum and maximum of the input image */
  ImageIteratorType it( this->GetInput(), this->GetOutput()->GetRequestedRegion() );
  MaskIteratorType maskIt;
  if( useMask )
  {
    maskIt = MaskIteratorType( this->GetMask(), this->GetOutput()->GetRequestedRegion() );
    maskIt.GoToBegin();
  }

  InputImagePixelType tempmin = itk::NumericTraits<InputImagePixelType>::max();
  InputImagePixelType tempmax =
    itk::NumericTraits<InputImagePixelType>::NonpositiveMin();

  unsigned long numberOfValidPixels = 0;
  it.GoToBegin();
  while ( !it.IsAtEnd() )
  {
    bool validPixel = true;
    if( useMask )
    {
      validPixel = static_cast<bool>( maskIt.Value() );
      ++maskIt;
    }
    if( validPixel )
    {
      ++numberOfValidPixels;
      const InputImagePixelType & current = it.Value();
      if( current < tempmin )
      {
        tempmin = current;
      }
      if( current > tempmax )
      {
        tempmax = current;
      }
    }
    ++it;
  }

  this->m_Min = tempmin;
  this->m_Max = tempmax;

  /** Compute the number of bins and the ideal number of times a intensity value
   * should occur in the image */
  this->m_NumberOfBins = tempmax - tempmin + 1;
  this->m_MeanFrequency =
    static_cast<double>( numberOfValidPixels ) /
    static_cast<double>( this->m_NumberOfBins );

  /** Compute the histogram of the input image */
  typedef itk::Array<unsigned long> HistogramType;
  HistogramType hist( this->m_NumberOfBins );
  hist.Fill( 0 );
  it.GoToBegin();
  if( useMask )
  {
    maskIt.GoToBegin();
  }
  while ( !it.IsAtEnd() )
  {
    bool validPixel = true;
    if( useMask )
    {
      validPixel = static_cast<bool>( maskIt.Value() );
      ++maskIt;
    }
    if( validPixel )
    {
      // assuming integer pixel type of binsize 1
      hist[ static_cast<unsigned int>( it.Value() - tempmin ) ]++;
    }
    ++it;
  }

  /** convert it to a cumulative histogram */
  for( unsigned int i = 1; i < this->m_NumberOfBins; i++ )
  {
    hist[ i ] += hist[i-1];
  }

  /** Compute LUT */
  this->m_LUT.SetSize(this->m_NumberOfBins);
  for( unsigned int i = 0; i < this->m_NumberOfBins; i++ )
  {
    this->m_LUT[ i ] = static_cast<OutputImagePixelType>(
      std::max(
      static_cast<double>( tempmin ),
      -1.0 + tempmin + std::floor( static_cast<double>( hist[ i ] ) / this->m_MeanFrequency + 0.5 ) ) );
  }

} // end BeforeThreadedGenerateData()


template<class TImage>
void
HistogramEqualizationImageFilter<TImage>
::AfterThreadedGenerateData( void )
{
  //nothing
} // end AfterThreadedGenerateData()


template<class TImage>
void
HistogramEqualizationImageFilter<TImage>
::ThreadedGenerateData(
  const OutputImageRegionType & outputRegionForThread,
  ThreadIdType threadId )
{
  typedef ImageRegionConstIterator<InputImageType>   InputImageIteratorType;
  typedef ImageRegionIterator<OutputImageType>       OutputImageIteratorType;
  typedef ImageRegionConstIterator<MaskImageType>    MaskIteratorType;

  /** Use a mask or not */
  bool useMask = false;
  if( this->GetMask() ) useMask = true;

  InputImageIteratorType  it( this->GetInput(), outputRegionForThread );
  OutputImageIteratorType ot( this->GetOutput(), outputRegionForThread );
  MaskIteratorType maskIt;
  if( useMask )
  {
    maskIt = MaskIteratorType( this->GetMask(), outputRegionForThread );
    maskIt.GoToBegin();
  }

  // support progress methods/callbacks
  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

  LUTType & lut = this->m_LUT;
  InputImagePixelType & tempmin = this->m_Min;

  // shift and scale the input pixels
  while( !it.IsAtEnd() )
  {
    bool validPixel = true;
    if( useMask )
    {
      validPixel = static_cast<bool>( maskIt.Value() );
      ++maskIt;
    }
    if( validPixel )
    {
      ot.Set( lut[ static_cast<unsigned int>( it.Value() - tempmin ) ] );
    }
    else
    {
      ot.Set( it.Value() ); // original?
    }
    ++it;
    ++ot;
    progress.CompletedPixel();
  }
} // end ThreadedGenerateData()


template <class TImage>
void
HistogramEqualizationImageFilter<TImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "NumberOfBins: "  << this->m_NumberOfBins << std::endl;
  os << indent << "Minimum intensity: "  << this->m_Min << std::endl;
  os << indent << "Maximum intensity: "  << this->m_Max << std::endl;

} // end PrintSelf()


}// end namespace itk

#endif
