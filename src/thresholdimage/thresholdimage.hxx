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
#ifndef __thresholdimage_hxx
#define __thresholdimage_hxx

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkOtsuThresholdWithMaskImageFilter.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkAdaptiveOtsuThresholdImageFilter.h"
#include "itkRobustAutomaticThresholdImageFilter.h"
#include "itkKappaSigmaThresholdImageFilter.h"
#include "itkMinErrorThresholdImageFilter.h"


/**
 * ******************* ThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::ThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & threshold1,
  const double & threshold2,
  const bool & useCompression )
{
  /** Typedef's. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef InputImageType                              OutputImageType;
  typedef InputPixelType                              OutputPixelType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::BinaryThresholdImageFilter<
    InputImageType, OutputImageType>                  ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Declarations. */
  InputPixelType lowerthreshold;
  typename ReaderType::Pointer reader = ReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  lowerthreshold = static_cast<InputPixelType>( vnl_math_max(
    static_cast<double>( itk::NumericTraits<InputPixelType>::NonpositiveMin() ),
    threshold1 ) );
  thresholder->SetLowerThreshold( lowerthreshold );
  thresholder->SetUpperThreshold( static_cast<InputPixelType>( threshold2 ) );
  thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end ThresholdImage()


/**
 * ******************* OtsuThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::OtsuThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const bool & useCompression )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::OtsuThresholdWithMaskImageFilter<
    InputImageType, OutputImageType>                    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetNumberOfHistogramBins( bins );
  thresholder->SetInsideValue( static_cast<OutputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<OutputPixelType>( outside ) );
  thresholder->SetInput( reader1->GetOutput() );
  if ( maskFileName != "" )
  {
    reader2->SetFileName( maskFileName.c_str() );
    thresholder->SetMaskImage( reader2->GetOutput() );
  }

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end OtsuThresholdImage()


/**
 * ******************* OtsuMultipleThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::OtsuMultipleThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & itkNotUsed( maskFileName ),
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & numThresholds,
  const bool & useCompression )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::OtsuMultipleThresholdsImageFilter<
    InputImageType, OutputImageType>                    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  //typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetInput( reader1->GetOutput() );
  thresholder->SetNumberOfHistogramBins( bins );
  //thresholder->SetInsideValue( static_cast<InputPixelType>( inside ) );
  //thresholder->SetOutsideValue( static_cast<InputPixelType>( outside ) );
  thresholder->SetNumberOfThresholds( numThresholds );
//   if ( maskFileName != "" )
//   {
//     reader2->SetFileName( maskFileName.c_str() );
//     thresholder->SetMaskImage( reader2->GetOutput() );
//   }

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end OtsuMultipleThresholdImage()


// /**
//  * ******************* AdaptiveOtsuThresholdImage *******************
//  */
//
// template< class TComponentType, unsigned int VDimension >
// void
// ITKToolsThresholdImage< TComponentType, VDimension >
// ::AdaptiveOtsuThresholdImage(
//   const std::string & inputFileName,
//   const std::string & outputFileName,
//   const unsigned int & radius,
//   const unsigned int & bins,
//   const unsigned int & controlPoints,
//   const unsigned int & levels,
//   const unsigned int & samples,
//   const unsigned int & splineOrder )
// {
//   /** Typedef's. */
//   const unsigned int ImageDimension = InputImageType::ImageDimension;
//
//   typedef unsigned char                                 OutputPixelType;
//   typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
//   typedef itk::ImageFileReader< InputImageType >        ReaderType;
//   typedef itk::AdaptiveOtsuThresholdImageFilter<
//     InputImageType, OutputImageType>                    ThresholderType;
//   typedef itk::ImageFileWriter< OutputImageType >       WriterType;
//   typedef ThresholderType::InputSizeType                RadiusType;
//
//   /** Declarations. */
//   typename ReaderType::Pointer reader = ReaderType::New();
//   typename ThresholderType::Pointer thresholder = ThresholderType::New();
//   typename WriterType::Pointer writer = WriterType::New();
//   RadiusType Radius; Radius.Fill( radius );
//
//   /** Read in the inputImage. */
//   reader->SetFileName( inputFileName.c_str() );
//
//   /** Apply the threshold. */
//   thresholder->SetRadius( Radius );
//   thresholder->SetNumberOfHistogramBins( bins );
//   thresholder->SetNumberOfControlPoints( controlPoints );
//   thresholder->SetNumberOfLevels( levels );
//   thresholder->SetNumberOfSamples( samples );
//   thresholder->SetSplineOrder( splineOrder );
//   thresholder->SetInsideValue( 1 );
//   thresholder->SetOutsideValue( 0 );
//   thresholder->SetInput( reader->GetOutput() );
//
//   /** Write the output image. */
//   writer->SetInput( thresholder->GetOutput() );
//   writer->SetFileName( outputFileName.c_str() );
//   writer->Update();
//
// } // end AdaptiveOtsuThresholdImage()


