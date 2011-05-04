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
 \brief This program replaces the value of a user specified voxel.
 
 \verbinclude replacevoxel.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageFileReader.h"
//#include "itkChangeLabelImageFilter.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  function< type, dim >( inputFileName, outputFileName, voxel, value ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare ReplacePixel. */
template< class TInputPixel, unsigned int NDimension >
void ReplaceVoxel(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & voxel,
  const double & value );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-vox", "Voxel." );
  parser->MarkArgumentAsRequired( "-val", "Value." );

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

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "VOXELREPLACED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector< unsigned int > voxel;
  parser->GetCommandLineArgument( "-vox", voxel );

  double value;
  parser->GetCommandLineArgument( "-val", value );

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
  if ( retgip != 0 )
  {
    std::cerr << "ERROR: error while getting image properties of the input image!" << std::endl;
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Cannot make vector of vector images." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check if the specified voxel-size has Dimension number of components. */
  if ( voxel.size() != Dimension )
  {
    std::cerr << "ERROR: You should specify "
      << Dimension
      << " numbers with \"-vox\"." << std::endl;
    return 1;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( ReplaceVoxel, char, 2 );
    run( ReplaceVoxel, unsigned char, 2 );
    run( ReplaceVoxel, short, 2 );
    run( ReplaceVoxel, unsigned short, 2 );
    run( ReplaceVoxel, int, 2 );
    run( ReplaceVoxel, unsigned int, 2 );
    run( ReplaceVoxel, long, 2 );
    run( ReplaceVoxel, unsigned long, 2 );
    run( ReplaceVoxel, float, 2 );
    run( ReplaceVoxel, double, 2 );

    run( ReplaceVoxel, char, 3 );
    run( ReplaceVoxel, unsigned char, 3 );
    run( ReplaceVoxel, short, 3 );
    run( ReplaceVoxel, unsigned short, 3 );
    run( ReplaceVoxel, int, 3 );
    run( ReplaceVoxel, unsigned int, 3 );
    run( ReplaceVoxel, long, 3 );
    run( ReplaceVoxel, unsigned long, 3 );
    run( ReplaceVoxel, float, 3 );
    run( ReplaceVoxel, double, 3 );

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
      << "pixel (component) type = " << ComponentTypeIn
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


/**
 * ******************* ReplaceVoxel *******************
 */

template< class TInputPixel, unsigned int NDimension >
void ReplaceVoxel( const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & voxel,
  const double & value )
{
  /** Typedefs. */
  typedef TInputPixel                           PixelType;
  const unsigned int Dimension = NDimension;

  typedef itk::Image< PixelType, Dimension >    ImageType;
  typedef typename ImageType::SizeType          SizeType;
  typedef typename ImageType::IndexType         IndexType;
  typedef itk::ImageFileReader< ImageType >     ReaderType;
  typedef itk::ImageFileWriter< ImageType >     WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Read input image. */
  reader->SetFileName( inputFileName );
  reader->Update();
  typename ImageType::Pointer image = reader->GetOutput();

  /** Check size. */
  SizeType size = image->GetLargestPossibleRegion().GetSize();
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    if ( voxel[ i ] < 0 || voxel[ i ] > size[ i ] - 1 )
    {
      itkGenericExceptionMacro( << "ERROR: invalid voxel index." );
    }
  }

  /** Set the value to the voxel. */
  IndexType index;
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    index[ i ] = voxel[ i ];
  }
  image->SetPixel( index, static_cast<PixelType>( value ) );

  /** Write output image. */
  writer->SetFileName( outputFileName );
  writer->SetInput( image );
  writer->Update();

} // end ReplaceVoxel()


/**
 * ******************* GetHelpString *******************
 */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program replaces the value of a user specified voxel." << std::endl
  << "Usage:" << std::endl
  << "pxreplacevoxel" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + VOXELREPLACED.mhd" << std::endl
  << "  -vox     input voxel index" << std::endl
  << "  -val     value that replaces the voxel" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int," << std::endl
  << "(unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()
