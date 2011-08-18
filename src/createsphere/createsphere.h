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
 \brief Create a sphere image.
 
 \verbinclude createsphere.help
 */

#ifndef __createsphere_h
#define __createsphere_h

#include "ITKToolsBase.h"

#include "itkSphereSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/** CreateSphere */

class ITKToolsCreateSphereBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsCreateSphereBase()
  {
    this->m_OutputFileName = "";
    //this->m_Size;
    //this->m_Spacing;
    //this->m_Center;
    this->m_Radius = 0.0f;
  };
  ~ITKToolsCreateSphereBase(){};

  /** Input parameters */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Center;
  double m_Radius;
    
}; // end CreateSphereBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsCreateSphere : public ITKToolsCreateSphereBase
{
public:
  typedef ITKToolsCreateSphere Self;

  ITKToolsCreateSphere(){};
  ~ITKToolsCreateSphere(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>  ImageType;
    typedef itk::ImageRegionIterator< ImageType >   IteratorType;
    typedef itk::SphereSpatialFunction< VDimension > SphereSpatialFunctionType;
    typedef typename SphereSpatialFunctionType::InputType    InputType;
    typedef itk::ImageFileWriter< ImageType >       ImageWriterType;

    typedef typename ImageType::RegionType      RegionType;
    typedef typename RegionType::SizeType     SizeType;
    typedef typename RegionType::SizeValueType  SizeValueType;
    typedef typename ImageType::PointType     PointType;
    typedef typename ImageType::IndexType     IndexType;
    typedef typename ImageType::SpacingType   SpacingType;

    /** Parse the arguments. */
    SizeType    Size;
    SpacingType Spacing;
    InputType   Center;
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      Size[ i ] = static_cast<SizeValueType>( this->m_Size[ i ] );
      Spacing[ i ] = this->m_Spacing[ i ];
      Center[ i ] = static_cast<double>( this->m_Center[ i ] );
    }

    /** Create image. */
    RegionType region;
    region.SetSize( Size );
    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions( region );
    image->SetSpacing( Spacing );
    image->Allocate();

    /** Create and initialize ellipsoid. */
    typename SphereSpatialFunctionType::Pointer sphere = SphereSpatialFunctionType::New();
    sphere->SetCenter( Center );
    sphere->SetRadius( this->m_Radius );

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
      it.Set( sphere->Evaluate( point ) );
      ++it;
    }

    /** Write image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();
  }

}; // end CreateSphere

#endif // end #ifndef __createsphere_h
