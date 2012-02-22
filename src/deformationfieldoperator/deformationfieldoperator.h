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
#ifndef __deformationfieldoperator_h_
#define __deformationfieldoperator_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDisplacementFieldJacobianDeterminantFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkIterativeInverseDisplacementFieldImageFilter.h"


/** \class ITKToolsDeformationFieldOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsDeformationFieldOperatorBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsDeformationFieldOperatorBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Ops = "";
    this->m_NumberOfStreams = 0;
    this->m_NumberOfIterations = 0;
    this->m_StopValue = 0.0f;
  };
  /** Destructor. */
  ~ITKToolsDeformationFieldOperatorBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_Ops;
  unsigned int m_NumberOfStreams;
  unsigned int m_NumberOfIterations;
  double m_StopValue;

}; // end class ITKToolsDeformationFieldOperatorBase


/** \class ITKToolsDeformationFieldOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsDeformationFieldOperator : public ITKToolsDeformationFieldOperatorBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsDeformationFieldOperator Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsDeformationFieldOperator(){};
  ~ITKToolsDeformationFieldOperator(){};

  /** Typedef's. */
  typedef TComponentType                                ScalarPixelType;
  typedef itk::Vector< TComponentType, VDimension >     VectorPixelType;
  typedef itk::Image< ScalarPixelType, VDimension >     ScalarImageType;
  typedef itk::Image< VectorPixelType, VDimension >     VectorImageType;

  /** Run function. */
  void Run( void )
  {
    typedef itk::ImageFileReader< VectorImageType >       ReaderType;

    /** DECLARATION'S. */
    typename VectorImageType::Pointer workingImage;
    typename ReaderType::Pointer reader = ReaderType::New();

    /** Read in the inputImage. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    // temporarily: only streaming support for Jacobian case needed for EMPIRE10 challenge.
    if( this->m_Ops != "DEF2JAC" && this->m_Ops != "JACOBIAN" )
    {
      reader->Update();
    }

    /** Change to Transformation or Deformation by adding/subtracting pixel coordinates */
    workingImage = reader->GetOutput();

    /** Do something with this image and save the result */
    if( this->m_Ops == "DEF2TRANS" )
    {
      this->Deformation2Transformation( workingImage, true );
    }
    else if( this->m_Ops == "TRANS2DEF" )
    {
      this->Deformation2Transformation( workingImage, false );
    }
    else if( this->m_Ops == "MAGNITUDE" )
    {
      this->ComputeMagnitude( workingImage );
    }
    else if( this->m_Ops == "DEF2JAC" )
    {
      this->ComputeJacobian();
    }
    else if( this->m_Ops == "INVERSE" )
    {
      this->ComputeInverse();
    }
    else
    {
      itkGenericExceptionMacro( << "<< invalid operator: " << this->m_Ops );
    }
  } // end Run()

  /** Helper functions that implement the real functionality. */
  void Deformation2Transformation( VectorImageType * inputImage, bool def2trans );
  void ComputeMagnitude( VectorImageType * inputImage );
  void ComputeJacobian( void );
  void ComputeInverse( void );

}; // end class ITKToolsDeformationFieldOperator

// \todo: should be moved to hxx

