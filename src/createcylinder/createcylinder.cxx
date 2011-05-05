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
 \brief Create a cylinder image.
 
 \verbinclude createcylinder.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkCylinderSpatialFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, dim ) \
if ( Dimension == dim ) \
{ \
  function< dim >( inputFileName, outputFileName, center, radius ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/* Declare CreateCylinder. */
template< unsigned int Dimension >
void CreateCylinder(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & center,
  const double & radius );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-c", "The center." );
  parser->MarkArgumentAsRequired( "-r", "The radius." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> center;
  parser->GetCommandLineArgument( "-c", center );

  double radius;
  parser->GetCommandLineArgument( "-r", radius );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 ) return 1;

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CreateCylinder, 2 );
    run( CreateCylinder, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. Return a value. */
  return 0;

} // end main

/*
 * ******************* CreateCylinder *******************
 */

template< unsigned int Dimension >
void CreateCylinder(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & center,
  const double & radius )
{
  /** Typedefs. */
  typedef float               InputPixelType;
  typedef unsigned char       OutputPixelType;
  typedef itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;
  typedef itk::ImageRegionIteratorWithIndex< OutputImageType >  IteratorType;
  typedef itk::CylinderSpatialFunction< Dimension > CylinderSpatialFunctionType;
  typedef typename CylinderSpatialFunctionType::InputType    InputType;
  typedef typename OutputImageType::IndexType   IndexType;
  typedef typename OutputImageType::PointType   PointType;

  /** Create a testReader. */
  typename ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( inputFileName.c_str() );
  testReader->Update();

  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->CopyInformation( testReader->GetOutput() );
  outputImage->SetRegions( outputImage->GetLargestPossibleRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer( 0 );

  /** Parse the arguments. */
  InputType   Center;
  PointType point;
  IndexType index;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    index[ i ] = center[ i ];
  }
  outputImage->TransformIndexToPhysicalPoint( index, point );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Center[ i ] = point[ i ];
  }

  /** Create and initialize ellipsoid. */
  typename CylinderSpatialFunctionType::Pointer cylinder
    = CylinderSpatialFunctionType::New();
  cylinder->SetCenter( Center );
  cylinder->SetRadius( radius );

  /** Create iterator, index and point. */
  IteratorType it( outputImage, outputImage->GetLargestPossibleRegion() );
  it.GoToBegin();

  /** Walk over the image. */
  while ( !it.IsAtEnd() )
  {
    index = it.GetIndex();
    outputImage->TransformIndexToPhysicalPoint( index, point );
    it.Set( cylinder->Evaluate( point ) );
    ++it;
  }

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( outputImage );
  writer->Update();

} // end CreateCylinder()


/**
 * ******************* PrintHelp *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxcreatecylinder" << std::endl
  << "-in      inputFilename" << std::endl
  << "-out     outputFilename" << std::endl
  << "-c       center (mm)" << std::endl
  << "-r       radii (mm)" << std::endl
  << "Supported: 2D, 3D.";
  return ss.str();
} // end GetHelpString()

