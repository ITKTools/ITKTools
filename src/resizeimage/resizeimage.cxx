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
 \brief Resize an image.
 
 \verbinclude resizeimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( PixelType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, factorOrSpacing, isFactor, interpolationOrder ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/* Declare ResizeImage. */
template< class InputImageType >
void ResizeImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<double> & factorOrSpacing,
  const bool & isFactor,
  const unsigned int & interpolationOrder );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-f");
  exactlyOneArguments.push_back("-sp");
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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "RESIZED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<double> factor;
  bool retf = parser->GetCommandLineArgument( "-f", factor );
  bool isFactor = retf;

  std::vector<double> spacing;
  bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  parser->GetCommandLineArgument( "-pt", PixelType );

  unsigned int interpolationOrder = 1;
  parser->GetCommandLineArgument( "-io", interpolationOrder );

  /** Get rid of the possible "_" in PixelType. */
  ReplaceUnderscoreWithSpace( PixelType );

  /** Check factor and spacing. */
  if ( retf )
  {
    if( factor.size() != Dimension && factor.size() != 1 )
    {
      std::cout << "ERROR: The number of factors should be 1 or Dimension." << std::endl;
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

  /** Get the factor or spacing. */
  double vector0 = ( retf ? factor[ 0 ] : spacing[ 0 ] );
  std::vector<double> factorOrSpacing( Dimension, vector0 );
  if ( retf && factor.size() == Dimension )
  {
    for ( unsigned int i = 1; i < Dimension; i++ )
    {
      factorOrSpacing[ i ] = factor[ i ];
    }
  }
  if ( retsp && spacing.size() == Dimension )
  {
    for ( unsigned int i = 1; i < Dimension; i++ )
    {
      factorOrSpacing[ i ] = spacing[ i ];
    }
  }

  /** Check factorOrSpacing for nonpositive numbers. */
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    if ( factorOrSpacing[ i ] < 0.00001 )
    {
      std::cout << "ERROR: No negative numbers are allowed in factor or spacing." << std::endl;
      return 1;
    }
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( ResizeImage, unsigned char, 2 );
    run( ResizeImage, char, 2 );
    run( ResizeImage, unsigned short, 2 );
    run( ResizeImage, short, 2 );
    run( ResizeImage, unsigned int, 2 );
    run( ResizeImage, int, 2 );
    run( ResizeImage, unsigned long, 2 );
    run( ResizeImage, long, 2 );
    run( ResizeImage, float, 2 );
    run( ResizeImage, double, 2 );

    run( ResizeImage, unsigned char, 3 );
    run( ResizeImage, char, 3 );
    run( ResizeImage, unsigned short, 3 );
    run( ResizeImage, short, 3 );
    run( ResizeImage, unsigned int, 3 );
    run( ResizeImage, int, 3 );
    run( ResizeImage, unsigned long, 3 );
    run( ResizeImage, long, 3 );
    run( ResizeImage, float, 3 );
    run( ResizeImage, double, 3 );

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


  /*
   * ******************* ResizeImage *******************
   *
   * The resize function templated over the input pixel type.
   */

template< class InputImageType >
void ResizeImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<double> & factorOrSpacing,
  const bool & isFactor,
  const unsigned int & interpolationOrder )
{
  /** Typedefs. */
  typedef itk::ResampleImageFilter< InputImageType, InputImageType >  ResamplerType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< InputImageType >      WriterType;
  typedef itk::NearestNeighborInterpolateImageFunction<
    InputImageType, double >                          NNInterpolatorType;
  typedef itk::BSplineInterpolateImageFunction<
    InputImageType >                                  BSplineInterpolatorType;

  typedef typename InputImageType::SizeType         SizeType;
  typedef typename InputImageType::SpacingType      SpacingType;

  const unsigned int Dimension = InputImageType::ImageDimension;

  /** Declarations. */
  typename InputImageType::Pointer inputImage;
  typename ResamplerType::Pointer resampler = ResamplerType::New();
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename NNInterpolatorType::Pointer nnInterpolator
    = NNInterpolatorType::New();
  typename BSplineInterpolatorType::Pointer bsInterpolator
    = BSplineInterpolatorType::New();

  /** Read in the inputImage. */
  reader->SetFileName( inputFileName.c_str() );
  inputImage = reader->GetOutput();
  inputImage->Update();

  /** Prepare stuff. */
  SpacingType inputSpacing  = inputImage->GetSpacing();
  SizeType    inputSize     = inputImage->GetLargestPossibleRegion().GetSize();
  SpacingType outputSpacing = inputSpacing;
  SizeType    outputSize    = inputSize;
  if ( isFactor )
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      outputSpacing[ i ] /= factorOrSpacing[ i ];
      outputSize[ i ] = static_cast<unsigned int>( outputSize[ i ] * factorOrSpacing[ i ] );
    }
  }
  else
  {
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      outputSpacing[ i ] = factorOrSpacing[ i ];
      outputSize[ i ] = static_cast<unsigned int>( inputSpacing[ i ] * inputSize[ i ] / factorOrSpacing[ i ] );
    }
  }

  /** Setup the pipeline. */
  resampler->SetInput( inputImage );
  resampler->SetSize( outputSize );
  resampler->SetDefaultPixelValue( 0 );
  resampler->SetOutputStartIndex( inputImage->GetLargestPossibleRegion().GetIndex() );
  resampler->SetOutputSpacing( outputSpacing );
  resampler->SetOutputOrigin( inputImage->GetOrigin() );
  /* The interpolator: the resampler has by default a LinearInterpolateImageFunction
   * as interpolator. */
  if ( interpolationOrder == 0 )
  {
    resampler->SetInterpolator( nnInterpolator );
  }
  else if ( interpolationOrder > 1 )
  {
    bsInterpolator->SetSplineOrder( interpolationOrder );
    resampler->SetInterpolator( bsInterpolator );
  }

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( resampler->GetOutput() );
  writer->Update();

} // end ResizeImage()


  /**
   * ******************* GetHelpString *******************
   */
std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxresizeimage" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + RESIZED.mhd" << std::endl
  << "  [-f]     factor" << std::endl
  << "  [-sp]    spacing" << std::endl
  << "  [-io]    interpolation order, default 1" << std::endl
  << "  [-dim]   dimension, default 3" << std::endl
  << "  [-pt]    pixelType, default short" << std::endl
  << "One of -f and -sp should be given." << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return ss.str();
} // end GetHelpString()

