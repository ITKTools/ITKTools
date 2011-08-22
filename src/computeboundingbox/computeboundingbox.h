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
/** \file
 \brief Compute the bounding box of an image.

 \verbinclude computeboundingbox.help
 */

#ifndef __computeboundingbox_h
#define __computeboundingbox_h

#include "ITKToolsBase.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "vnl/vnl_math.h"


/** ComputeBoundingBox */

class ITKToolsComputeBoundingBoxBase : public itktools::ITKToolsBase
{
public:
  ITKToolsComputeBoundingBoxBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  }
  ~ITKToolsComputeBoundingBoxBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end ComputeBoundingBoxBase


template< class TComponentType, unsigned int VImageDimension >
class ITKToolsComputeBoundingBox : public ITKToolsComputeBoundingBoxBase
{
public:
  typedef ITKToolsComputeBoundingBox Self;

  ITKToolsComputeBoundingBox(){};
  ~ITKToolsComputeBoundingBox(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VImageDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VImageDimension> InputImageType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageRegionConstIteratorWithIndex<
      InputImageType>                                   IteratorType;
    typedef typename InputImageType::PixelType          PixelType;
    typedef typename InputImageType::IndexType          IndexType;
    typedef typename InputImageType::PointType          PointType;
    const unsigned int dimension = InputImageType::GetImageDimension();

    /** Declarations */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename InputImageType::Pointer image;
    IndexType minIndex;
    IndexType maxIndex;

    /** Read input image */
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();
    image = reader->GetOutput();

    /** Define iterator on input image */
    IteratorType iterator( image, image->GetLargestPossibleRegion() );

    /** Initialize the two corner points */
    iterator.GoToReverseBegin();
    minIndex = iterator.GetIndex();
    iterator.GoToBegin();
    maxIndex = iterator.GetIndex();
    PixelType zero = itk::NumericTraits< PixelType>::Zero;

    while ( ! iterator.IsAtEnd() )
    {
      if ( iterator.Get() > zero )
      {
        const IndexType & index = iterator.GetIndex();
        for ( unsigned int i = 0; i < dimension; ++i)
        {
          minIndex[i] = vnl_math_min( index[i], minIndex[i] );
          maxIndex[i] = vnl_math_max( index[i], maxIndex[i] );
        }
      }
      ++iterator;
    }

    PointType minPoint;
    PointType maxPoint;
    image->TransformIndexToPhysicalPoint(minIndex, minPoint);
    image->TransformIndexToPhysicalPoint(maxIndex, maxPoint);

    std::cout << "MinimumIndex = " << minIndex << "\n"
	      << "MaximumIndex = " << maxIndex << std::endl;
    std::cout << std::showpoint;
    std::cout << "MinimumPoint = " << minPoint << "\n"
	      << "MaximumPoint = " << maxPoint << std::endl;

  } // end Run()

}; // end ComputeBoundingBox

#endif // end #ifndef __computeboundingbox_h

