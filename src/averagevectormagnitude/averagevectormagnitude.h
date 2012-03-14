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
#ifndef __averagevectormagnitude_h_
#define __averagevectormagnitude_h_


#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkGradientToMagnitudeImageFilter.h"


/** \class ITKToolsAverageVectorMagnitudeBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsAverageVectorMagnitudeBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsAverageVectorMagnitudeBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  }
  /** Destructor. */
  ~ITKToolsAverageVectorMagnitudeBase(){};

  /** Input member parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end class ITKToolsAverageVectorMagnitudeBase


/** \class ITKToolsUnaryImageOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType, unsigned int VVectorDimension >
class ITKToolsAverageVectorMagnitude : public ITKToolsAverageVectorMagnitudeBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsAverageVectorMagnitude Self;

  ITKToolsAverageVectorMagnitude(){};
  ~ITKToolsAverageVectorMagnitude(){};

  static Self * New( unsigned int imageDimension,
    itk::ImageIOBase::IOComponentType componentType,
    unsigned int vectorDimension )
  {
    if( VDimension == imageDimension
      && itktools::IsType<TComponentType>( componentType )
      && VVectorDimension == vectorDimension )
    {
      return new Self;
    }
    return 0;
  }

  /** Run function. */
  void Run( void )
  {
    // \todo: support vector images with any number of components
    // probably by using VariableLebgthVector

    /** Typedefs */
    typedef itk::Vector< TComponentType, VVectorDimension >   InputPixelType;
    typedef TComponentType                                    OutputPixelType;
    typedef itk::Image< InputPixelType, VDimension >          InputImageType;
    typedef itk::Image< OutputPixelType, VDimension >         OutputImageType;

    typedef itk::ImageFileReader< InputImageType >            ReaderType;
    typedef itk::GradientToMagnitudeImageFilter<
      InputImageType, OutputImageType >                       FilterType;
    typedef itk::ImageFileWriter< OutputImageType >           WriterType;

    /** Setup the pipeline */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName );

    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( filter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsAverageVectorMagnitude


#endif // end #ifndef __averagevectormagnitude_h_

