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
 \brief Either tiles a stack of 2D images into a 3D image, or tiles nD images to form another nD image.
 
 \verbinclude tileimages.help
 */

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkTileImageFilter.h"
#include "itkImageFileWriter.h"
#include <vector>
#include <string>

/** This program tiles a stacks of 2D images into a 3D image.
 * This is done by employing an itk::SeriesFileReader.
 *
 */

/** run: A macro to call a function. */
#define runA(function,type) \
if ( ComponentType == #type ) \
{ \
  function< type >( inputFileNames, outputFileName, zspacing ); \
  supported = true; \
}

#define runB(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileNames, outputFileName, layout, defaultvalue ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare TileImages2D3D. */
template< class PixelType >
void TileImages2D3D(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const double & zspacing );

/** Declare TileImages. */
template< class PixelType, unsigned int Dimension >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::vector<unsigned int> & layout,
  const double & defaultvalue );

/** Declare GetHelpString function. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  /** Get the input file names. */
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  if ( inputFileNames.size() < 2 )
  {
    std::cout << "ERROR: You should specify at least two input images." << std::endl;
    return 1;
  }

  /** Get the outputFileName. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Read the z-spacing. */
  double zspacing = -1.0;
  parser->GetCommandLineArgument( "-sp", zspacing );

  /** Get the layout. */
  std::vector< unsigned int > layout;
  bool retly = parser->GetCommandLineArgument( "-ly", layout );

  /** Get the layout. */
  double defaultvalue = 0.0;
  parser->GetCommandLineArgument( "-d", defaultvalue );

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileNames[ 0 ],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );

  if ( retgip != 0 )
  {
    return 1;
  }

  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** Let the user overrule this. */
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if ( retpt )
  {
    std::cout << "The user has overruled this by specifying -pt:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentType );

  /** Run the program. */
  bool supported = false;
  try
  {
    if ( !retly )
    {
      runA( TileImages2D3D, unsigned char );
      runA( TileImages2D3D, char );
      runA( TileImages2D3D, unsigned short );
      runA( TileImages2D3D, short );
      runA( TileImages2D3D, float );
    }
    else
    {
      runB( TileImages, unsigned char, 2 );
      runB( TileImages, char, 2 );
      runB( TileImages, unsigned short, 2 );
      runB( TileImages, short, 2 );
      runB( TileImages, float, 2 );

      runB( TileImages, unsigned char, 3 );
      runB( TileImages, char, 3 );
      runB( TileImages, unsigned short, 3 );
      runB( TileImages, short, 3 );
      runB( TileImages, short, 3 );
      runB( TileImages, float, 3 );
    }
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** Return a value. */
  return 0;

} // end main

//-------------------------------------------------------------------------------------

/** Define TileImages2D3D. */
template< class PixelType >
void TileImages2D3D(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const double & zspacing )
{
  /** Define image type. */
  const unsigned int Dimension = 3;

  /** Some typedef's. */
  typedef itk::Image<PixelType, Dimension>            ImageType;
  typedef typename ImageType::SpacingType             SpacingType;
  typedef itk::ImageSeriesReader<ImageType>           ImageSeriesReaderType;
  typedef itk::ImageFileWriter<ImageType>             ImageWriterType;

  /** Create reader. */
  typename ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
  reader->SetFileNames( inputFileNames );

  /** Update the reader. */
  std::cout << "Input images are read..." << std::endl;
  reader->Update();
  std::cout << "Reading done." << std::endl;
  typename ImageType::Pointer tiledImage = reader->GetOutput();

  /** Get and set the spacing, if it was set by the user. */
  if ( zspacing > 0.0 )
  {
    /** Make sure that changes are not undone */
    tiledImage->DisconnectPipeline();
    /** Set the zspacing */
    SpacingType spacing = tiledImage->GetSpacing();
    spacing[ 2 ] = zspacing;
    tiledImage->SetSpacing( spacing );
  }

  /** Write to disk. */
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( tiledImage );
  std::cout << "Writing tiled image..." << std::endl;
  writer->Update();
  std::cout << "Ready." << std::endl;

} // end TileImages2D3D

//-------------------------------------------------------------------------------------

/** Define TileImages. */
template< class PixelType, unsigned int Dimension >
void TileImages(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::vector<unsigned int> & layout,
  const double & defaultvalue )
{
  /** Some typedef's. */
  typedef itk::Image<PixelType, Dimension>            ImageType;
  typedef itk::ImageFileReader<ImageType>             ImageReaderType;
  typedef itk::TileImageFilter<ImageType, ImageType>  TilerType;
  typedef itk::ImageFileWriter<ImageType>             ImageWriterType;
  //typedef typename ImageType::SpacingType             SpacingType;

  /** Copy layout into a fixed array. */
  itk::FixedArray< unsigned int, Dimension > Layout;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    Layout[ i ] = layout[ i ];
  }

  /** Cast the defaultvalue. */
  PixelType defaultValue = static_cast<PixelType>( defaultvalue );

  /** Create tiler. */
  typename TilerType::Pointer tiler = TilerType::New();
  tiler->SetLayout( Layout );
  tiler->SetDefaultPixelValue( defaultValue );

  /** Read input images and set it into the tiler. */
  for ( unsigned int i = 0; i < inputFileNames.size(); i++ )
  {
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( inputFileNames[ i ].c_str() );
    reader->Update();
    tiler->SetInput( i, reader->GetOutput() );
  }

  /** Do the tiling. */
  tiler->Update();

  /** Write to disk. */
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( tiler->GetOutput() );
  writer->Update();

} // end TileImages

//-------------------------------------------------------------------------------------

/** Define GetHelpString. */
std::string GetHelpString( void )
{
  std::string helpText = "pxtileimages EITHER tiles a stack of 2D images into a 3D image, \
  OR tiles nD images to form another nD image. \
  In the last case the way to tile is specified by a layout. \
  To stack a pile of 2D images an itk::SeriesFileReader is employed. \
  If no layout is specified with \"-ly\" 2D-3D tiling is done, \
  otherwise 2D-2D or 3D-3D tiling is performed.\n\n \
  Usage:  \npxtileimages \
    -in      input image filenames, at least 2 \
    -out     output image filename \
    [-pt]    pixel type of input and output images \
             default: automatically determined from the first input image \
    [-sp]    spacing in z-direction for 2D-3D tiling [double]; \
             if omitted, the origins of the 2d images are used to find the spacing; \
             if these are identical, a spacing of 1.0 is assumed \
    [-ly]    layout of the nD-nD tiling \
             example: in 2D for 4 images \"-ly 2 2\" results in \
               im1 im2 \
               im3 im4 \
             example: in 2D for 4 images \"-ly 4 1\" (or \"-ly 0 1\") results in \
               im1 im2 im3 im4 \
    [-d]     default value, by default 0. \
  Supported pixel types: (unsigned) char, (unsigned) short, float.\n";

  return helpText;

} // end GetHelpString
