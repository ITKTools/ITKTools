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
#ifndef __statisticsonimage_h_
#define __statisticsonimage_h_

#include "ITKToolsBase.h"

#include "itkImageToImageFilter.h"
#include "itkStatisticsImageFilterWithMask.h"
#include "itkScalarImageToHistogramGenerator2.h"


/** \class ITKToolsStatisticsOnImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsStatisticsOnImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsStatisticsOnImageBase()
  {
    this->m_InputFileName = "";
    this->m_MaskFileName = "";
    this->m_HistogramOutputFileName = "";
    this->m_NumberOfBins = 0;
    this->m_Select = "";
  };
  /** Destructor. */
  ~ITKToolsStatisticsOnImageBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_MaskFileName;
  std::string m_HistogramOutputFileName;
  unsigned int m_NumberOfBins;
  std::string m_Select;

}; // end class StatisticsOnImageBase


/** \class ITKToolsStatisticsOnImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, unsigned int VNumberOfComponents, class TComponentType >
class ITKToolsStatisticsOnImage : public ITKToolsStatisticsOnImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsStatisticsOnImage Self;

  ITKToolsStatisticsOnImage(){};
  ~ITKToolsStatisticsOnImage(){};

  static Self * New( unsigned int dim, unsigned int numberOfComponents, itk::ImageIOBase::IOComponentType componentType )
  {
    if( itktools::IsType<TComponentType>( componentType )
      && VDimension == dim && VNumberOfComponents == numberOfComponents )
    {
      return new Self;
    }
    return 0;
  }

  /** Typedefs */
  typedef double                                      InternalPixelType;
  typedef itk::Image<InternalPixelType, VDimension>   InternalImageType;
  typedef itk::ImageToImageFilter<
    InternalImageType, InternalImageType>             BaseFilterType;
  typedef itk::StatisticsImageFilter<
    InternalImageType >                               StatisticsFilterType;
  typedef itk::Statistics::ScalarImageToHistogramGenerator2<
    InternalImageType >                               HistogramGeneratorType;

  /** Run function. */
  void Run( void );

  /** Helper function. */
  void ComputeStatistics(
    InternalImageType * inputImage,
    BaseFilterType * maskerOrCopier,
    StatisticsFilterType * statistics,
    HistogramGeneratorType * histogramGenerator,
    unsigned int numberOfBins, // Type is defined in ScalarImageToHistogramGenerator2
    const std::string & histogramOutputFileName,
    const std::string & select );

  /** Helper function. */
  void DetermineHistogramMaximum(
    const InternalPixelType & maxPixelValue,
    const InternalPixelType & minPixelValue,
    const unsigned int & numberOfBins,
    InternalPixelType & histogramMax );

}; // end class ITKToolsStatisticsOnImage

#include "statisticsonimage.hxx"

#endif // #ifndef __statisticsonimage_h_
