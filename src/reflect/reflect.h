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
#ifndef __reflect_h_
#define __reflect_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsUnaryImageOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsReflectBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsReflectBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Direction = 0;
  };
  /** Destructor. */
  ~ITKToolsReflectBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  unsigned int m_Direction;

}; // end ITKToolsReflectBase


/** \class ITKToolsReflect
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsReflect : public ITKToolsReflectBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsReflect Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsReflect(){};
  ~ITKToolsReflect(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef TComponentType                                    OutputPixelType;
    const unsigned int Dimension = VDimension;

    typedef OutputPixelType                                 InputPixelType;

    typedef itk::Image< InputPixelType, Dimension >         InputImageType;
    typedef itk::Image< OutputPixelType, Dimension >        OutputImageType;

    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::FlipImageFilter< InputImageType >          ReflectFilterType;
    typedef itk::ImageFileWriter< OutputImageType >         WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename ReflectFilterType::Pointer reflectFilter = ReflectFilterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Set up pipeline. */
    reader->SetFileName( this->m_InputFileName );

    itk::FixedArray<bool, Dimension> flipAxes(false);
    flipAxes[m_Direction] = true;

    reflectFilter->SetFlipAxes( flipAxes );
    writer->SetFileName( this->m_OutputFileName );

    reflectFilter->SetInput( reader->GetOutput() );
    writer->SetInput( reflectFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end Reflect


#endif // end #ifndef __reflect_h_
