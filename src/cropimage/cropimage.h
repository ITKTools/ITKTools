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
#ifndef __cropimage_h_
#define __cropimage_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkCropImageFilter.h"
#include "itkConstantPadImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "cropimageMainHelper.h"

/** \class ITKToolsCropImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCropImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCropImageBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Force = false;
    this->m_UseCompression = false;
  };
  /** Destructor. */
  ~ITKToolsCropImageBase(){};

  /** Input member parameters. */
  std::string       m_InputFileName;
  std::string       m_OutputFileName;
  std::vector<int>  m_Input1;
  std::vector<int>  m_Input2;
  unsigned int      m_Option;
  bool              m_Force;
  bool              m_UseCompression;

}; // end class ITKToolsCropImageBase


/** \class ITKToolsCropImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCropImage : public ITKToolsCropImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCropImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCropImage(){};
  ~ITKToolsCropImage(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>                  InputImageType;
    typedef itk::CropImageFilter<InputImageType, InputImageType >   CropImageFilterType;
    typedef itk::ConstantPadImageFilter<
      InputImageType, InputImageType >                              PadFilterType;
    typedef itk::ImageFileReader< InputImageType >                  ReaderType;
    typedef itk::ImageFileWriter< InputImageType >                  WriterType;
    typedef typename InputImageType::SizeType                       SizeType;
    typedef typename InputImageType::SizeValueType                  SizeValueType;

    const unsigned int Dimension = InputImageType::ImageDimension;

    /** Declarations. */
    typename CropImageFilterType::Pointer cropFilter = CropImageFilterType::New();
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Prepare stuff. */
    SizeType input1Size, input2Size;
    for( unsigned int i = 0; i < Dimension; i++ )
    {
      input1Size[ i ] = this->m_Input1[ i ];
      input2Size[ i ] = this->m_Input2[ i ];
    }

    /** Read the image. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Get the size of input image. */
    SizeType imageSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    std::vector<int> imSize( Dimension );
    for( unsigned int i = 0; i < Dimension; i++ )
    {
      imSize[ i ] = static_cast<int>( imageSize[ i ] );
    }

    /** Get the lower and upper boundary. */
    std::vector<unsigned long> padLowerBound, padUpperBound;
    std::vector<int> down = GetLowerBoundary(
      this->m_Input1, Dimension, this->m_Force, padLowerBound );
    std::vector<int> up = GetUpperBoundary(
      this->m_Input1, this->m_Input2, imSize, Dimension,
      this->m_Option, this->m_Force, padUpperBound );
    SizeType downSize, upSize;
    for( unsigned int i = 0; i < Dimension; i++ )
    {
      downSize[ i ] = down[ i ];
      upSize[ i ] = up[ i ];
    }

    /** Set the boundaries for the cropping filter. */
    cropFilter->SetInput( reader->GetOutput() );
    cropFilter->SetLowerBoundaryCropSize( downSize );
    cropFilter->SetUpperBoundaryCropSize( upSize );

    /** In case the force option is set to true, we force the
     * output image to be of the desired size.
     */
    if( this->m_Force )
    {
      SizeValueType uBound[ Dimension ];
      SizeValueType lBound[ Dimension ];
      for( unsigned int i = 0; i < Dimension; i++ )
      {
        lBound[ i ] = padLowerBound[ i ];
        uBound[ i ] = padUpperBound[ i ];
      }
      padFilter->SetPadLowerBound( lBound );
      padFilter->SetPadUpperBound( uBound );
      padFilter->SetInput( cropFilter->GetOutput() );
      writer->SetInput( padFilter->GetOutput() );
    }
    else
    {
      writer->SetInput( cropFilter->GetOutput() );
    }

    /** Setup and process the pipeline. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCropImage


#endif // end #ifndef __cropimage_h_
