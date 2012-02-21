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
#ifndef __extractindexfromvectorimage_h_
#define __extractindexfromvectorimage_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"


/** \class ITKToolsExtractIndexBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsExtractIndexBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsExtractIndexBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsExtractIndexBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Indices;

}; // end class ITKToolsExtractIndexBase


/** \class ITKToolsExtractIndex
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsExtractIndex : public ITKToolsExtractIndexBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsExtractIndex Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsExtractIndex(){};
  ~ITKToolsExtractIndex(){};

  /** Run function. */
  void Run( void )
  {
    /** Use vector image type that dynamically determines vector length: */
    typedef itk::VectorImage< TComponentType, VDimension >  VectorImageType;
    typedef itk::Image< TComponentType, VDimension >        ScalarImageType;
    typedef itk::ImageFileReader< VectorImageType >         ImageReaderType;
    typedef itk::VectorIndexSelectionCastImageFilter<
      VectorImageType, ScalarImageType >                    IndexExtractorType;
    typedef itk::ImageFileWriter< VectorImageType >         ImageWriterType;

    /** Read input image. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( this->m_InputFileName );
    reader->Update();

    /** Extract indices. */

    // Create the assembler
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    for( unsigned int i = 0; i < this->m_Indices.size(); ++i )
    {
      typename IndexExtractorType::Pointer extractor = IndexExtractorType::New();
      extractor->SetInput( reader->GetOutput() );
      extractor->SetIndex( this->m_Indices[ i ] );
      extractor->Update();
      //extractor->DisconnectPipeline();

      imageToVectorImageFilter->SetNthInput( i, extractor->GetOutput() );
    }

    imageToVectorImageFilter->Update();

    /** Write output image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsExtractIndex


#endif // end #ifndef __extractindexfromvectorimage_h_
