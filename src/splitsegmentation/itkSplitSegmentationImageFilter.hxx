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
#ifndef _itkSplitSegmentationImageFilter_hxx_
#define _itkSplitSegmentationImageFilter_hxx_

#include "itkSplitSegmentationImageFilter.h"

#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkProgressAccumulator.h"


namespace itk
{

/**
 * ********************* Constructor ****************************
 */

template <typename TInputImage, typename TOutputImage >
SplitSegmentationImageFilter<TInputImage,TOutputImage>
::SplitSegmentationImageFilter()
{
  /** Initialize variables. */
  this->m_NumberOfSplitsZ = 3;
  this->m_NumberOfSplitsY = 2;
  this->ResizeChunkLabels();
} // end Constructor


/**
 * ********************* SetNumberOfSplitsZ ****************************
 */

template <typename TInputImage, typename TOutputImage >
void
SplitSegmentationImageFilter<TInputImage,TOutputImage >
::SetNumberOfSplitsZ( const unsigned int &_v )
{
  if ( this->m_NumberOfSplitsZ != _v )
  {
    if( _v == 0 ) this->m_NumberOfSplitsZ = 1;
    else this->m_NumberOfSplitsZ = _v;

    this->ResizeChunkLabels();
    this->Modified();
  }
} // end SetNumberOfSplitsZ()


/**
 * ********************* SetNumberOfSplitsY ****************************
 */

template <typename TInputImage, typename TOutputImage >
void
SplitSegmentationImageFilter<TInputImage,TOutputImage >
::SetNumberOfSplitsY( const unsigned int &_v )
{
  if( this->m_NumberOfSplitsY != _v )
  {
    if( _v == 0 ) this->m_NumberOfSplitsY = 1;
    else this->m_NumberOfSplitsY = _v;

    this->ResizeChunkLabels();
    this->Modified();
  }
} // end SetNumberOfSplitsY()


/**
 * ********************* ResizeChunkLabels ****************************
 */

template <typename TInputImage, typename TOutputImage >
void
SplitSegmentationImageFilter<TInputImage,TOutputImage >
::ResizeChunkLabels()
{
  const unsigned int numLabels = this->m_NumberOfSplitsZ * this->m_NumberOfSplitsY;
  this->m_ChunkLabels.resize( numLabels );
  for( unsigned int i = 0; i < numLabels; ++i )
  {
    this->m_ChunkLabels[ i ] = static_cast<OutputPixelType>( i + 1 );
  }
} // end ResizeChunkLabels()


/**
 * ********************* SetChunkLabels ****************************
 */

template <typename TInputImage, typename TOutputImage >
void
SplitSegmentationImageFilter<TInputImage,TOutputImage >
::SetChunkLabels( const LabelType & labels )
{
  const unsigned int numLabels = this->m_NumberOfSplitsZ * this->m_NumberOfSplitsY;
  if( labels.size() != numLabels )
  {
    itkExceptionMacro( << "ERROR: You should provide " << numLabels
      << " labels, but you did provide " << labels.size() << " labels." );
  }

  this->m_ChunkLabels.resize( numLabels );
  for( unsigned int i = 0; i < numLabels; ++i )
  {
    this->m_ChunkLabels[ i ] = labels[ i ];
  }
  this->Modified();
} // end SetChunkLabels()


/**
 * ********************* GenerateData ****************************
 */

template <typename TInputImage, typename TOutputImage >
void
SplitSegmentationImageFilter<TInputImage,TOutputImage >
::GenerateData( void )
{
  /** Typedefs. */
  typedef ImageSliceConstIteratorWithIndex<
    InputImageType >                       InputSliceIteratorType;
  typedef ImageRegionConstIteratorWithIndex<
    InputImageType >                       InputIteratorType;
  typedef ImageRegionIterator<
    OutputImageType >                      OutputIteratorType;

  /** Get a pointer to the input. */
  InputImageConstPointer input = this->GetInput();

  /***/
  SizeType imageSize = input->GetLargestPossibleRegion().GetSize();
  std::vector<SizeValueType> sliceVolumeZ( imageSize[2] );

  /** Compute total volume of segmentation as the total number of nonzero voxels. */
  InputSliceIteratorType it( input, input->GetLargestPossibleRegion() );
  it.SetFirstDirection( 0 ); it.SetSecondDirection( 1 );
  it.GoToBegin();
  SizeValueType totalVolume = 0;
  SizeValueType sliceZ = 0;
  while( !it.IsAtEnd() )
  {
    SizeValueType tmp = 0;
    while( !it.IsAtEndOfSlice() )
    {
      while( !it.IsAtEndOfLine() )
      {
        if( it.Value() ) tmp++;
        ++it;
      }
      it.NextLine();
    }
    sliceVolumeZ[ sliceZ ] = tmp;
    totalVolume += tmp;
    it.NextSlice(); ++sliceZ;
  }

  /** Chunk size in the z direction. */
  const SizeValueType chunkSizeZ = Math::Round<SizeValueType>(
    static_cast<double>( totalVolume ) / this->m_NumberOfSplitsZ );

  /** Divide in chunks in the z direction. */
  std::vector<SizeValueType> startSliceNumberZ( this->m_NumberOfSplitsZ );
  std::vector<SizeValueType> endSliceNumberZ( this->m_NumberOfSplitsZ );
  std::vector<SizeValueType> chunkVolumeZ( this->m_NumberOfSplitsZ, 0 );
  SizeValueType chunkZ = 0;
  bool firstNonzeroVolumeFound = false;
  for( SizeValueType slice = 0; slice < sliceVolumeZ.size(); ++slice )
  {
    if( sliceVolumeZ[ slice ] == 0 ) continue;

    if( !firstNonzeroVolumeFound )
    {
      startSliceNumberZ[ 0 ] = slice;
      firstNonzeroVolumeFound = true;
    }

    chunkVolumeZ[ chunkZ ] += sliceVolumeZ[ slice ];
    endSliceNumberZ[ chunkZ ] = slice;

    if( chunkVolumeZ[ chunkZ ] > chunkSizeZ )
    {
      if( chunkZ != this->m_NumberOfSplitsZ - 1 )
      {
        ++chunkZ;
        startSliceNumberZ[ chunkZ ] = slice + 1;
      }
    }
  }

  /** Y direction. */
  RegionType region; IndexType index; SizeType size;
  std::vector<SizeValueType> sliceVolumeY( this->m_NumberOfSplitsZ * imageSize[ 1 ] );
  std::vector<SizeValueType> chunkSizeY( this->m_NumberOfSplitsZ );
  for( unsigned int chunk = 0; chunk < this->m_NumberOfSplitsZ; ++chunk )
  {
    size[ 0 ] = imageSize[ 0 ]; size[ 1 ] = imageSize[ 1 ];
    size[ 2 ] = endSliceNumberZ[ chunk ] - startSliceNumberZ[ chunk ] + 1;
    index[ 0 ] = 0; index[ 1 ] = 0; index[ 2 ] = startSliceNumberZ[ chunk ];
    region.SetIndex( index );
    region.SetSize( size );

    it = InputSliceIteratorType( input, region );
    it.SetFirstDirection( 0 ); it.SetSecondDirection( 2 );
    it.GoToBegin();
    sliceZ = 0;
    while( !it.IsAtEnd() )
    {
      SizeValueType tmp = 0;
      while( !it.IsAtEndOfSlice() )
      {
        while( !it.IsAtEndOfLine() )
        {
          if( it.Value() ) tmp++;
          ++it;
        }
        it.NextLine();
      }
      sliceVolumeY[ sliceZ + chunk * imageSize[ 1 ] ] = tmp;
      it.NextSlice(); ++sliceZ;
    }

    /** Chunk size in the y direction. */
    chunkSizeY[ chunk ] = Math::Round<SizeValueType>(
      static_cast<double>( chunkVolumeZ[ chunk ] ) / this->m_NumberOfSplitsY );
  }

  /** Divide in chunks in the y direction. */
  std::vector<SizeValueType> startSliceNumberY( this->m_NumberOfSplitsZ * this->m_NumberOfSplitsY );
  std::vector<SizeValueType> endSliceNumberY( this->m_NumberOfSplitsZ * this->m_NumberOfSplitsY );
  std::vector<SizeValueType> chunkVolumeY( this->m_NumberOfSplitsZ * this->m_NumberOfSplitsY, 0 );
  SizeValueType chunkY = 0;
  for( unsigned int chunk_z = 0; chunk_z < this->m_NumberOfSplitsZ; ++chunk_z )
  {
    chunkY = 0;
    SizeValueType chunkY2 = chunk_z * this->m_NumberOfSplitsY;
    bool firstNonzeroVolumeFound = false;
    for( SizeValueType slice_y = 0; slice_y < imageSize[ 1 ]; ++slice_y )
    {
      SizeValueType slice2 = slice_y + chunk_z * imageSize[ 1 ];
      if( sliceVolumeY[ slice2 ] == 0 ) continue;

      if( !firstNonzeroVolumeFound )
      {
        startSliceNumberY[ chunk_z * this->m_NumberOfSplitsY ] = slice_y;
        firstNonzeroVolumeFound = true;
      }

      chunkVolumeY[ chunkY2 ] += sliceVolumeY[ slice2 ];
      endSliceNumberY[ chunkY2 ] = slice_y;

      if( chunkVolumeY[ chunkY2 ] > chunkSizeY[ chunk_z ] )
      {
        if( chunkY != this->m_NumberOfSplitsY - 1 )
        {
          ++chunkY;
          chunkY2 = chunkY + chunk_z * this->m_NumberOfSplitsY;
          startSliceNumberY[ chunkY2 ] = slice_y + 1;
        }
      }
    }
  }

  /** Allocate output image. */
  OutputImagePointer output = this->GetOutput();
  output->SetRegions( input->GetRequestedRegion() );
  output->Allocate();
  output->FillBuffer( 0 );

  /** Create a process accumulator for tracking the progress of this minipipeline. *
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter( this );
  progress->RegisterInternalFilter(
      this->m_DerivativeFilters[ i ], 1.0 / ( ImageDimension + 1.0 ) );
*/

  /** Split the input segmentation. */
  InputIteratorType itIn( input, input->GetLargestPossibleRegion() );
  OutputIteratorType itOut( output, output->GetLargestPossibleRegion() );
  itIn.GoToBegin(); itOut.GoToBegin();
  unsigned int y, z; OutputPixelType label;
  while( !itIn.IsAtEnd() )
  {
    if( itIn.Value() == 0 )
    {
      ++itIn; ++itOut;
      continue;
    }

    index = itIn.GetIndex();

    // Determine z chunk
    for( unsigned int chunk_z = 0; chunk_z < this->m_NumberOfSplitsZ; ++chunk_z )
    {
      if( index[2] >= startSliceNumberZ[ chunk_z ] && index[2] <= endSliceNumberZ[ chunk_z ] )
      {
        z = chunk_z;
        break;
      }
    }

    // Determine y chunk
    for( unsigned int chunk_y = 0; chunk_y < this->m_NumberOfSplitsY; ++chunk_y )
    {
      unsigned int chunk_y2 = chunk_y + z * this->m_NumberOfSplitsY;
      if( index[1] >= startSliceNumberY[ chunk_y2 ] && index[1] <= endSliceNumberY[ chunk_y2 ] )
      {
        y = chunk_y;
        break;
      }
    }

    // Determine and set label
    label = static_cast<OutputPixelType>( this->m_ChunkLabels[ y + z * this->m_NumberOfSplitsY ] );
    itOut.Set( label );

    ++itIn; ++itOut;
  } // end while

} // end GenerateData()


/**
 * ********************* PrintSelf ****************************
 */

template <typename TInputImage, typename TOutputImage>
void
SplitSegmentationImageFilter<TInputImage,TOutputImage>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << "NumberOfSplitsZ: " << this->m_NumberOfSplitsZ << std::endl;
  os << "NumberOfSplitsY: " << this->m_NumberOfSplitsY << std::endl;
  os << "ChunkLabels: [ ";
  for( std::size_t i = 0; i < this->m_ChunkLabels.size(); ++i )
  {
    os << this->m_ChunkLabels[ i ] << " ";
  }
  os << "]" << std::endl;
} // end PrintSelf()


} // end namespace itk

#endif // end #ifndef _itkSplitSegmentationImageFilter_hxx_
