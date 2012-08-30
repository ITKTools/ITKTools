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
#ifndef __enhancement_h_
#define __enhancement_h_

#include "ITKToolsBase.h"

//
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// Supported vesselness filters
#include "itkMultiScaleGaussianEnhancementImageFilter.h"
#include "itkFrangiVesselnessFunctor.h"
#include "itkModifiedKrissianVesselnessFunctor.h"
#include "itkStrainEnergyVesselnessFunctor.h"
#include "itkStrainEnergySheetnessFunctor.h"
#include "itkFrangiSheetnessFunctor.h"
#include "itkDescoteauxSheetnessFunctor.h"
#include "itkFrangiXiaoSheetnessFunctor.h"
#include "itkDescoteauxXiaoSheetnessFunctor.h"

#include <vector>
#include <string>
#include <iostream>


/** \class ITKToolsEnhancementBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 * \authors Changyan Xiao, Marius Staring, Denis Shamonin,
 * Johan H.C. Reiber, Jan Stolk, Berend C. Stoel
 */

class ITKToolsEnhancementBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsEnhancementBase()
  {
    this->m_InputFileName = "";
    this->m_Method = "";

    this->m_Rescale = true;

    this->m_SigmaStepMethod = 1;
    this->m_SigmaMinimum = 1.0;
    this->m_SigmaMaximum = 4.0;
    this->m_NumberOfSigmaSteps = 4;

    this->m_Alpha = 0.5;
    this->m_Beta = 0.5;
    this->m_C = 500.0;
    this->m_Nu = 0.0;
    this->m_Kappa = 0.8;
  };
  /** Destructor. */
  ~ITKToolsEnhancementBase(){};

  /** Input member parameters. */
  std::string                 m_InputFileName;
  std::vector<std::string>    m_OutputFileNames;

  std::string m_Method;

  bool m_Rescale;

  unsigned int m_SigmaStepMethod;
  double m_SigmaMinimum;
  double m_SigmaMaximum;
  unsigned int m_NumberOfSigmaSteps;

  double m_Alpha;
  double m_Beta;
  double m_C;
  double m_Nu;
  double m_Kappa;

}; // end class ITKToolsEnhancementBase


