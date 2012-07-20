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
#ifndef __invertintensityimagefilter_h_
#define __invertintensityimagefilter_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkStatisticsImageFilter.h"
#include "itkInvertIntensityImageFilter.h"

// Vector image support
#include "itkVectorIndexSelectionCastImageFilter.h" // decompose
#include "itkComposeImageFilter.h" // reassemble
#include "itkChannelByChannelVectorImageFilter2.h"


/** \class ITKToolsInvertIntensityBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsInvertIntensityBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsInvertIntensityBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsInvertIntensityBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end class ITKToolsInvertIntensityBase


/** \class ITKToolsLogicalImageOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsInvertIntensity : public ITKToolsInvertIntensityBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsInvertIntensity Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsInvertIntensity(){};
  ~ITKToolsInvertIntensity(){};

  /** Run function. */
  void Run( void )
  {
    /** Some typedef's. */
    typedef itk::Image<TComponentType, VDimension>              ScalarImageType;
    typedef typename ScalarImageType::PixelType                 InputPixelType;
    typedef itk::VectorImage< InputPixelType,
      ScalarImageType::ImageDimension >                         VectorImageType;
    typedef itk::ImageFileReader< VectorImageType >             ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >             WriterType;
    typedef itk::StatisticsImageFilter< ScalarImageType >       StatisticsFilterType;
    typedef typename StatisticsFilterType::RealType             RealType;
    typedef itk::InvertIntensityImageFilter< ScalarImageType >  InvertIntensityFilterType;

    /** Create reader. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );

    // In this case, we must manually disassemble the image rather than use a
    // ChannelByChannel filter because the image is not the output,
    // but rather the GetMaximum() function is what we want.

    // Create the disassembler
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexSelectionType;
    typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
    indexSelectionFilter->SetInput( reader->GetOutput() );

    // Initialize so that any number will be bigger than this one
    TComponentType max = std::numeric_limits<TComponentType>::min();

    // Get the max of each channel, keeping the largest
    for( unsigned int channel = 0; channel < reader->GetOutput()->GetNumberOfComponentsPerPixel(); channel++ )
    {
      // Extract the current channel
      indexSelectionFilter->SetIndex( channel );
      indexSelectionFilter->Update();

      /** Create statistics filter. */
      typename StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
      statistics->SetInput( indexSelectionFilter->GetOutput() );
      statistics->Update();
      if( statistics->GetMaximum() > max )
      {
        max = statistics->GetMaximum();
      }
    }

    /** Create invert filter. */
    typename InvertIntensityFilterType::Pointer invertFilter = InvertIntensityFilterType::New();
    invertFilter->SetMaximum( max );

    // Setup the filter to apply the invert filter to every channel
    typedef itk::ChannelByChannelVectorImageFilter2<
      VectorImageType, InvertIntensityFilterType >        ChannelByChannelInvertType;
    typename ChannelByChannelInvertType::Pointer channelByChannelInvertFilter
      = ChannelByChannelInvertType::New();
    channelByChannelInvertFilter->SetInput( reader->GetOutput() );
    channelByChannelInvertFilter->SetFilter( invertFilter );
    channelByChannelInvertFilter->Update();

    /** Create writer. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( channelByChannelInvertFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsInvertIntensity


#endif // end #ifndef __invertintensityimagefilter_h_
