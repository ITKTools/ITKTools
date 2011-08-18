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
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


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


class ITKToolsResizeImageBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsResizeImageBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    //std::vector<double> this->m_FactorOrSpacing;
    this->m_IsFactor = false;
    this->m_InterpolationOrder = 0;
  };
  ~ITKToolsResizeImageBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_FactorOrSpacing;
  bool m_IsFactor;
  unsigned int m_InterpolationOrder;

    
}; // end ResizeImageBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsResizeImage : public ITKToolsResizeImageBase
{
public:
  typedef ITKToolsResizeImage Self;

  ITKToolsResizeImage(){};
  ~ITKToolsResizeImage(){};

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
    typedef itk::Image<TComponentType, VDimension>      InputImageType;
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
    reader->SetFileName( this->m_InputFileName.c_str() );
    inputImage = reader->GetOutput();
    inputImage->Update();

    /** Prepare stuff. */
    SpacingType inputSpacing  = inputImage->GetSpacing();
    SizeType    inputSize     = inputImage->GetLargestPossibleRegion().GetSize();
    SpacingType outputSpacing = inputSpacing;
    SizeType    outputSize    = inputSize;
    if ( this->m_IsFactor )
    {
      for ( unsigned int i = 0; i < Dimension; i++ )
      {
	outputSpacing[ i ] /= this->m_FactorOrSpacing[ i ];
	outputSize[ i ] = static_cast<unsigned int>( outputSize[ i ] * this->m_FactorOrSpacing[ i ] );
      }
    }
    else
    {
      for ( unsigned int i = 0; i < Dimension; i++ )
      {
	outputSpacing[ i ] = this->m_FactorOrSpacing[ i ];
	outputSize[ i ] = static_cast<unsigned int>( inputSpacing[ i ] * inputSize[ i ] / this->m_FactorOrSpacing[ i ] );
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
    if ( this->m_InterpolationOrder == 0 )
    {
      resampler->SetInterpolator( nnInterpolator );
    }
    else if ( this->m_InterpolationOrder > 1 )
    {
      bsInterpolator->SetSplineOrder( this->m_InterpolationOrder );
      resampler->SetInterpolator( bsInterpolator );
    }

    /** Write the output image. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( resampler->GetOutput() );
    writer->Update();
  }

}; // end ResizeImage

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-f");
  exactlyOneArguments.push_back("-sp");
  parser->MarkExactlyOneOfArgumentsAsRequired(exactlyOneArguments);

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
  itktools::ReplaceUnderscoreWithSpace( PixelType );

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


  /** Class that does the work */
  ITKToolsResizeImageBase * resizeImage = 0; 

  /** Short alias */
  unsigned int imageDimension = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;
  try
  {    
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned char, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< char, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned short, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< short, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned int, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< int, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned long, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< long, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< float, 2 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< double, 2 >::New( componentType, imageDimension );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned char, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< char, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned short, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< short, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned int, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< int, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< unsigned long, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< long, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< float, 3 >::New( componentType, imageDimension );
    if (!resizeImage) resizeImage = ITKToolsResizeImage< double, 3 >::New( componentType, imageDimension );
#endif
    if (!resizeImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    resizeImage->m_InputFileName = inputFileName;
    resizeImage->m_OutputFileName = outputFileName;
    resizeImage->m_FactorOrSpacing = factorOrSpacing;
    resizeImage->m_IsFactor = isFactor;
    resizeImage->m_InterpolationOrder = interpolationOrder;

    resizeImage->Run();
    
    delete resizeImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete resizeImage;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main

