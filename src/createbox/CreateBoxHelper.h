#ifndef __CreateBoxHelper_h
#define __CreateBoxHelper_h

#include <iostream>
#include "CommandLineArgumentHelper.h"

#include "itkBoxSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "vnl/vnl_math.h"


/**
 * ******************* GetHelpText *******************
 */

std::string GetHelpText( void )
{
  std::stringstream helpText( "" );
  helpText << "Usage:" << std::endl << "pxcreatebox\n"
    << "  -out     outputFilename\n"
    << "  Arguments to specify the output image:\n"
    << "  [-in]    inputFilename, to copy image information from\n"
    << "  [-sz]    image size (voxels)\n"
    << "  [-sp]    image spacing (mm), default 1.0\n"
    << "  [-io]    image origin, default 0.0\n"
    << "  [-d]     image direction, default identity\n"
    << "  [-dim]   dimension, default 3\n"
    << "  [-pt]    pixelType, default short\n"
    << "  Arguments to specify the box:\n"
    << "  [-c]     center (mm)\n"
    << "  [-r]     radii (mm)\n"
    << "  [-cp1]   cornerpoint 1 (mm)\n"
    << "  [-cp2]   cornerpoint 2 (mm)\n"
    << "  [-ci1]   cornerindex 1\n"
    << "  [-ci2]   cornerindex 2\n"
    << "  [-o]     orientation of the box, default xyz\n"
    << "- The user should EITHER specify the input filename OR the output image size.\n"
    << "- The user should EITHER specify the center and the radius,\n"
    << "    OR the positions of two opposite corner points.\n"
    << "    OR the positions of two opposite corner indices.\n"
    << "- The orientation is a vector with Euler angles (rad).\n"
    << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

  return helpText.str();

} // end GetHelpText()


/**
 * ******************* CreateBox *******************
 */

template< class ImageType >
void CreateBox(
  itk::ImageIOBase::Pointer & referenceIOBase,
  std::string outputFileName,
  std::vector<double> input1,
  std::vector<double> input2,
  std::string input,
  std::vector<double> orientationOfBox )
{
  /** Typedefs. */
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileWriter< ImageType >           ImageWriterType;
  typedef itk::BoxSpatialFunction< Dimension >        BoxSpatialFunctionType;
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
  GetImageInformationFromImageIOBase(
    referenceIOBase, dummyPixelTypeAsString, dummyComponentTypeAsString,
    dummyDimension, dummyNumberOfComponents,
    size, spacing, origin, direction );

  /** Convert image information to ITK type. */
  SizeType    sizeITK;
  SpacingType spacingITK;
  OriginType  originITK;
  DirectionType directionITK;
  ConvertImageInformationToITKTypes<Dimension>(
    size, spacing, origin, direction,
    sizeITK, spacingITK, originITK, directionITK );

  /** Create image. */
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
  if ( input == "CornersAsPoints" )
  {
    /** The input is points, copy it. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      point1[ i ] = input1[ i ];
      point2[ i ] = input2[ i ];
    }
  }
  else if ( input == "CornersAsIndices" )
  {
    /** The input is indices, copy and transform to the point. */
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      index1[ i ] = static_cast<unsigned int>( input1[ i ] );
      index2[ i ] = static_cast<unsigned int>( input2[ i ] );
    }
    image->TransformIndexToPhysicalPoint( index1, point1 );
    image->TransformIndexToPhysicalPoint( index2, point2 );
  }

  /** Compute the center and radius. */
  if ( input != "CenterRadius" )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = ( point1[ i ] + point2[ i ] ) / 2.0;
      Radius[ i ] = spacingITK[ i ] + vcl_abs( point1[ i ] - Center[ i ] );
    }
  }
  else
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      Center[ i ] = point1[ i ];
      Radius[ i ] = point2[ i ];
    }
  }

  /** Convert box orientation to ITK type. */
  InputType   orientationOfBoxITK;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    orientationOfBoxITK[ i ] = orientationOfBox[ i ];
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
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( image );
  writer->Update();

} // end CreateBox()


#endif // end #ifndef __CreateBoxHelper_h
