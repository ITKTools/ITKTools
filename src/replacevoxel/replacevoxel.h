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
#ifndef __replacevoxel_h_
#define __replacevoxel_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsReplaceVoxelBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsReplaceVoxelBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsReplaceVoxelBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Value = 0.0f;
  };
  /** Destructor. */
  ~ITKToolsReplaceVoxelBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Voxel;
  double m_Value;

}; // end class ITKToolsReplaceVoxelBase


/** \class ITKToolsReplaceVoxel
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsReplaceVoxel : public ITKToolsReplaceVoxelBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsReplaceVoxel Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsReplaceVoxel(){};
  ~ITKToolsReplaceVoxel(){};

  /** Run function. */
  void Run( void )
  {
    typedef TComponentType                        PixelType;
    typedef itk::Image< PixelType, VDimension >   ImageType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::IndexType         IndexType;
    typedef itk::ImageFileReader< ImageType >     ReaderType;
    typedef itk::ImageFileWriter< ImageType >     WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Read input image. */
    reader->SetFileName( this->m_InputFileName );
    reader->Update();
    typename ImageType::Pointer image = reader->GetOutput();

    /** Check size. */
    SizeType size = image->GetLargestPossibleRegion().GetSize();
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      if( this->m_Voxel[ i ] < 0 || this->m_Voxel[ i ] > size[ i ] - 1 )
      {
        itkGenericExceptionMacro( << "ERROR: invalid voxel index." );
      }
    }

    /** Set the value to the voxel. */
    IndexType index;
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      index[ i ] = this->m_Voxel[ i ];
    }
    image->SetPixel( index, static_cast<PixelType>( this->m_Value ) );

    /** Write output image. */
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( image );
    writer->Update();

  } // end Run()

}; // end class ITKToolsReplaceVoxel

#endif // end #ifndef __replacevoxel_h_
