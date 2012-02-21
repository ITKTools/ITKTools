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
#ifndef __extracteveryotherslice_h_
#define __extracteveryotherslice_h_

#include "ITKToolsBase.h"

#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsExtractEveryOtherSliceBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsExtractEveryOtherSliceBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsExtractEveryOtherSliceBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_EveryOther = 0;
    this->m_Offset = 0;
    this->m_Direction = 0;
  };
  /** Destructor. */
  ~ITKToolsExtractEveryOtherSliceBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  unsigned int m_EveryOther;
  unsigned int m_Offset;
  unsigned int m_Direction;

}; // end class ITKToolsExtractEveryOtherSliceBase


/** \class ITKToolsExtractEveryOtherSlice
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsExtractEveryOtherSlice : public ITKToolsExtractEveryOtherSliceBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsExtractEveryOtherSlice Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsExtractEveryOtherSlice(){};
  ~ITKToolsExtractEveryOtherSlice(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TComponentType, VDimension >    InputImageType;
    typedef itk::ImageSliceConstIteratorWithIndex<
      InputImageType >                                  SliceConstIteratorType;
    typedef itk::ImageSliceIteratorWithIndex<
      InputImageType >                                  SliceIteratorType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< InputImageType >      WriterType;
    typedef typename InputImageType::RegionType         RegionType;
    typedef typename RegionType::IndexType              IndexType;
    typedef typename InputImageType::SizeType           SizeType;

    /** Read in the inputImage. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Define size of output image. */
    SizeType sizeIn = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    SizeType sizeOut = sizeIn;
    float newSize = vcl_ceil(
      ( static_cast<float>( sizeOut[ this->m_Direction ] - this->m_Offset ) )
      / static_cast<float>( this->m_EveryOther ) );
    sizeOut[ this->m_Direction ] = static_cast<unsigned int>( newSize );

    /** Define region of output image. */
    RegionType region;
    region.SetIndex( reader->GetOutput()->GetLargestPossibleRegion().GetIndex() );
    region.SetSize( sizeOut );

    /** Create output image. */
    typename InputImageType::Pointer outputImage = InputImageType::New();
    outputImage->SetSpacing( reader->GetOutput()->GetSpacing() );
    outputImage->SetOrigin( reader->GetOutput()->GetOrigin() );
    outputImage->SetRegions( region );
    outputImage->Allocate();

    /** Create iterators. */
    SliceConstIteratorType itIn( reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion() );
    SliceIteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );

    /** Set direction, default slice = z. */
    if( this->m_Direction == 0 )
    {
      itIn.SetFirstDirection(1);
      itIn.SetSecondDirection(2);
      itOut.SetFirstDirection(1);
      itOut.SetSecondDirection(2);
    }
    else if( this->m_Direction == 1 )
    {
      itIn.SetFirstDirection(0);
      itIn.SetSecondDirection(2);
      itOut.SetFirstDirection(0);
      itOut.SetSecondDirection(2);
    }
    else if( this->m_Direction == 2 )
    {
      itIn.SetFirstDirection(0);
      itIn.SetSecondDirection(1);
      itOut.SetFirstDirection(0);
      itOut.SetSecondDirection(1);
    }

    /** Initialize iterators. */
    itIn.GoToBegin();
    itOut.GoToBegin();
    IndexType index= itIn.GetIndex();
    index[ this->m_Direction ] += this->m_Offset;
    itIn.SetIndex( index );

    /** Loop over images. */
    while( !itOut.IsAtEnd() )
    {
      while( !itOut.IsAtEndOfSlice() )
      {
        while( !itOut.IsAtEndOfLine() )
        {
          itOut.Set( itIn.Get() );
          ++itIn;
          ++itOut;
        }
        itIn.NextLine();
        itOut.NextLine();
      }
      itIn.NextSlice();
      itOut.NextSlice();

      /** Skip some slices in inputImage. */
      index = itIn.GetIndex();
      for( unsigned int i = 1; i < this->m_EveryOther; i++ )
      {
        index[ this->m_Direction ]++;
      }
      itIn.SetIndex( index );
    } // end while

    /** Write the output image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( outputImage );
    writer->Update();

  } // end Run()

}; // end class ITKToolsExtractEveryOtherSlice


#endif // end #ifndef __extracteveryotherslice_h_