/** \class ITKToolsEnhancement
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsEnhancement : public ITKToolsEnhancementBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsEnhancement Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsEnhancement(){};
  ~ITKToolsEnhancement(){};

  /** Typedef's. */
  typedef TComponentType                              InputPixelType;
  typedef TComponentType                              OutputPixelType;
  typedef itk::Image< InputPixelType, VDimension >    InputImageType;
  typedef itk::Image< OutputPixelType, VDimension >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  typedef itk::MultiScaleGaussianEnhancementImageFilter<
    InputImageType, OutputImageType >                 MultiScaleFilterType;
  typedef typename MultiScaleFilterType
    ::GradientMagnitudePixelType                      GradientMagnitudePixelType;
  typedef typename MultiScaleFilterType
    ::EigenValueArrayType                             EigenValueArrayType;

  /** Supported functors. */
  typedef itk::Functor::FrangiVesselnessFunctor<
    EigenValueArrayType, OutputPixelType >            FrangiVesselnessFunctorType;
  typedef itk::Functor::FrangiSheetnessFunctor<
    EigenValueArrayType, OutputPixelType >            FrangiSheetnessFunctorType;
  typedef itk::Functor::DescoteauxSheetnessFunctor<
    EigenValueArrayType, OutputPixelType >            DescoteauxSheetnessFunctorType;
  typedef itk::Functor::ModifiedKrissianVesselnessFunctor<
    EigenValueArrayType, OutputPixelType >            ModifiedKrissianVesselnessFunctorType;

  typedef itk::Functor::StrainEnergyVesselnessFunctor<
    GradientMagnitudePixelType, EigenValueArrayType,
    OutputPixelType >                                 StrainEnergyVesselnessFunctorType;
  typedef itk::Functor::StrainEnergySheetnessFunctor<
    GradientMagnitudePixelType, EigenValueArrayType,
    OutputPixelType >                                 StrainEnergySheetnessFunctorType;
  typedef itk::Functor::FrangiXiaoSheetnessFunctor<
    GradientMagnitudePixelType, EigenValueArrayType,
    OutputPixelType >                                 FrangiXiaoSheetnessFunctorType;
  typedef itk::Functor::DescoteauxXiaoSheetnessFunctor<
    GradientMagnitudePixelType, EigenValueArrayType,
    OutputPixelType >                                 DescoteauxXiaoSheetnessFunctorType;

  /** Run function. */
  void Run( void )
  {
    /** Read the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );

    /** Setup the multi-scale filter. */
    typename MultiScaleFilterType::Pointer multiScaleFilter
      = MultiScaleFilterType::New();

    const bool generateScalesOutput = ( this->m_OutputFileNames.size() > 1 );

    multiScaleFilter->SetSigmaMinimum( this->m_SigmaMinimum );
    multiScaleFilter->SetSigmaMaximum( this->m_SigmaMaximum );
    multiScaleFilter->SetNumberOfSigmaSteps( this->m_NumberOfSigmaSteps );
    multiScaleFilter->SetNonNegativeHessianBasedMeasure( true );
    multiScaleFilter->SetGenerateScalesOutput( generateScalesOutput );
    multiScaleFilter->SetSigmaStepMethod( this->m_SigmaStepMethod );
    multiScaleFilter->SetRescale( this->m_Rescale );
    multiScaleFilter->SetInput( reader->GetOutput() );

    /** Setup the requested functor and connect it to the filter. */
    if ( this->m_Method == "FrangiVesselness" )
    {
      typename FrangiVesselnessFunctorType::Pointer functor
        = FrangiVesselnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetC( this->m_C );
      functor->SetBrightObject( true );

      multiScaleFilter->SetUnaryFunctor( functor );
    }
    else if ( this->m_Method == "StrainEnergyVesselness" )
    {
      typename StrainEnergyVesselnessFunctorType::Pointer functor
        = StrainEnergyVesselnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetNu( this->m_Nu );
      functor->SetKappa( this->m_Kappa );
      functor->SetBrightObject( true );

      multiScaleFilter->SetBinaryFunctor( functor );
    }
    else if ( this->m_Method == "ModifiedKrissianVesselness" )
    {
      typename ModifiedKrissianVesselnessFunctorType::Pointer functor
        = ModifiedKrissianVesselnessFunctorType::New();
      functor->SetBrightObject( true );

      multiScaleFilter->SetUnaryFunctor( functor );
    }
    /** Setup the requested functor and connect it to the filter. */
    else if ( this->m_Method == "FrangiSheetness" )
    {
      typename FrangiSheetnessFunctorType::Pointer functor
        = FrangiSheetnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetC( this->m_C );
      functor->SetBrightObject( true );

      multiScaleFilter->SetUnaryFunctor( functor );
    }
    else if ( this->m_Method == "DescoteauxSheetness" )
    {
      typename DescoteauxSheetnessFunctorType::Pointer functor
        = DescoteauxSheetnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetC( this->m_C );
      functor->SetBrightObject( true );

      multiScaleFilter->SetUnaryFunctor( functor );
    }
    else if ( this->m_Method == "StrainEnergySheetness" )
    {
      typename StrainEnergySheetnessFunctorType::Pointer functor
        = StrainEnergySheetnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetNu( this->m_Nu );
      functor->SetKappa( this->m_Kappa );
      functor->SetBrightObject( true );

      multiScaleFilter->SetBinaryFunctor( functor );
    }
    else if ( this->m_Method == "FrangiXiaoSheetness" )
    {
      typename FrangiXiaoSheetnessFunctorType::Pointer functor
        = FrangiXiaoSheetnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetC( this->m_C );
      functor->SetKappa( this->m_Kappa );
      functor->SetBrightObject( true );

      multiScaleFilter->SetBinaryFunctor( functor );
    }
    else if ( this->m_Method == "DescoteauxXiaoSheetness" )
    {
      typename DescoteauxXiaoSheetnessFunctorType::Pointer functor
        = DescoteauxXiaoSheetnessFunctorType::New();
      functor->SetAlpha( this->m_Alpha );
      functor->SetBeta( this->m_Beta );
      functor->SetC( this->m_C );
      functor->SetKappa( this->m_Kappa );
      functor->SetBrightObject( true );

      multiScaleFilter->SetBinaryFunctor( functor );
    }
    else
    {
      itkGenericExceptionMacro( << "ERROR: unknown method " << this->m_Method << "!" );
    }

    /** Write enhanced output. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput( multiScaleFilter->GetOutput() );
    writer->SetFileName( this->m_OutputFileNames[ 0 ] );
    writer->Update();

    /** Write the maximumn scale response. */
    if( generateScalesOutput )
    {
      writer->SetInput( multiScaleFilter->GetOutput( 1 ) );
      writer->SetFileName( this->m_OutputFileNames[ 1 ] );
      writer->Update();
    }

  } // end Run()

}; // end class ITKToolsEnhancement


#endif // end #ifndef __enhancement_h_
