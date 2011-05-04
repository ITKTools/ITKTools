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
 \brief Create a blank image.
 
 \verbinclude createzeroimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > ImageType; \
  function< ImageType >( fileName, size, spacing, origin ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CreateZeroImage. */
template< class InputImageType >
void CreateZeroImage(
  const std::string & fileName,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin );

/** Declare PrintHelp. */
std::string PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  
  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-sz");
  exactlyOneArguments.push_back("-in");
  
  parser->MarkExactlyOneOfArgumentsAsRequired(exactlyOneArguments);

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
  std::string fileNameIn = "";
  bool retin = parser->GetCommandLineArgument( "-in", fileNameIn );

  std::string fileName = "";
  parser->GetCommandLineArgument( "-out", fileName );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  parser->GetCommandLineArgument( "-pt", PixelType );

  std::vector<unsigned int> size( Dimension, 0 );
  bool retsz = parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( Dimension, 1.0 );
  bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( Dimension, 0.0 );
  bool reto = parser->GetCommandLineArgument( "-o", origin );

  std::vector<double> direction( Dimension * Dimension, 0.0 );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    direction[ i * ( Dimension + 1 ) ] = 1.0;
  }

  parser->GetCommandLineArgument( "-d", direction );

  if ( retin )
  {
    /** Determine image properties. */
    std::string dummyPixelType; //we don't use this
    unsigned int NumberOfComponents = 1;
    int retgip = GetImageProperties(
      fileNameIn,
      dummyPixelType, PixelType, Dimension, NumberOfComponents,
      size, spacing, origin, direction );
    if ( retgip != 0 )
    {
      return 1;
    }
  }

  /** Get rid of the possible "_" in PixelType. */
  ReplaceUnderscoreWithSpace( PixelType );

  /** Check size, spacing and origin. */
  if ( retsz )
  {
    if( size.size() != Dimension && size.size() != 1 )
    {
      std::cout << "ERROR: The number of sizes should be 1 or Dimension." << std::endl;
      return 1;
    }
  }
  if ( retsp )
  {
    if( spacing.size() != Dimension && spacing.size() != 1 )
    {
      std::cout << "ERROR: The number of spacings should be 1 or Dimension." << std::endl;
      return 1;
    }
  }
  if ( reto )
  {
    if( origin.size() != Dimension && origin.size() != 1 )
    {
      std::cout << "ERROR: The number of origins should be 1 or Dimension." << std::endl;
      return 1;
    }
  }

  /** Check size and spacing for nonpositive numbers. */
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    if ( size[ i ] < 1 )
    {
      std::cerr << "ERROR: For each dimension the size should be at least 1." << std::endl;
      return 1;
    }
    if ( spacing[ i ] < 0.00001 )
    {
      std::cerr << "ERROR: No negative numbers are allowed in the spacing." << std::endl;
      return 1;
    }
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CreateZeroImage, unsigned char, 2 );
    run( CreateZeroImage, unsigned char, 3 );
    run( CreateZeroImage, char, 2 );
    run( CreateZeroImage, char, 3 );
    run( CreateZeroImage, unsigned short, 2 );
    run( CreateZeroImage, unsigned short, 3 );
    run( CreateZeroImage, short, 2 );
    run( CreateZeroImage, short, 3 );
    run( CreateZeroImage, float, 2 );
    run( CreateZeroImage, float, 3 );
    run( CreateZeroImage, double, 2 );
    run( CreateZeroImage, double, 3 );
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
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


  /**
   * ******************* CreateZeroImage *******************
   *
   * The CreateZeroImage function templated over the input pixel type.
   */

template< class ImageType >
void CreateZeroImage(
  const std::string & fileName,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin )
{
  /** Typedefs. */
  typedef itk::ImageFileWriter< ImageType >     WriterType;
  typedef typename ImageType::PixelType         PixelType;
  typedef typename ImageType::SizeType          SizeType;
  typedef typename ImageType::SpacingType       SpacingType;
  typedef typename ImageType::PointType         OriginType;
  const unsigned int Dimension = ImageType::ImageDimension;

  /** Prepare stuff. */
  SizeType    imSize;
  SpacingType imSpacing;
  OriginType  imOrigin;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    imSize[ i ] = size[ i ];
    imSpacing[ i ] = spacing[ i ];
    imOrigin[ i ] = origin[ i ];
  }

  /** Create image. */
  typename ImageType::Pointer image = ImageType::New();
  image->SetRegions( imSize );
  image->SetOrigin( imOrigin );
  image->SetSpacing( imSpacing );
  image->Allocate();
  image->FillBuffer( itk::NumericTraits<PixelType>::Zero );

  /** Write the image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( fileName.c_str() );
  writer->SetInput( image );
  writer->Update();

} // end CreateZeroImage


  /**
   * ******************* PrintHelp *******************
   */
std::string PrintHelp( void )
{
  std::string helpText = "Usage: \
  pxcreatezeroimage \
    [-in]    inputFilename \
    -out     outputFilename \
    -sz      size \
    [-sp]    spacing \
    [-o]     origin \
    [-dim]   dimension, default 3 \
    [-pt]    pixelType, default short \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";
  return helpText;
} // end PrintHelp

