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
#ifndef __TileImages_h_
#define __TileImages_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkTileImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsTileImagesBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsTileImagesBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsTileImagesBase()
  {
    this->m_OutputFileName = "";
    this->m_Defaultvalue = 0.0;
  };
  /** Destructor. */
  ~ITKToolsTileImagesBase(){};

  /** Input member parameters. */
  std::vector<std::string>  m_InputFileNames;
  std::string               m_OutputFileName;
  std::vector<unsigned int> m_Layout;
  double                    m_Defaultvalue;

}; // end class ITKToolsTileImagesBase


/** \class ITKToolsTileImages
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsTileImages : public ITKToolsTileImagesBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsTileImages Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsTileImages(){};
  ~ITKToolsTileImages(){};

  /** Run function. */
  void Run( void )
  {
  /** Some typedef's. */
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef itk::ImageFileReader<ImageType>             ImageReaderType;
    typedef itk::TileImageFilter<ImageType, ImageType>  TilerType;
    typedef itk::ImageFileWriter<ImageType>             ImageWriterType;

    /** Copy layout into a fixed array. */
    itk::FixedArray< unsigned int, VDimension > Layout;
    for( unsigned int i = 0; i < VDimension; i++ )
    {
      Layout[ i ] = this->m_Layout[ i ];
    }

    /** Cast the defaultvalue. */
    TComponentType defaultValue = static_cast<TComponentType>( this->m_Defaultvalue );

    /** Create tiler. */
    typename TilerType::Pointer tiler = TilerType::New();
    tiler->SetLayout( Layout );
    tiler->SetDefaultPixelValue( defaultValue );

    /** Read input images and set it into the tiler. */
    for( unsigned int i = 0; i < this->m_InputFileNames.size(); i++ )
    {
      typename ImageReaderType::Pointer reader = ImageReaderType::New();
      reader->SetFileName( this->m_InputFileNames[ i ].c_str() );
      reader->Update();
      tiler->SetInput( i, reader->GetOutput() );
    }

    /** Write to disk. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( tiler->GetOutput() );
    writer->Update();

  }// end Run()

}; // end class ITKToolsTileImages

#endif