/**
 * *************** Deformation2Transformation *********************
 * convert between deformation fields and transformation 'fields'
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsDeformationFieldOperator< VDimension, TComponentType >
::Deformation2Transformation( VectorImageType * inputImage, bool def2trans )
{
  typedef itk::ImageFileWriter< VectorImageType >     WriterType;
  typedef itk::ImageRegionIteratorWithIndex<
    VectorImageType >                                 IteratorType;
  typedef typename VectorImageType::IndexType         IndexType;
  typedef typename VectorImageType::PointType         PointType;

  /** We are going to change the image, so make sure these changes are not undone */
  inputImage->Update();
  inputImage->DisconnectPipeline();

  IteratorType it( inputImage, inputImage->GetLargestPossibleRegion() );
  it.GoToBegin();
  double plusormin = 1.0;
  std::string message = "from deformation to transformation";
  if( !def2trans )
  {
    plusormin = -1.0;
    message = "from transformation to deformation";
  }
  std::cout << "Changing image " << message << "..." << std::endl;
  while ( !( it.IsAtEnd() ) )
  {
    const IndexType & index = it.GetIndex();
    VectorPixelType & value = it.Value();
    PointType point;
    inputImage->TransformIndexToPhysicalPoint( index, point );
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      value[ i ] += static_cast<TComponentType>( plusormin * point[ i ] );
    }
    ++it;
  }
  std::cout << "Ready changing image " << message << "." << std::endl;

  /** Write the output image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput( inputImage );
  writer->SetFileName( this->m_OutputFileName.c_str() );
  writer->Update();

} // end Deformation2Transformation()


/**
 * ******************* ComputeMagnitude ************************
 * Write magnitude of deformation field to disk
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsDeformationFieldOperator< VDimension, TComponentType >
::ComputeMagnitude( VectorImageType * inputImage )
{
  typedef itk::ImageFileWriter< ScalarImageType >     WriterType;
  typedef itk::GradientToMagnitudeImageFilter<
    VectorImageType, ScalarImageType >                MagnitudeFilterType;

  typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
  typename WriterType::Pointer writer = WriterType::New();

  magnitudeFilter->SetInput( inputImage );
  magnitudeFilter->Update();

  /** Write the output image. */
  writer->SetInput( magnitudeFilter->GetOutput() );
  writer->SetFileName( this->m_OutputFileName.c_str() );
  writer->Update();

} // end ComputeMagnitude()


/**
 * ******************* ComputeJacobian ************************
 * Compute Jacobian of deformation field
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsDeformationFieldOperator< VDimension, TComponentType >
::ComputeJacobian( void )
{
  /** Typedef's. */
  typedef itk::ImageFileReader< VectorImageType >       ReaderType;
  typedef itk::DisplacementFieldJacobianDeterminantFilter<
    VectorImageType, TComponentType, ScalarImageType >  DefToJacFilterType;
  typedef itk::ImageFileWriter< ScalarImageType >       WriterType;

  /** Setup reader. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( this->m_InputFileName.c_str() );

  /** Setup Jacobian filter. */
  typename DefToJacFilterType::Pointer defToJacFilter = DefToJacFilterType::New();
  defToJacFilter->SetUseImageSpacingOn();
  defToJacFilter->SetInput( reader->GetOutput() );

  /** Setup writer.  No intermediate calls to Update() are allowed,
   * otherwise streaming does not work.
   */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput( defToJacFilter->GetOutput() );
  writer->SetFileName( this->m_OutputFileName.c_str() );
  writer->SetNumberOfStreamDivisions( this->m_NumberOfStreams );
  writer->Update();

} // end ComputeJacobian()


/**
 * ******************* ComputeInverse ************************
 * Compute inverse of deformation field
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsDeformationFieldOperator< VDimension, TComponentType >
::ComputeInverse( void )
{
  /** Typedef's. */
  typedef itk::ImageFileReader< VectorImageType >     ReaderType;
  typedef itk::ImageFileWriter< VectorImageType >     WriterType;
  typedef itk::IterativeInverseDisplacementFieldImageFilter<
    VectorImageType, VectorImageType >                InverseDeformationFilterType;

  /** Declare filters. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename InverseDeformationFilterType::Pointer inversionFilter
    = InverseDeformationFilterType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Setup reader. */
  reader->SetFileName( this->m_InputFileName.c_str() );

  /** Setup inversion filter. */
  inversionFilter->SetInput( reader->GetOutput() );
  inversionFilter->SetNumberOfIterations( this->m_NumberOfIterations );
  inversionFilter->SetStopValue( this->m_StopValue );

  /** Setup writer.  No intermediate calls to Update() are allowed,
   * otherwise streaming does not work.
   */
  writer->SetInput( inversionFilter->GetOutput() );
  writer->SetFileName( this->m_OutputFileName.c_str() );
  writer->SetNumberOfStreamDivisions( this->m_NumberOfStreams );
  writer->Update();

} // end ComputeInverse()


#endif // end #ifndef __deformationfieldoperator_h_
