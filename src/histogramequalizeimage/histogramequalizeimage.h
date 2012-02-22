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
#ifndef __histogramequalizeimage_h_
#define __histogramequalizeimage_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkHistogramEqualizationImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsHistogramEqualizeImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsHistogramEqualizeImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsHistogramEqualizeImageBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_MaskFileName = "";
  };
  /** Destructor. */
  ~ITKToolsHistogramEqualizeImageBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_MaskFileName;

}; // end class ITKToolsHistogramEqualizeImageBase


/** \class ITKToolsHistogramEqualizeImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsHistogramEqualizeImage : public ITKToolsHistogramEqualizeImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsHistogramEqualizeImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsHistogramEqualizeImage(){};
  ~ITKToolsHistogramEqualizeImage(){};

  /** Run function. */
  void Run( void )
  {
    typedef itk::Image< TComponentType, VDimension >  ImageType;
    typedef itk::Image< unsigned char, VDimension >   MaskImageType;
    typedef typename ImageType::Pointer             ImagePointer;
    typedef typename ImageType::IndexType           IndexType;
    typedef typename ImageType::SizeType            SizeType;
    typedef typename ImageType::RegionType          RegionType;
    typedef typename ImageType::PointType           PointType;
    typedef itk::ImageFileReader<ImageType>         ReaderType;
    typedef itk::ImageFileReader<MaskImageType>     MaskReaderType;
    typedef itk::ImageFileWriter<ImageType>         WriterType;
    typedef typename ReaderType::Pointer            ReaderPointer;
    typedef typename WriterType::Pointer            WriterPointer;
    typedef itk::HistogramEqualizationImageFilter<
      ImageType>                                    EnhancerType;
    typedef typename EnhancerType::Pointer          EnhancerPointer;

    WriterPointer writer = WriterType::New();
    EnhancerPointer enhancer = EnhancerType::New();

    /** Try to read input image */
    ReaderPointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Try to read mask image */
    typename MaskReaderType::Pointer maskReader;
    if( this->m_MaskFileName != "" )
    {
      maskReader = MaskReaderType::New();
      maskReader->SetFileName( this->m_MaskFileName.c_str() );
      maskReader->Update();
    }

    /** Setup pipeline and configure its components */
    enhancer->SetInput( reader->GetOutput() );
    if( this->m_MaskFileName != "" )
    {
      enhancer->SetMask( maskReader->GetOutput() );
    }
    writer->SetInput( enhancer->GetOutput() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsHistogramEqualizeImage


#endif // #ifndef __histogramequalizeimage_h_
