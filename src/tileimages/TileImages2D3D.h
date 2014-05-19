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
#ifndef __TileImages2D3D_h_
#define __TileImages2D3D_h_

#include "ITKToolsBase.h"

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"


/** \class TileImages2D3DBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsTileImages2D3DBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsTileImages2D3DBase()
  {
    this->m_OutputFileName = "";
    this->m_LastSpacing = 1.0;
  };
  /** Destructor. */
  ~ITKToolsTileImages2D3DBase(){};

  /** Input member parameters. */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  double m_LastSpacing;

}; // end class ITKToolsTileImages2D3DBase


/** \class ITKToolsTileImages2D3D
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsTileImages2D3D : public ITKToolsTileImages2D3DBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsTileImages2D3D Self;

  itktoolsOneTypeNewMacro( Self );

  ITKToolsTileImages2D3D(){};
  ~ITKToolsTileImages2D3D(){};

  /** Run function. */
  void Run( void )
  {
    /** Some typedef's. */
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef typename ImageType::SpacingType             SpacingType;
    typedef itk::ImageSeriesReader<ImageType>           ImageSeriesReaderType;
    typedef itk::ImageFileWriter<ImageType>             ImageWriterType;

    /** Create reader. */
    typename ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
    reader->SetFileNames( this->m_InputFileNames );

    /** Update the reader. */
    reader->Update();
    typename ImageType::Pointer tiledImage = reader->GetOutput();

    /** Get and set the spacing, if it was set by the user. */
    if( this->m_LastSpacing > 0.0 )
    {
      /** Make sure that changes are not undone. */
      tiledImage->DisconnectPipeline();

      /** Set the spacing of the last dimension. */
      SpacingType spacing = tiledImage->GetSpacing();
      spacing[ VDimension - 1 ] = this->m_LastSpacing;
      tiledImage->SetSpacing( spacing );
    }

    /** Write to disk. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( tiledImage );
    writer->Update();

  } // end Run()

}; // end class ITKToolsTileImages2D3D


#endif
