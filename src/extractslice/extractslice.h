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
#ifndef __extractslice_h_
#define __extractslice_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"

#include <string>
#include <vector>


/** \class ITKToolsExtractSliceBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsExtractSliceBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsExtractSliceBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Slicenumber = 0;
    this->m_WhichDimension = 0;
    this->m_UseCompression = false;
  };
  /** Destructor. */
  ~ITKToolsExtractSliceBase(){};

  /** Input member parameters. */
  std::string  m_InputFileName;
  std::string  m_OutputFileName;
  unsigned int m_Slicenumber;
  unsigned int m_WhichDimension;
  bool         m_UseCompression;

}; // end class ITKToolsExtractSliceBase


/** \class ITKToolsExtractSlice
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsExtractSlice : public ITKToolsExtractSliceBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsExtractSlice Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsExtractSlice(){};
  ~ITKToolsExtractSlice(){};

  /** Run function. */
  void Run( void )
  {
    /** Some typedef's. */
    typedef itk::Image< TComponentType, VDimension >      Image3DType;
    typedef itk::Image< TComponentType, VDimension - 1 >  Image2DType;
    typedef itk::ImageFileReader<Image3DType>             ImageReaderType;
    typedef itk::ExtractImageFilter<
      Image3DType, Image2DType >                          ExtractFilterType;
    typedef itk::ImageFileWriter<Image2DType>             ImageWriterType;

    typedef typename Image3DType::RegionType  RegionType;
    typedef typename Image3DType::SizeType    SizeType;
    typedef typename Image3DType::IndexType   IndexType;

    /** Create reader. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Create extractor. */
    typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
    extractor->SetInput( reader->GetOutput() );

    /** Get the size and set which_dimension to zero. */
    RegionType inputRegion = reader->GetOutput()->GetLargestPossibleRegion();
    SizeType size = inputRegion.GetSize();
    size[ this->m_WhichDimension ] = 0;

    /** Get the index and set which_dimension to the correct slice. */
    IndexType start = inputRegion.GetIndex();
    start[ this->m_WhichDimension ] = this->m_Slicenumber;

    /** Create a desired extraction region and set it into the extractor. */
    RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extractor->SetExtractionRegion( desiredRegion );

    /** The direction cosines of the 2D extracted data is set to
     * a submatrix of the 3D input image. */
    extractor->SetDirectionCollapseToSubmatrix();

    /** Write the 2D output image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( extractor->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();

  } // end Run()

}; // end class ITKToolsExtractSlice


#endif // end #ifndef __extractslice_h_
