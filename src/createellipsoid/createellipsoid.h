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
#ifndef __createellipsoid_h_
#define __createellipsoid_h_

#include "ITKToolsBase.h"

#include "itkEllipsoidInteriorExteriorSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsCreateEllipsoidBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCreateEllipsoidBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsCreateEllipsoidBase()
  {
    this->m_OutputFileName = "";
  }
  /** Destructor. */
  ~ITKToolsCreateEllipsoidBase(){};

  /** Input member parameters. */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Center;
  std::vector<double> m_Radius;
  std::vector<double> m_Orientation;
    
}; // end class ITKToolsCreateEllipsoidBase


/** \class ITKToolsCreateEllipsoid
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCreateEllipsoid : public ITKToolsCreateEllipsoidBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCreateEllipsoid Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCreateEllipsoid(){};
  ~ITKToolsCreateEllipsoid(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TComponentType, VDimension >      ImageType;
    typedef itk::ImageRegionIterator< ImageType >         IteratorType;
    typedef itk::EllipsoidInteriorExteriorSpatialFunction<
      VDimension >                                        SpatialFunctionType;
    typedef typename SpatialFunctionType::InputType       InputType;
    typedef typename SpatialFunctionType::OrientationType OrientationType;
    typedef itk::ImageFileWriter< ImageType >             ImageWriterType;

    typedef typename ImageType::RegionType                RegionType;
    typedef typename RegionType::SizeType                 SizeType;
    typedef typename RegionType::SizeValueType            SizeValueType;
    typedef typename ImageType::PointType                 PointType;
    typedef typename ImageType::IndexType                 IndexType;
    typedef typename ImageType::SpacingType               SpacingType;

    /** Parse the arguments. */
    SizeType Size;
    SpacingType Spacing;
    InputType Center;
    InputType Radius;
    OrientationType Orientation;
    for( unsigned int i = 0; i < VDimension; i++ )
    {
      Size[ i ] = static_cast<SizeValueType>( this->m_Size[ i ] );
      Spacing[ i ] = this->m_Spacing[ i ];
      Center[ i ] = this->m_Center[ i ];
      Radius[ i ] = this->m_Radius[ i ];
      for( unsigned int j = 0; j < VDimension; j++ )
      {
        Orientation[ i ][ j ] = this->m_Orientation[ i * VDimension + j ];
      }
    }

    /** Create image. */
    RegionType region;
    region.SetSize( Size );
    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions( region );
    image->SetSpacing( Spacing );
    image->Allocate();

    /** Create and initialize ellipsoid. */
    typename SpatialFunctionType::Pointer ellipsoid = SpatialFunctionType::New();
    ellipsoid->SetCenter( Center );
    ellipsoid->SetAxes( Radius );
    ellipsoid->SetOrientations( Orientation );

    /** Create iterator, index and point. */
    IteratorType it( image, region );
    it.GoToBegin();
    PointType point;
    IndexType index;

    /** Walk over the image. */
    while ( !it.IsAtEnd() )
    {
      index = it.GetIndex();
      image->TransformIndexToPhysicalPoint( index, point );
      it.Set( ellipsoid->Evaluate( point ) );
      ++it;
    } // end while

    /** Write image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCreateEllipsoid


#endif // end #ifndef __createellipsoid_h_
