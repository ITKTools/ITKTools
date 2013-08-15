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
#ifndef __resizeimage_h_
#define __resizeimage_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsResizeImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsResizeImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsResizeImageBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_ResizingSpecifiedBy = "";
    this->m_InterpolationOrder = 0;
  };
  /** Destructor. */
  ~ITKToolsResizeImageBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_ResizingSpecifiedBy;
  std::vector<double> m_ResizeFactor;
  std::vector<double> m_OutputSpacing;
  std::vector<unsigned int> m_OutputSize;
  unsigned int m_InterpolationOrder;

}; // end class ITKToolsResizeImageBase


/** \class ITKToolsResizeImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsResizeImage : public ITKToolsResizeImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsResizeImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsResizeImage(){};
  ~ITKToolsResizeImage(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>      InputImageType;
    typedef itk::ResampleImageFilter< InputImageType, InputImageType >  ResamplerType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< InputImageType >      WriterType;
    typedef itk::NearestNeighborInterpolateImageFunction<
      InputImageType, double >                          NNInterpolatorType;
    typedef itk::BSplineInterpolateImageFunction<
      InputImageType >                                  BSplineInterpolatorType;

    typedef typename InputImageType::SizeType         SizeType;
    typedef typename InputImageType::SpacingType      SpacingType;

    const unsigned int Dimension = InputImageType::ImageDimension;

    /** Declarations. */
    typename InputImageType::Pointer inputImage;
    typename ResamplerType::Pointer resampler = ResamplerType::New();
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();
    typename NNInterpolatorType::Pointer nnInterpolator
      = NNInterpolatorType::New();
    typename BSplineInterpolatorType::Pointer bsInterpolator
      = BSplineInterpolatorType::New();

    /** Read in the inputImage. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    inputImage = reader->GetOutput();
    inputImage->Update();

    /** Prepare stuff. */
    SpacingType inputSpacing  = inputImage->GetSpacing();
    SizeType    inputSize     = inputImage->GetLargestPossibleRegion().GetSize();
    SpacingType outputSpacing = inputSpacing;
    SizeType    outputSize    = inputSize;
    if( this->m_ResizingSpecifiedBy == "ResizeFactor" )
    {
      for( unsigned int i = 0; i < Dimension; i++ )
      {
        outputSpacing[ i ] /= this->m_ResizeFactor[ i ];
        outputSize[ i ] = static_cast<unsigned int>( outputSize[ i ] * this->m_ResizeFactor[ i ] );
      }
    }
    else if( this->m_ResizingSpecifiedBy == "OutputSpacing" )
    {
      for( unsigned int i = 0; i < Dimension; i++ )
      {
        outputSpacing[ i ] = this->m_OutputSpacing[ i ];
        outputSize[ i ] = static_cast<unsigned int>(
          inputSpacing[ i ] * inputSize[ i ] / this->m_OutputSpacing[ i ] );
      }
    }
    else if( this->m_ResizingSpecifiedBy == "OutputSize" )
    {
      for( unsigned int i = 0; i < Dimension; i++ )
      {
        outputSpacing[ i ] = inputSpacing[ i ] * inputSize[ i ] / this->m_OutputSize[ i ];
        outputSize[ i ] = this->m_OutputSize[ i ];
      }
    }

    /** Setup the pipeline. */
    resampler->SetInput( inputImage );
    resampler->SetSize( outputSize );
    resampler->SetDefaultPixelValue( 0 );
    resampler->SetOutputStartIndex( inputImage->GetLargestPossibleRegion().GetIndex() );
    resampler->SetOutputSpacing( outputSpacing );
    resampler->SetOutputOrigin( inputImage->GetOrigin() );
    resampler->SetOutputDirection( inputImage->GetDirection() );

    /* The interpolator: the resampler has by default a
     * LinearInterpolateImageFunction as interpolator.
     */
    if( this->m_InterpolationOrder == 0 )
    {
      resampler->SetInterpolator( nnInterpolator );
    }
    else if( this->m_InterpolationOrder > 1 )
    {
      bsInterpolator->SetSplineOrder( this->m_InterpolationOrder );
      resampler->SetInterpolator( bsInterpolator );
    }

    /** Write the output image. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( resampler->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsResizeImage

#endif // end #ifndef __resizeimage_h_
