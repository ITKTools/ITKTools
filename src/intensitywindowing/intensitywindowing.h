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
#ifndef __intensitywindowing_h_
#define __intensitywindowing_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsIntensityWindowingBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsIntensityWindowingBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsIntensityWindowingBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsIntensityWindowingBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_Window;

}; // end class ITKToolsIntensityWindowingBase


/** \class ITKToolsIntensityWindowing
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsIntensityWindowing : public ITKToolsIntensityWindowingBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsIntensityWindowing Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsIntensityWindowing(){};
  ~ITKToolsIntensityWindowing(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>      InputImageType;
    typedef itk::IntensityWindowingImageFilter<
      InputImageType, InputImageType >                  WindowingType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< InputImageType >      WriterType;
    typedef typename InputImageType::PixelType          InputPixelType;

    /** Declarations. */
    typename WindowingType::Pointer windowfilter = WindowingType::New();
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Setup the pipeline. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    InputPixelType min = static_cast<InputPixelType>( this->m_Window[ 0 ] );
    InputPixelType max = static_cast<InputPixelType>( this->m_Window[ 1 ] );
    windowfilter->SetWindowMinimum( min );
    windowfilter->SetWindowMaximum( max );
    windowfilter->SetOutputMinimum( min );
    windowfilter->SetOutputMaximum( max );

    /** Connect and execute the pipeline. */
    windowfilter->SetInput( reader->GetOutput() );
    writer->SetInput( windowfilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end IntensityWindowing

#endif // end #ifndef __intensitywindowing_h_
