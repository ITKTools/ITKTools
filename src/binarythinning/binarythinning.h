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
/** \file
 \brief Compute binary thinning of an image.
 
 \verbinclude binarythinning.help
 */

#ifndef __binarythinning_h
#define __binarythinning_h

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkImageFileWriter.h"


class ITKToolsBinaryThinningBase : public itktools::ITKToolsBase
{
public:
  ITKToolsBinaryThinningBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  }

  ~ITKToolsBinaryThinningBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

  virtual void Run( void ) = 0;

}; // end BinaryThinningBase


template< class TComponentType, unsigned int VImageDimension >
class ITKToolsBinaryThinning : public ITKToolsBinaryThinningBase
{
public:
  typedef ITKToolsBinaryThinning Self;

  ITKToolsBinaryThinning(){};
  ~ITKToolsBinaryThinning(){};

  static Self * New( itktools::ComponentType componentType, unsigned int imageDimension )
  {
    if ( itktools::IsType<TComponentType>(componentType) && VImageDimension == imageDimension )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedef's. */
    typedef itk::Image<TComponentType, VImageDimension>     InputImageType;
    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::BinaryThinningImageFilter<
      InputImageType, InputImageType >                      FilterType;
    typedef itk::ImageFileWriter< InputImageType >          WriterType;

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

}; // end BinaryThinning

#endif // end __binarythinning_h
