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
#ifndef __rescaleintensityimagefilter_h_
#define __rescaleintensityimagefilter_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "vnl/vnl_math.h"


/** \class ITKToolsRescaleIntensityImageFilterBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsRescaleIntensityImageFilterBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsRescaleIntensityImageFilterBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_ValuesAreExtrema = false;
  };
  /** Destructor. */
  ~ITKToolsRescaleIntensityImageFilterBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_Values;
  bool m_ValuesAreExtrema;

}; // end class ITKToolsRescaleIntensityImageFilterBase


/** \class ITKToolsRescaleIntensityFilter
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsRescaleIntensityFilter : public ITKToolsRescaleIntensityImageFilterBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsRescaleIntensityFilter Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsRescaleIntensityFilter(){};
  ~ITKToolsRescaleIntensityFilter(){};

  /** Run function. */
  void Run( void )
  {
    /** TYPEDEF's. */
    typedef itk::Image<TComponentType, VDimension>        ScalarImageType;
    typedef itk::VectorImage<TComponentType, VDimension>  VectorImageType;

    typedef itk::ImageFileReader< VectorImageType >       ReaderType;
    typedef itk::RescaleIntensityImageFilter<
      ScalarImageType, ScalarImageType >                  RescalerType;
    typedef itk::StatisticsImageFilter< ScalarImageType > StatisticsType;
    typedef itk::ShiftScaleImageFilter<
      ScalarImageType, ScalarImageType >                  ShiftScalerType;
    typedef itk::ImageFileWriter< VectorImageType >       WriterType;
    typedef typename ScalarImageType::PixelType           PixelType;
    typedef typename StatisticsType::RealType             RealType;

    /** DECLARATION'S. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();
    typename RescalerType::Pointer    filter;
    typename StatisticsType::Pointer  statistics;
    typename ShiftScalerType::Pointer shiftscaler;

    /** Read in the inputImage. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    // Setup type to disassemble the components
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexSelectionType;

    // Setup the filter to reassemble the components
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    for( unsigned int component = 0; component < reader->GetOutput()->GetNumberOfComponentsPerPixel(); ++component)
    {
      typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
      indexSelectionFilter->SetIndex(component);
      indexSelectionFilter->SetInput(reader->GetOutput());
      indexSelectionFilter->Update();

      /** If the input values are extrema (minimum and maximum),
       * then an IntensityRescaler is used. Otherwise, the values represent
       * the desired mean and variance and a ShiftScaler is used.
       */
      if( this->m_ValuesAreExtrema )
      {
        /** Create instance. */
        filter = RescalerType::New();

        /** Define the extrema. */
        PixelType min, max;
        if( this->m_Values[ 0 ] == 0.0 && this->m_Values[ 1 ] == 0.0 )
        {
          min = itk::NumericTraits<PixelType>::NonpositiveMin();
          max = itk::NumericTraits<PixelType>::max();
        }
        else
        {
          min = static_cast<PixelType>( this->m_Values[ 0 ] );
          max = static_cast<PixelType>( this->m_Values[ 1 ] );
        }

        /** Setup the filter. */
        filter->SetInput( indexSelectionFilter->GetOutput() );
        filter->SetOutputMinimum( min );
        filter->SetOutputMaximum( max );
        filter->Update();

        /** Setup the recombining. */
        imageToVectorImageFilter->SetNthInput(component, filter->GetOutput());

      } // end if values are extrema
      else
      {
        /** Create instances. */
        statistics = StatisticsType::New();
        shiftscaler = ShiftScalerType::New();

        /** Calculate image statistics. */
        statistics->SetInput( indexSelectionFilter->GetOutput() );
        statistics->Update();

        /** Get mean and variance of input image. */
        RealType mean = statistics->GetMean();
        RealType sigma = statistics->GetSigma();

        /** Setup the shiftscaler. */
        shiftscaler->SetInput( indexSelectionFilter->GetOutput() );
        shiftscaler->SetShift( this->m_Values[ 0 ] * sigma / vcl_sqrt( this->m_Values[ 1 ] ) - mean );
        shiftscaler->SetScale( vcl_sqrt( this->m_Values[ 1 ] ) / sigma );
        shiftscaler->Update();

        /** Setup the recombining. */
        imageToVectorImageFilter->SetNthInput(component, shiftscaler->GetOutput());

      } // end if values are mean and variance
    }// end component loop

    imageToVectorImageFilter->Update();
    writer->SetInput(imageToVectorImageFilter->GetOutput());

    /** Write the output image. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsRescaleIntensityFilter


#endif // end __rescaleintensityimagefilter_h_
