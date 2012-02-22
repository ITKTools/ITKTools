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
#ifndef __contrastenhanceimage_h_
#define __contrastenhanceimage_h_

#include "ITKToolsBase.h"

#include <iostream>
#include <string>
#include <map>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"


/** \class ITKToolsContrastEnhanceImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsContrastEnhanceImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsContrastEnhanceImageBase()
  {
    this->m_Alpha = 0.0f;
    this->m_Beta = 0.0f;
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_LookUpTable = false;
  };
  /** Destructor. */
  ~ITKToolsContrastEnhanceImageBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  float m_Alpha;
  float m_Beta;
  bool m_LookUpTable;
  std::vector<unsigned int> m_Radius;

}; // end class ITKToolsContrastEnhanceImageBase


/** \class ITKToolsContrastEnhanceImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsContrastEnhanceImage : public ITKToolsContrastEnhanceImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsContrastEnhanceImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsContrastEnhanceImage(){};
  ~ITKToolsContrastEnhanceImage(){};

  /** Run function. */
  void Run( void )
  {
    typedef itk::Image< TComponentType, VDimension > ImageType;
    typedef itk::ImageFileReader<ImageType>       ReaderType;
    typedef itk::AdaptiveHistogramEqualizationImageFilter<
      ImageType >                                 EnhancerType;
    typedef itk::ImageFileWriter<ImageType>       WriterType;
    typedef typename EnhancerType::ImageSizeType  RadiusType;

    /** vars */
    itk::Size<VDimension> radiusSize;
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      radiusSize[ i ] = this->m_Radius[ i ];
    }

    /** Try to read input image */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Setup pipeline and configure its components */
    typename EnhancerType::Pointer enhancer = EnhancerType::New();
    enhancer->SetUseLookupTable( this->m_LookUpTable );
    enhancer->SetAlpha( this->m_Alpha );
    enhancer->SetBeta( this->m_Beta );
    enhancer->SetRadius( radiusSize );
    enhancer->SetInput( reader->GetOutput() );

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput( enhancer->GetOutput() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsContrastEnhanceImage


#endif // #ifndef __contrastenhanceimage_h_
