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
 \brief Create an ellipsoid image.
 
 \verbinclude createellipsoid.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkEllipsoidInteriorExteriorSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxcreateellipsoid" << std::endl
    << "-out     outputFilename" << std::endl
    << "-sz      image size (voxels)" << std::endl
    << "[-sp]    image spacing (mm)" << std::endl
    << "-c       center (mm)" << std::endl
    << "-r       radii (mm)" << std::endl
    << "[-o]     orientation, default xyz" << std::endl
    << "[-dim]   dimension, default 3" << std::endl
    << "[-pt]    pixelType, default short" << std::endl
  << "The orientation is a dim*dim matrix, specified in row order." << std::endl
  << "The user should take care of supplying an orthogonal matrix." << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";
  return ss.str();
} // end GetHelpString()


/** CreateEllipsoid */

class ITKToolsCreateEllipsoidBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsCreateEllipsoidBase()
  {
    this->m_OutputFileName = "";
    //std::vector<unsigned int> m_Size;
    //std::vector<double> m_Spacing;
    //std::vector<double> m_Center;
    //std::vector<double> m_Radius;
    //std::vector<double> m_Orientation;
  }
  ~ITKToolsCreateEllipsoidBase(){};

  /** Input parameters */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Center;
  std::vector<double> m_Radius;
  std::vector<double> m_Orientation;
    
}; // end CreateEllipsoidBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsCreateEllipsoid : public ITKToolsCreateEllipsoidBase
{
public:
  typedef ITKToolsCreateEllipsoid Self;

  ITKToolsCreateEllipsoid(){};
  ~ITKToolsCreateEllipsoid(){};

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
    typedef itk::Image<TComponentType, VDimension>           ImageType;
    typedef itk::ImageRegionIterator< ImageType >   IteratorType;
    typedef itk::EllipsoidInteriorExteriorSpatialFunction<
      VDimension >                                   EllipsoidSpatialFunctionType;
    typedef typename EllipsoidSpatialFunctionType::InputType InputType;
    typedef typename EllipsoidSpatialFunctionType::OrientationType OrientationType;
    typedef itk::ImageFileWriter< ImageType >       ImageWriterType;

    typedef typename ImageType::RegionType      RegionType;
    typedef typename RegionType::SizeType     SizeType;
    typedef typename RegionType::SizeValueType  SizeValueType;
    typedef typename ImageType::PointType     PointType;
    typedef typename ImageType::IndexType     IndexType;
    typedef typename ImageType::SpacingType   SpacingType;

    /** Parse the arguments. */
    SizeType Size;
    SpacingType Spacing;
    InputType Center;
    InputType Radius;
    OrientationType Orientation;
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      Size[ i ] = static_cast<SizeValueType>( this->m_Size[ i ] );
      Spacing[ i ] = this->m_Spacing[ i ];
      Center[ i ] = this->m_Center[ i ];
      Radius[ i ] = this->m_Radius[ i ];
      for ( unsigned int j = 0; j < VDimension; j++ )
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
    typename EllipsoidSpatialFunctionType::Pointer ellipsoid = EllipsoidSpatialFunctionType::New();
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
  }

}; // end CreateEllipsoid


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-sz", "The size." );
  parser->MarkArgumentAsRequired( "-c", "The center." );
  parser->MarkArgumentAsRequired( "-r", "The radius." );
  
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> size;
  parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> center;
  parser->GetCommandLineArgument( "-c", center );

  std::vector<double> radius;
  parser->GetCommandLineArgument( "-r", radius );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  parser->GetCommandLineArgument( "-pt", PixelType );

  std::vector<double> spacing( Dimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> orientation( Dimension * Dimension, 0.0 );
  bool reto = parser->GetCommandLineArgument( "-o", orientation );

  if ( !reto )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      orientation[ i * ( Dimension + 1 ) ] = 1.0;
    }
  }

  /** Get rid of the possible "_" in PixelType. */
  itktools::ReplaceUnderscoreWithSpace( PixelType );

  
  /** Class that does the work */
  ITKToolsCreateEllipsoidBase * createEllipsoid = 0; 

  /** Short alias */
  unsigned int dim = Dimension;

  itktools::ComponentType componentType = itk::ImageIOBase::GetComponentTypeFromString( PixelType );

  try
  {    
    // now call all possible template combinations.
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< unsigned char, 2 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< char, 2 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< unsigned short, 2 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< short, 2 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< float, 2 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< double, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< unsigned char, 3 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< char, 3 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< unsigned short, 3 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< short, 3 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< float, 3 >::New( componentType, dim );
    if (!createEllipsoid) createEllipsoid = ITKToolsCreateEllipsoid< double, 3 >::New( componentType, dim );
#endif
    if (!createEllipsoid) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    createEllipsoid->m_OutputFileName = outputFileName;
    createEllipsoid->m_Size = size;
    createEllipsoid->m_Spacing = spacing;
    createEllipsoid->m_Center = center;
    createEllipsoid->m_Radius = radius;
    createEllipsoid->m_Orientation = orientation;

    createEllipsoid->Run();
    
    delete createEllipsoid;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createEllipsoid;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main
