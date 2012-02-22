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
#ifndef __imagestovectorimage_h_
#define __imagestovectorimage_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"


/** \class ITKToolsImagesToVectorImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsImagesToVectorImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsImagesToVectorImageBase()
  {
    this->m_OutputFileName = "";
    this->m_NumberOfStreams = 0;
  };
  /** Destructor. */
  ~ITKToolsImagesToVectorImageBase(){};

  /** Input member parameters. */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  unsigned int m_NumberOfStreams;

}; // end class ITKToolsImagesToVectorImageBase


/** \class ITKToolsImagesToVectorImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsImagesToVectorImage : public ITKToolsImagesToVectorImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsImagesToVectorImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsImagesToVectorImage(){};
  ~ITKToolsImagesToVectorImage(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedef's. */
    typedef itk::VectorImage< TComponentType, VDimension >    VectorImageType;
    typedef VectorImageType                                   OutputImageType;
    typedef itk::ImageFileReader< VectorImageType >           ReaderType;
    typedef itk::Image< TComponentType, VDimension >          ScalarImageType;
    typedef itk::ImageFileWriter< VectorImageType >           WriterType;

    /** Read in the input images. */
    std::vector<typename ReaderType::Pointer> readers( this->m_InputFileNames.size() );
    for( unsigned int i = 0; i < this->m_InputFileNames.size(); ++i )
    {
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( this->m_InputFileNames[ i ] );
      readers[ i ]->Update();
    }

    /** Create assembler. */
    typedef itk::ImageToVectorImageFilter< ScalarImageType > ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter
      = ImageToVectorImageFilterType::New();

    // For each input image
    std::cout << "There are " << this->m_InputFileNames.size() << " input images." << std::endl;
    unsigned int currentOutputIndex = 0;
    for( unsigned int inputImageIndex = 0; inputImageIndex < this->m_InputFileNames.size(); ++inputImageIndex )
    {
      typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;

      // For each component of the current image
      std::cout << "There are " << readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel()
        << " components in image " << inputImageIndex << std::endl;
      for( unsigned int component = 0;
        component < readers[inputImageIndex]->GetOutput()->GetNumberOfComponentsPerPixel(); ++component )
      {
        typename ComponentExtractionType::Pointer componentExtractionFilter = ComponentExtractionType::New();
        componentExtractionFilter->SetIndex( component );
        componentExtractionFilter->SetInput( readers[inputImageIndex]->GetOutput() );
        componentExtractionFilter->Update();

        imageToVectorImageFilter->SetNthInput( currentOutputIndex, componentExtractionFilter->GetOutput());
        currentOutputIndex++;
      }
    }

    imageToVectorImageFilter->Update();

    std::cout << "Output image has "
      << imageToVectorImageFilter->GetOutput()->GetNumberOfComponentsPerPixel()
      << " components." << std::endl;

    /** Write vector image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetNumberOfStreamDivisions( this->m_NumberOfStreams );
    writer->Update();

  } // end Run()

}; // end class ITKToolsImagesToVectorImage


#endif // end #ifndef __imagestovectorimage_h_
