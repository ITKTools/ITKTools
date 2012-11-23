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
#ifndef __splitsegmentation_h_
#define __splitsegmentation_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSplitSegmentationImageFilter.h"


/** \class ITKToolsSplitSegmentationFilterBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsSplitSegmentationFilterBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsSplitSegmentationFilterBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_NumberOfSplitsZ = 3;
    this->m_NumberOfSplitsY = 2;
  };
  /** Destructor. */
  ~ITKToolsSplitSegmentationFilterBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  unsigned int m_NumberOfSplitsZ;
  unsigned int m_NumberOfSplitsY;
  std::vector<long> m_ChunkLabels;

}; // end class ITKToolsSplitSegmentationFilterBase


/** \class ITKToolsSplitSegmentationFilter
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsSplitSegmentationFilter : public ITKToolsSplitSegmentationFilterBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsSplitSegmentationFilter Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsSplitSegmentationFilter(){};
  ~ITKToolsSplitSegmentationFilter(){};

  /** Run function. */
  void Run( void )
  {
    /** TYPEDEF's. */
    typedef itk::Image<TComponentType, VDimension>        ImageType;
    typedef itk::ImageFileReader< ImageType >             ReaderType;
    typedef itk::SplitSegmentationImageFilter<
      ImageType, ImageType >                              SplitterType;
    typedef typename SplitterType::LabelType              LabelType;
    typedef typename SplitterType::OutputPixelType        OutputPixelType;
    typedef itk::ImageFileWriter< ImageType >             WriterType;

    /** Copy and cast the labels. */
    LabelType labels( this->m_ChunkLabels.size() );
    for( unsigned int i = 0; i < this->m_ChunkLabels.size(); ++i )
    {
      labels[ i ] = static_cast<OutputPixelType>( this->m_ChunkLabels[ i ] );
    }

    /** DECLARATION'S. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename SplitterType::Pointer filter = SplitterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Read in the input segmentation. */
    reader->SetFileName( this->m_InputFileName.c_str() );

    /** Setup the filter. */
    filter->SetInput( reader->GetOutput() );
    filter->SetNumberOfSplitsZ( this->m_NumberOfSplitsZ );
    filter->SetNumberOfSplitsY( this->m_NumberOfSplitsY );
    filter->SetChunkLabels( labels );

    /** Write the output image. */
    writer->SetInput( filter->GetOutput() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsSplitSegmentationFilter


#endif // end __splitsegmentation_h_
