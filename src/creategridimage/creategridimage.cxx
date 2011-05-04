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
 \brief Create a grid image.
 
 \verbinclude creategridimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, dim ) \
if ( imageDimension == dim ) \
{ \
  function< dim >( inputFileName, outputFileName, imageSize, imageSpacing, distance, is2DStack ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare functions. */
std::string GetHelpString( void );

template<unsigned int Dimension>
void CreateGridImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & imageSize,
  const std::vector<float> & imageSpacing,
  const std::vector<unsigned int> & distance,
  const bool & is2DStack );

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

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
  
  /** Get arguments. */
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> imageSize;
  bool retsz = parser->GetCommandLineArgument( "-sz", imageSize );

  const bool is2DStack = parser->ArgumentExists( "-stack" );

  /** Check if the required arguments are given. */
  if ( ( !retin && !retsz ) || ( retin && retsz ) )
  {
    std::cerr << "ERROR: You should specify \"-in\" or \"-sz\"." << std::endl;
    return 1;
  }

  /** Check arguments: size. */
  if ( retsz )
  {
    for ( unsigned int i = 0; i < imageSize.size(); ++i )
    {
      if ( imageSize[ i ] == 0 )
      {
        std::cerr << "ERROR: image size[" << i << "] = 0." << std::endl;
        return 1;
      }
    }
  }

  /** Get desired grid image dimension. */
  unsigned int imageDimension = 3;
  if ( retsz )
  {
    imageDimension = imageSize.size();
  }
  else
  {
    /** Determine image properties. */
    std::string ComponentTypeIn = "short";
    std::string PixelType; //we don't use this
    unsigned int NumberOfComponents = 1;
    std::vector<unsigned int> imagesize( imageDimension, 0 );
    int retgip = GetImageProperties(
      inputFileName,
      PixelType,
      ComponentTypeIn,
      imageDimension,
      NumberOfComponents,
      imagesize );
    if ( retgip != 0 ) return 1;
  }

  /** Check arguments: dimensionality. */
  if ( imageDimension < 2 || imageDimension > 3 )
  {
    std::cerr << "ERROR: Only image dimensions of 2 or 3 are supported." << std::endl;
    return 1;
  }

  /** Get more arguments. */
  std::vector<float> imageSpacing( imageDimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", imageSpacing );

  std::vector<unsigned int> distance( imageDimension, 1 );
  bool retd = parser->GetCommandLineArgument( "-d", distance );

  /** Check arguments: distance. */
  if ( !retd )
  {
    std::cerr << "ERROR: You should specify \"-d\"." << std::endl;
    return 1;
  }
  for ( unsigned int i = 0; i < distance.size(); ++i )
  {
    if ( distance[ i ] == 0 ) distance[ i ] = 1;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CreateGridImage, 2 );
    run( CreateGridImage, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this dimension is not supported!" << std::endl;
    std::cerr <<
      " ; dimension = " << imageDimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


/**
 * ******************* CreateGridImage *******************
 */

template< unsigned int Dimension >
void CreateGridImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & imageSize,
  const std::vector<float> & imageSpacing,
  const std::vector<unsigned int> & distance,
  const bool & is2DStack )
{
  /** Typedef's. */
  typedef unsigned char                                   PixelType;
  typedef itk::Image< PixelType, Dimension >              ImageType;
  typedef itk::ImageRegionIteratorWithIndex< ImageType >  IteratorType;
  typedef itk::ImageFileReader< ImageType >               ReaderType;
  typedef itk::ImageFileWriter< ImageType >               WriterType;

  typedef typename ImageType::SizeType    SizeType;
  typedef typename ImageType::IndexType   IndexType;
  typedef typename ImageType::SpacingType SpacingType;

  /* Create image and writer. */
  typename ImageType::Pointer  image  = ImageType::New();
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Get and set grid image information. */
  if ( inputFileName != "" )
  {
    reader->SetFileName( inputFileName.c_str() );
    reader->GenerateOutputInformation();

    SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    image->SetRegions( size );
    image->SetSpacing( reader->GetOutput()->GetSpacing() );
    image->SetOrigin( reader->GetOutput()->GetOrigin() );
    image->SetDirection( reader->GetOutput()->GetDirection() );
  }
  else
  {
    SizeType    size;
    SpacingType spacing;
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      size[ i ] = imageSize[ i ];
      spacing[ i ] = imageSpacing[ i ];
    }
    image->SetRegions( size );
    image->SetSpacing( spacing );
  }

  /** Allocate image. */
  image->Allocate();

  /* Fill the image. */
  IteratorType  it( image, image->GetLargestPossibleRegion() );
  it.GoToBegin();
  IndexType ind;
  while ( !it.IsAtEnd() )
  {
    /** Check if on grid. */
    ind = it.GetIndex();
    bool onGrid = false;
    onGrid |= ind[ 0 ] % distance[ 0 ] == 0;
    onGrid |= ind[ 1 ] % distance[ 1 ] == 0;
    if ( Dimension == 3 && !is2DStack )
    {
      if ( ind[ 2 ] % distance[ 2 ] != 0 )
      {
        onGrid = ind[ 0 ] % distance[ 0 ] == 0;
        onGrid &= ind[ 1 ] % distance[ 1 ] == 0;
      }
    }
    /** Set the value and continue. */
    if ( onGrid ) it.Set( 1 );
    else it.Set( 0 );
    ++it;
  } // end while

  /* Write result to file. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( image );
  writer->Update();

} // end CreateGridImage()


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxcreategridimage" << std::endl
    << "[-in]    inputFilename, information about size, etc, is taken from it" << std::endl
    << "-out     outputFilename" << std::endl
    << "-sz      image size for each dimension" << std::endl
    << "[-sp]    image spacing, default 1.0" << std::endl
    << "-d       distance in pixels between two gridlines" << std::endl
    << "[-stack] for 3D images, create a stack of 2D images, default false" << std::endl
  << "Supported: 2D, 3D, short.";
  return ss.str();

} // end GetHelpString()
