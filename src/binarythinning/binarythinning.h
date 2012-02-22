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
#ifndef __binarythinning_h_
#define __binarythinning_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsBinaryThinningBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsBinaryThinningBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsBinaryThinningBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  }
  /** Destructor. */
  ~ITKToolsBinaryThinningBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end class ITKToolsBinaryThinningBase


/** \class ITKToolsBinaryThinning
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsBinaryThinning : public ITKToolsBinaryThinningBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsBinaryThinning Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsBinaryThinning(){};
  ~ITKToolsBinaryThinning(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedef's. */
    typedef itk::Image<TComponentType, VDimension>        InputImageType;
    typedef itk::ImageFileReader< InputImageType >        ReaderType;
    typedef itk::BinaryThinningImageFilter<
      InputImageType, InputImageType >                    FilterType;
    typedef itk::ImageFileWriter< InputImageType >        WriterType;

    /** Read in the input images. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName );

    /** Thin the image. */
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );

    /** Write image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( filter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsBinaryThinning

#endif // end __binarythinning_h_