/**
 * ******************* RobustAutomaticThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::RobustAutomaticThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const double & pow,
  const bool & useCompression )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef float                                         GMPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::Image< GMPixelType, ImageDimension >     GMImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<
    InputImageType, GMImageType >                       GMFilterType;
  typedef itk::RobustAutomaticThresholdImageFilter<
    InputImageType, GMImageType, OutputImageType >      ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename GMFilterType::Pointer gradientFilter = GMFilterType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get the gradient magnitude of the input. */
  gradientFilter->SetInput( reader->GetOutput() );
  gradientFilter->SetSigma( 1.0 );
  gradientFilter->SetNormalizeAcrossScale( false );

  /** Apply the threshold. */
  thresholder->SetPow( pow );
  thresholder->SetInsideValue( static_cast<OutputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<OutputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );
  thresholder->SetGradientImage( gradientFilter->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end RobustAutomaticThresholdImage()


/**
 * ******************* KappaSigmaThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::KappaSigmaThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & maskFileName,
  const double & inside,
  const double & outside,
  const unsigned int & maskValue,
  const double & sigma,
  const unsigned int & iterations,
  const bool & useCompression )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef unsigned char                                 MaskPixelType;
  typedef itk::Image< MaskPixelType, ImageDimension >   MaskImageType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileReader< MaskImageType >         MaskReaderType;
  typedef itk::KappaSigmaThresholdImageFilter<
    InputImageType, MaskImageType, OutputImageType >    ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename MaskReaderType::Pointer reader2 = MaskReaderType::New();
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader1->SetFileName( inputFileName.c_str() );
  reader2->SetFileName( maskFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetMaskValue( maskValue );
  thresholder->SetSigmaFactor( sigma );
  thresholder->SetNumberOfIterations( iterations );
  thresholder->SetInsideValue( static_cast<OutputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<OutputPixelType>( outside ) );
  thresholder->SetInput( reader1->GetOutput() );
  thresholder->SetMaskImage( reader2->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end KappaSigmaThresholdImage()


/**
 * ******************* MinErrorThresholdImage *******************
 */

template< class TComponentType, unsigned int VDimension >
void
ITKToolsThresholdImage< TComponentType, VDimension >
::MinErrorThresholdImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const double & inside,
  const double & outside,
  const unsigned int & bins,
  const unsigned int & mixtureType,
  const bool & useCompression )
{
  /** Typedef's. */
  const unsigned int ImageDimension = InputImageType::ImageDimension;

  typedef typename InputImageType::PixelType            InputPixelType;
  typedef unsigned char                                 OutputPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::MinErrorThresholdImageFilter<
    InputImageType, OutputImageType >                   ThresholderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();

  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );

  /** Apply the threshold. */
  thresholder->SetNumberOfHistogramBins( bins );
  thresholder->SetMixtureType( mixtureType );
  thresholder->SetInsideValue( static_cast<OutputPixelType>( inside ) );
  thresholder->SetOutsideValue( static_cast<OutputPixelType>( outside ) );
  thresholder->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end MinErrorThresholdImage()


#endif // end #ifndef __thresholdimage_hxx
