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
#ifndef __createzeroimage_h_
#define __createzeroimage_h_

#include "ITKToolsBase.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsCreateZeroImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCreateZeroImageBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsCreateZeroImageBase()
  {
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsCreateZeroImageBase(){};

  /** Input member parameters. */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Origin;
    
}; // end class ITKToolsCreateZeroImageBase


/** \class ITKToolsCreateZeroImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCreateZeroImage : public ITKToolsCreateZeroImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCreateZeroImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCreateZeroImage(){};
  ~ITKToolsCreateZeroImage(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TComponentType, VDimension >  ImageType;
    typedef itk::ImageFileWriter< ImageType >         WriterType;
    typedef typename ImageType::PixelType             PixelType;
    typedef typename ImageType::SizeType              SizeType;
    typedef typename ImageType::SpacingType           SpacingType;
    typedef typename ImageType::PointType             OriginType;

    /** Prepare stuff. */
    SizeType    imSize;
    SpacingType imSpacing;
    OriginType  imOrigin;
    for( unsigned int i = 0; i < VDimension; i++ )
    {
      imSize[ i ] = this->m_Size[ i ];
      imSpacing[ i ] = this->m_Spacing[ i ];
      imOrigin[ i ] = this->m_Origin[ i ];
    }

    /** Create image. */
    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions( imSize );
    image->SetOrigin( imOrigin );
    image->SetSpacing( imSpacing );
    image->Allocate();
    image->FillBuffer( itk::NumericTraits<PixelType>::Zero );

    /** Write the image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCreateZeroImage


#endif // end #ifndef __createzeroimage_h_
