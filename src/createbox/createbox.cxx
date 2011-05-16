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
 \brief Create a box image.
 
 \verbinclude createbox.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "CreateBoxHelper.h"

//-------------------------------------------------------------------------------------
/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
    << "pxcreatebox" << std::endl
    << "  -out     outputFilename" << std::endl
    << "  Arguments to specify the output image:" << std::endl
    << "  [-in]    inputFilename, to copy image information from" << std::endl
    << "  [-sz]    image size (voxels)" << std::endl
    << "  [-sp]    image spacing (mm), default 1.0" << std::endl
    << "  [-io]    image origin, default 0.0" << std::endl
    << "  [-d]     image direction, default identity" << std::endl
    << "  [-dim]   dimension, default 3" << std::endl
    << "  [-pt]    pixelType, default short" << std::endl
    << "  Arguments to specify the box:" << std::endl
    << "  [-c]     center (mm)" << std::endl
    << "  [-r]     radii (mm)" << std::endl
    << "  [-cp1]   cornerpoint 1 (mm)" << std::endl
    << "  [-cp2]   cornerpoint 2 (mm)" << std::endl
    << "  [-ci1]   cornerindex 1" << std::endl
    << "  [-ci2]   cornerindex 2" << std::endl
    << "  [-o]     orientation of the box, default xyz" << std::endl
    << "- The user should EITHER specify the input filename OR the output image size." << std::endl
    << "- The user should EITHER specify the center and the radius," << std::endl
    << "    OR the positions of two opposite corner points." << std::endl
    << "    OR the positions of two opposite corner indices." << std::endl
    << "- The orientation is a vector with Euler angles (rad)." << std::endl
    << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

  return ss.str();

} // end GetHelpString()

/** CreateBox */

class CreateBoxBase : public itktools::ITKToolsBase
{ 
public:
  CreateBoxBase(){};
  ~CreateBoxBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_Input1;
  std::vector<double> m_Input2;
  std::vector<double> m_OrientationOfBox;
  std::string m_BoxDefinition;

    
}; // end CreateBoxBase


template< class TComponentType, unsigned int VDimension >
class CreateBox : public CreateBoxBase
{
public:
  typedef CreateBox Self;

  CreateBox(){};
  ~CreateBox(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
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
    std::vector<unsigned int> size;
    std::vector<double> spacing, origin, direction;

    /** Convert image information to ITK type. */
    SizeType    sizeITK;
    ConvertVectorToSize<VDimension>(size, sizeITK);
    
    SpacingType spacingITK;
    ConvertVectorToSpacing<VDimension>(spacing, spacingITK);
    
    OriginType  originITK;
    ConvertVectorToOrigin<VDimension>(origin, originITK);
    
    DirectionType directionITK;
    ConvertVectorToDirection<VDimension>(direction, directionITK);

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
    if ( m_BoxDefinition == "CornersAsPoints" )
    {
      /** The input is points, copy it. */
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
	point1[ i ] = m_Input1[ i ];
	point2[ i ] = m_Input2[ i ];
      }
    }
    else if ( m_BoxDefinition == "CornersAsIndices" )
    {
      /** The input is indices, copy and transform to the point. */
      for ( unsigned int i = 0; i < VDimension; i++ )
      {
	index1[ i ] = static_cast<unsigned int>( m_Input1[ i ] );
	index2[ i ] = static_cast<unsigned int>( m_Input2[ i ] );
      }
      image->TransformIndexToPhysicalPoint( index1, point1 );
      image->TransformIndexToPhysicalPoint( index2, point2 );
    }

