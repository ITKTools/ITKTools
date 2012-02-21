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
#ifndef _itkReshapeImageToImageFilter_txx_
#define _itkReshapeImageToImageFilter_txx_

#include "itkReshapeImageToImageFilter.h"

namespace itk
{

/**
 * ********************* Constructor ****************************
 */

template< class TInputImage >
ReshapeImageToImageFilter< TInputImage >
::ReshapeImageToImageFilter( )
{
  this->m_OutputSize.Fill( NumericTraits<
    typename SizeType::SizeValueType>::Zero );

} // end Constructor()


/**
 * ********************* GenerateOutputInformation ****************************
 */

template< class TInputImage >
void
ReshapeImageToImageFilter< TInputImage >
::GenerateOutputInformation( void )
{
  // call the superclass's implementation of this method
  this->Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  ImagePointer inputPtr = const_cast< TInputImage * >( this->GetInput() );
  ImagePointer outputPtr = this->GetOutput();

  if( !inputPtr || !outputPtr )
  {
    return;
  }

  unsigned long numVoxelsOutput = 1;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    numVoxelsOutput *= this->m_OutputSize[ i ];
  }

  // Check if the output region was set
  if( numVoxelsOutput == 0 )
  {
    itkExceptionMacro( << "ERROR: You have to specify the output size." );
  }
//
//   // Check if reshaping is possible
//   // \todo: Could be less strict append with 0's or just take a part
//   if( numPixelsInput != numPixelsOutput )
//   {
//     itkExceptionMacro( << "ERROR: input region and output region should have "
//       << "the same number of voxels." );
//   }

  outputPtr->SetRegions( this->m_OutputSize );

} // end GenerateOutputInformation()


/**
 * ********************* GenerateInputRequestedRegion ****************************
 */

template< class TInputImage>
void
ReshapeImageToImageFilter< TInputImage>
::GenerateInputRequestedRegion( void )
{
} // end GenerateInputRequestedRegion()


/**
 * ********************* GenerateData ****************************
 */

template< class TInputImage >
void
ReshapeImageToImageFilter< TInputImage >
::GenerateData( void )
{
  /** Get handles to the input and output. */
  ImageConstPointer input = this->GetInput();
  ImagePointer output = this->GetOutput();

  /** Allocate memory. */
  output->Allocate();
  output->FillBuffer( NumericTraits<ImagePixelType>::Zero );

  /** Get the number of pixels. */
  unsigned long numVoxelsInput = input->GetLargestPossibleRegion().GetNumberOfPixels();
  unsigned long numVoxelsOutput = 1;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    numVoxelsOutput *= this->m_OutputSize[ i ];
  }
  unsigned long minVoxels = numVoxelsInput < numVoxelsOutput ? numVoxelsInput : numVoxelsOutput;

  /** Copy pixels. */
  memcpy( output->GetBufferPointer(), input->GetBufferPointer(),
    sizeof( ImagePixelType ) * minVoxels );

} // end GenerateData()


/**
* ********************* PrintSelf ****************************
*/

template < class TInputImage >
void
ReshapeImageToImageFilter< TInputImage >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  /** Call the superclass implementation. */
  Superclass::PrintSelf( os, indent );

  /** Print the member variables. */
  os << indent << "OutputSize: " << this->m_OutputSize << std::endl;

} // end PrintSelf()


} // namespace itk

#endif // end #ifndef _itkReshapeImageToImageFilter_txx
