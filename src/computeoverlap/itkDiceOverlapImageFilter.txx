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
#ifndef _itkDiceOverlapImageFilter_txx_
#define _itkDiceOverlapImageFilter_txx_

#include "itkDiceOverlapImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkProgressReporter.h"


namespace itk
{

/**
 * ******************* Constructor *******************
 */

template <typename TInputImage>
DiceOverlapImageFilter<TInputImage>
::DiceOverlapImageFilter()
{
  /** Initialize variables. */
  //this->m_RequestedLabels = 0;

  this->SetNumberOfRequiredInputs( 2 );
  this->m_MeanLabelOverlap = 0.0;

} // end Constructor


/**
 * ******************* BeforeThreadedGenerateData *******************
 */

template<class TInputImage>
void
DiceOverlapImageFilter<TInputImage>
::BeforeThreadedGenerateData( void )
{
  const int numberOfThreads = this->GetNumberOfWorkUnits();

  // Create the thread temporaries
  this->m_SumAForThread.resize( numberOfThreads );
  this->m_SumBForThread.resize( numberOfThreads );
  this->m_SumCForThread.resize( numberOfThreads );

} // end BeforeThreadedGenerateData()


/**
 * ******************* ThreadedGenerateData *******************
 */

template <typename TInputImage>
void
DiceOverlapImageFilter<TInputImage>
::ThreadedGenerateData(
  const InputImageRegionType & inputRegionForThread,
  ThreadIdType threadId )
{
  typedef itk::ImageRegionConstIterator<InputImageType>    IteratorType;

  /** Create a process reporter for tracking the progress of this filter. */
  ProgressReporter progress( this, threadId, inputRegionForThread.GetNumberOfPixels() );

  /** Create iterators. */
  IteratorType itA( this->GetInput( 0 ), inputRegionForThread );
  IteratorType itB( this->GetInput( 1 ), inputRegionForThread );
  itA.GoToBegin();
  itB.GoToBegin();

  /** Determine size of objects, and size in the overlap. */
  OverlapMapType sumA, sumB, sumC;
  while ( !itA.IsAtEnd() )
  {
    InputPixelType A = itA.Value();
    InputPixelType B = itB.Value();

    sumA[ A ]++;
    sumB[ B ]++;
    if( A == B  ) ++sumC[ A ];

    /** Increase iterators. */
    ++itA; ++itB;
    progress.CompletedPixel(); // potential exception thrown here

  } // end while

  /** Update sums for this thread. */
  this->m_SumAForThread[ threadId ] = sumA;
  this->m_SumBForThread[ threadId ] = sumB;
  this->m_SumCForThread[ threadId ] = sumC;

} // end ThreadedGenerateData()


/**
 * ******************* AfterThreadedGenerateData *******************
 */

template<class TInputImage>
void
DiceOverlapImageFilter<TInputImage>
::AfterThreadedGenerateData( void )
{
  /** Merge sums from all threads. */
  OverlapMapType sumA = this->m_SumAForThread[ 0 ];
  OverlapMapType sumB = this->m_SumBForThread[ 0 ];
  OverlapMapType sumC = this->m_SumCForThread[ 0 ];
  typename OverlapMapType::const_iterator  it;
  for( unsigned int threadId = 1; threadId < this->GetNumberOfWorkUnits(); threadId++ )
  {
    for( it = this->m_SumAForThread[ threadId ].begin(); it != this->m_SumAForThread[ threadId ].end(); ++it )
    {
      if( sumA.count( (*it).first ) == 0 )
      {
        sumA[ (*it).first ] = (*it).second;
      }
      else
      {
        sumA[ (*it).first ] += (*it).second;
      }
    }

    for( it = this->m_SumBForThread[ threadId ].begin(); it != this->m_SumBForThread[ threadId ].end(); ++it )
    {
      if( sumB.count( (*it).first ) == 0 )
      {
        sumB[ (*it).first ] = (*it).second;
      }
      else
      {
        sumB[ (*it).first ] += (*it).second;
      }
    }

    for( it = this->m_SumCForThread[ threadId ].begin(); it != this->m_SumCForThread[ threadId ].end(); ++it )
    {
      if( sumC.count( (*it).first ) == 0 )
      {
        sumC[ (*it).first ] = (*it).second;
      }
      else
      {
        sumC[ (*it).first ] += (*it).second;
      }
    }
  }

  /** Calculate the Dice overlaps. */
  std::size_t numberOfLabels = 0;
  for( it = sumA.begin(); it != sumA.end(); ++it )
  {
    InputPixelType currentLabel = (*it).first;

    /** Compute overlap. */
    const std::size_t sumAB = sumA[ currentLabel ] + sumB[ currentLabel ];
    if( sumAB == 0 )
    {
      this->m_DiceOverlap[ currentLabel ] = 0.0;
    }
    else
    {
      this->m_DiceOverlap[ currentLabel ]
        = static_cast<double>( 2 * sumC[ currentLabel ] )
        / static_cast<double>( sumAB );
    }

    this->m_SumA[ currentLabel ] = sumA[ currentLabel ];
    this->m_SumB[ currentLabel ] = sumB[ currentLabel ];
    this->m_SumC[ currentLabel ] = sumC[ currentLabel ];

    if( currentLabel > 0 )
    {
      numberOfLabels++;
      this->m_MeanLabelOverlap += this->m_DiceOverlap[ currentLabel ];
    }
  } // end loop over all labels

  if( numberOfLabels > 0 )
  {
    this->m_MeanLabelOverlap /= numberOfLabels;
  }
} // end AfterThreadedGenerateData()


/**
 * ******************* PrintRequestedDiceOverlaps *******************
 */

template<class TInputImage>
void
DiceOverlapImageFilter<TInputImage>
::PrintRequestedDiceOverlaps( void )
{
  /** Check if all requested labels exist. */
  for ( typename LabelsType::const_iterator itL = this->m_RequestedLabels.begin();
    itL != this->m_RequestedLabels.end(); itL++ )
  {
    //if( sumA.count( *itL ) == 0 && sumB.count( *itL ) == 0 )
    if( this->m_DiceOverlap.count( *itL ) == 0 )
    {
      itkExceptionMacro( << "The selected label "
        << static_cast<std::size_t>( *itL )
        << " does not exist in both input images." );
    }
  }

  /** Print the requested Dice overlaps. */
  std::cout << "label => sum input1 \t, sum input2 \t, sum overlap \t, overlap" << std::endl;
  //std::cout << "label => overlap" << std::endl;
  typename OverlapMapRealType::const_iterator  it;
  for ( it = this->m_DiceOverlap.begin() ; it != this->m_DiceOverlap.end(); it++ )
  {
    InputPixelType currentLabel = (*it).first;

    /** Skip the current label if not selected by user.
     * Print all labels if nothing is selected.
     */
    if( this->m_RequestedLabels.size() != 0
      && this->m_RequestedLabels.count( currentLabel ) == 0 )
    {
      continue;
    }

    /** Print current label. */
    std::cout << static_cast<std::size_t>( currentLabel ) << " => "
      << this->m_SumA[ currentLabel ]
      << "\t, " << this->m_SumB[ currentLabel ]
      << "\t, " << this->m_SumC[ currentLabel ]
      << "\t, "
      << this->m_DiceOverlap[ currentLabel ] << std::endl;

  } // end loop over all labels

    std::cout << "Mean overlap (exclude label 0)  =>  "
      << this->m_MeanLabelOverlap << std::endl;

} // end PrintRequestedDiceOverlaps()


/**
 * ******************* PrintSelf *******************
 */

template <typename TInputImage>
void
DiceOverlapImageFilter<TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  //os << indent << "RequestedLabels: " << this->m_RequestedLabels << std::endl;
  //os << indent << "Sum A: " << this->m_SumAForThread << std::endl;
  //os << indent << "Sum B: " << this->m_SumBForThread << std::endl;
  //os << indent << "Sum C: " << this->m_SumCForThread << std::endl;

  //LabelsType                    m_RequestedLabels;
  //std::vector<OverlapMapType>   m_SumAForThread;
  //std::vector<OverlapMapType>   m_SumBForThread;
  //std::vector<OverlapMapType>   m_SumCForThread;
  //OverlapMapRealType            m_SumA;
  //OverlapMapRealType            m_SumB;
  //OverlapMapRealType            m_SumC;
  //OverlapMapRealType            m_DiceOverlap;
  //double m_TotalOverlap;

} // end PrintSelf()


} // end namespace itk


#endif // end #ifndef _itkDiceOverlapImageFilter_txx_