    /** Compute the center and radius. */
    if ( m_BoxDefinition != "CenterRadius" )
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
      orientationOfBoxITK[ i ] = m_OrientationOfBox[ i ];
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
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();
  }

}; // end CreateBox

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  /** Required argument checking. */
  parser->MarkArgumentAsRequired( "-out", "" );
  parser->MarkArgumentAsRequired( "-in", "" );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Output image information is specified using either a reference
   * input image, or by -sz.
   */
  std::vector<std::string> exactlyOneArguments1;
  exactlyOneArguments1.push_back( "-in" );
  exactlyOneArguments1.push_back( "-sz" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments1,
    "ERROR: You should specify either \"-in\" or \"-sz\"." );

  /** Three ways to specify a box, see --help. */
  std::vector<std::string> exactlyOneArguments2;
  exactlyOneArguments2.push_back( "-c" );
  exactlyOneArguments2.push_back( "-cp1" );
  exactlyOneArguments2.push_back( "-ci1" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments2,
    "ERROR: You should specify either \"-c\" or \"-cp1\" or \"-ci1\"." );

  std::vector<std::string> exactlyOneArguments3;
  exactlyOneArguments3.push_back( "-r" );
  exactlyOneArguments3.push_back( "-cp2" );
  exactlyOneArguments3.push_back( "-ci2" );
  parser->MarkExactlyOneOfArgumentsAsRequired( exactlyOneArguments3,
    "ERROR: You should specify either \"-r\" or \"-cp2\" or \"-ci2\"." );

  /** Get arguments: output image file name. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Get arguments: output image information. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::vector<unsigned int> size( Dimension );

  parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( Dimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( Dimension, 0.0 );
  parser->GetCommandLineArgument( "-io", origin );

  std::vector<double> direction( Dimension * Dimension, 0.0 );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    direction[ i * ( Dimension + 1 ) ] = 1.0;
  }
  parser->GetCommandLineArgument( "-d", direction );

  /** Get arguments: box position, size and orientation. */
  std::vector<double> center( Dimension );
  bool retc = parser->GetCommandLineArgument( "-c", center );

  std::vector<double> radius( Dimension );
  bool retr = parser->GetCommandLineArgument( "-r", radius );

  std::vector<double> corner1( Dimension );
  bool retcp1 = parser->GetCommandLineArgument( "-cp1", corner1 );

  std::vector<double> corner2( Dimension );
  bool retcp2 = parser->GetCommandLineArgument( "-cp2", corner2 );

  std::vector<double> cornerindex1( Dimension );
  bool retci1 = parser->GetCommandLineArgument( "-ci1", cornerindex1 );

  std::vector<double> cornerindex2( Dimension );
  bool retci2 = parser->GetCommandLineArgument( "-ci2", cornerindex2 );

  std::vector<double> orientation( Dimension, 0.0 );
  parser->GetCommandLineArgument( "-o", orientation );

  /** Additional check. */
  if ( ( !retc | !retr | retcp1 | retcp2 | retci1 | retci2 )
    && ( retc | retr | !retcp1 | !retcp2 | retci1 | retci2 )
    && ( retc | retr | retcp1 | retcp2 | !retci1 | !retci2 ) )
  {
    std::cerr << "ERROR: Either you should specify \"-c\" and \"-r\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-cp1\" and \"-cp2\"." << std::endl;
    std::cerr << "ERROR: Or you should specify \"-ci1\" and \"-ci2\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine output image properties. */
  itktools::ComponentType componentType;
  itktools::GetImageComponentType( inputFileName, componentType);
  /** Let the user overrule this. */
  //parser->GetCommandLineArgument( "-pt", componentType );
  
  itktools::GetImageDimension( inputFileName, Dimension);
  
  /** Fix the output to be scalar. */
  //referenceIOBase->SetNumberOfComponents( 1 );
    
  /** How was the input supplied by the user? */
  std::vector<double> input1, input2;
  std::string boxDefinition = "";
  if ( retc )
  {
    boxDefinition = "CenterRadius";
    input1 = center;
    input2 = radius;
  }
  else if ( retcp1 )
  {
    boxDefinition = "CornersAsPoints";
    input1 = corner1;
    input2 = corner2;
  }
  else if ( retci1 )
  {
    boxDefinition = "CornersAsIndices";
    input1 = cornerindex1;
    input2 = cornerindex2;
  }


  /** Class that does the work */
  CreateBoxBase * createBox = 0; 

  try
  {        
    if (!createBox) createBox = CreateBox< unsigned char, 2 >::New( componentType, Dimension );
    if (!createBox) createBox = CreateBox< char, 2 >::New( componentType, Dimension );
    if (!createBox) createBox = CreateBox< unsigned short, 2 >::New( componentType, Dimension );
    if (!createBox) createBox = CreateBox< short, 2 >::New( componentType, Dimension );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createBox) createBox = CreateBox< unsigned char, 3 >::New( componentType, Dimension );    
    if (!createBox) createBox = CreateBox< char, 3 >::New( componentType, Dimension );
    if (!createBox) createBox = CreateBox< unsigned short, 3 >::New( componentType, Dimension );
    if (!createBox) createBox = CreateBox< short, 3 >::New( componentType, Dimension );
#endif
    if (!createBox) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    createBox->m_InputFileName = inputFileName;
    createBox->m_OutputFileName = outputFileName;
    createBox->m_Input1 = input1;
    createBox->m_Input2 = input2;
    createBox->m_OrientationOfBox = orientation;
    createBox->m_BoxDefinition = boxDefinition;
    
    createBox->Run();
    
    delete createBox;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createBox;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main
