#ifndef __CreateBoxHelper_h
#define __CreateBoxHelper_h

#include <iostream>
#include "CommandLineArgumentHelper.h"

#include "itkBoxSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "vnl/vnl_math.h"


/** CreateBox */

class ITKToolsCreateBoxBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsCreateBoxBase()
  {
    //m_InputFileName = "";
    this->m_ReferenceImageIOBase = NULL;
    this->m_OutputFileName = "";
    this->m_BoxDefinition = "";
  }

  ~ITKToolsCreateBoxBase(){};

  /** Input parameters */
  itk::ImageIOBase::Pointer m_ReferenceImageIOBase;
  std::string m_OutputFileName;
  std::vector<double> m_Input1;
  std::vector<double> m_Input2;
  std::vector<double> m_OrientationOfBox;
  std::string m_BoxDefinition;

}; // end CreateBoxBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsCreateBox : public ITKToolsCreateBoxBase
{
public:
  typedef ITKToolsCreateBox Self;

  ITKToolsCreateBox(){};
  ~ITKToolsCreateBox(){};

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
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef itk::ImageFileWriter< ImageType >           ImageWriterType;
    typedef itk::BoxSpatialFunction< VDimension >        BoxSpatialFunctionType;
    typedef typename BoxSpatialFunctionType::InputType  InputType;
    typedef itk::ImageRegionIterator< ImageType >       IteratorType;

    typedef typename ImageType::RegionType              RegionType;
    typedef typename RegionType::SizeType               SizeType;
    typedef typename RegionType::SizeValueType          SizeValueType;
    typedef typename ImageType::PointType               PointType;
    typedef typename ImageType::IndexType               IndexType;
    typedef typename ImageType::SpacingType             SpacingType;
    typedef typename ImageType::PointType               OriginType;
    typedef typename ImageType::DirectionType           DirectionType;

    /** Parse the arguments: output image information. */
    std::string dummyPixelTypeAsString = "scalar";
    std::string dummyComponentTypeAsString = "short";
    unsigned int dummyDimension = 3;
    unsigned int dummyNumberOfComponents = 1;
    std::vector<unsigned int> size;
    std::vector<double> spacing, origin, direction;
    itktools::GetImageInformationFromImageIOBase(
      this->m_ReferenceImageIOBase,
      dummyPixelTypeAsString, dummyComponentTypeAsString,
      dummyDimension, dummyNumberOfComponents,
      size, spacing, origin, direction );

    /** Convert image information to ITK type. */
    SizeType    sizeITK;
    sizeITK.Fill( itk::NumericTraits<SizeValueType>::Zero ); // kill anoying warning
    SpacingType spacingITK;
    OriginType  originITK;
    DirectionType directionITK;
    ConvertImageInformationToITKTypes<VDimension>(
      size, spacing, origin, direction,
      sizeITK, spacingITK, originITK, directionITK );

    /** Create output image. */
    typename ImageType::Pointer image = ImageType::New();
    RegionType region; region.SetSize( sizeITK );
    image->SetRegions( region );
    image->SetSpacing( spacingITK );
    image->SetOrigin( originITK );
    image->SetDirection( directionITK );
    image->Allocate();

    /** Translate input of two opposite corners to center + radius input. */
    InputType Center, Radius;
    PointType point1, point2;
    IndexType index1, index2;
    if ( this->m_BoxDefinition == "CornersAsPoints" )
    {
      /** The input is points, copy it. */
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
        point1[ i ] = this->m_Input1[ i ];
        point2[ i ] = this->m_Input2[ i ];
      }
    }
    else if ( this->m_BoxDefinition == "CornersAsIndices" )
    {
      /** The input is indices, copy and transform to the point. */
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
        index1[ i ] = static_cast<unsigned int>( this->m_Input1[ i ] );
        index2[ i ] = static_cast<unsigned int>( this->m_Input2[ i ] );
      }
      image->TransformIndexToPhysicalPoint( index1, point1 );
      image->TransformIndexToPhysicalPoint( index2, point2 );
    }

    /** Compute the center and radius. */
    if ( this->m_BoxDefinition != "CenterRadius" )
    {
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
        Center[ i ] = ( point1[ i ] + point2[ i ] ) / 2.0;
        Radius[ i ] = spacingITK[ i ] + vcl_abs( point1[ i ] - Center[ i ] );
      }
    }
    else
    {
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
        Center[ i ] = point1[ i ];
        Radius[ i ] = point2[ i ];
      }
    }

    /** Convert box orientation to ITK type. */
    InputType   orientationOfBoxITK;
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      orientationOfBoxITK[ i ] = this->m_OrientationOfBox[ i ];
    }

    /** Create and initialize box. */
    typename BoxSpatialFunctionType::Pointer box = BoxSpatialFunctionType::New();
    box->SetCenter( Center );
    box->SetRadius( Radius );
    box->SetOrientation( orientationOfBoxITK );

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
      it.Set( box->Evaluate( point ) );
      ++it;
    } // end while

    /** Write image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();

  } // end Run()

}; // end CreateBox


#endif // end #ifndef __CreateBoxHelper_h
