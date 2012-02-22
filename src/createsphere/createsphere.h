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
#ifndef __createsphere_h_
#define __createsphere_h_

#include "ITKToolsBase.h"

#include "itkSphereSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsCreateSphereBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCreateSphereBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCreateSphereBase()
  {
    this->m_OutputFileName = "";
    this->m_Radius = 0.0f;
  };
  /** Destructor. */
  ~ITKToolsCreateSphereBase(){};

  /** Input member parameters. */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Center;
  double m_Radius;

}; // end class ITKToolsCreateSphereBase


/** \class ITKToolsCreateSphere
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCreateSphere : public ITKToolsCreateSphereBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCreateSphere Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCreateSphere(){};
  ~ITKToolsCreateSphere(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>        ImageType;
    typedef itk::ImageRegionIterator< ImageType >         IteratorType;
    typedef itk::SphereSpatialFunction< VDimension >      SphereSpatialFunctionType;
    typedef typename SphereSpatialFunctionType::InputType InputType;
    typedef itk::ImageFileWriter< ImageType >             ImageWriterType;

    typedef typename ImageType::RegionType                RegionType;
    typedef typename RegionType::SizeType                 SizeType;
    typedef typename RegionType::SizeValueType            SizeValueType;
    typedef typename ImageType::PointType                 PointType;
    typedef typename ImageType::IndexType                 IndexType;
    typedef typename ImageType::SpacingType               SpacingType;

    /** Parse the arguments. */
    SizeType    Size;
    SpacingType Spacing;
    InputType   Center;
    for( unsigned int i = 0; i < VDimension; i++ )
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

  } // end Run()

}; // end class ITKToolsCreateSphere

#endif // end #ifndef __createsphere_h_
