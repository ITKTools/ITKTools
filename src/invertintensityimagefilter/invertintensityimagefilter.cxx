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
 \brief This program inverts the intensities of an image.  new = max - old (where max is the maximum of the image).
 
 \verbinclude invertintensityimagefilter.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkStatisticsImageFilter.h"
#include "itkInvertIntensityImageFilter.h"

// Vector image support
#include "itkVectorIndexSelectionCastImageFilter.h" // decompose
#include "itkImageToVectorImageFilter.h" // reassemble
#include "itkChannelByChannelVectorImageFilter2.h"
//-------------------------------------------------------------------------------------


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "This program inverts the intensities of an image." << std::endl
  << "Usage:" << std::endl
  << "pxinvertintensityimagefilter" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename; default: in + INVERTED.mhd" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

  return ss.str();

} // end GetHelpString()

/** InvertIntensity */

class InvertIntensityBase : public itktools::ITKToolsBase
{ 
public:
  InvertIntensityBase(){};
  ~InvertIntensityBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
    
}; // end InvertIntensityBase


template< unsigned int VImageDimension, class TComponentType >
class InvertIntensity : public InvertIntensityBase
{
public:
  typedef InvertIntensity Self;

  InvertIntensity(){};
  ~InvertIntensity(){};

  static Self * New( unsigned int imageDimension, itktools::ComponentType componentType )
  {
    if ( VImageDimension == imageDimension && itktools::IsType<TComponentType>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Some typedef's. */
    typedef itk::Image<TComponentType, VImageDimension>             ScalarImageType;
    typedef typename ScalarImageType::PixelType                InputPixelType;
    typedef itk::VectorImage<InputPixelType, ScalarImageType::ImageDimension> VectorImageType;
    typedef itk::ImageFileReader< VectorImageType >            ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >            WriterType;
    typedef itk::StatisticsImageFilter< ScalarImageType >      StatisticsFilterType;
    typedef typename StatisticsFilterType::RealType           RealType;
    typedef itk::InvertIntensityImageFilter< ScalarImageType > InvertIntensityFilterType;

    /** Create reader. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( m_InputFileName.c_str() );

    // In this case, we must manually disassemble the image rather than use a ChannelByChannel filter because the image is not the output,
    // but rather the GetMaximum() function is what we want.
    
    // Create the disassembler
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexSelectionType;
    typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
    indexSelectionFilter->SetInput(reader->GetOutput());

    double max = std::numeric_limits<double>::min(); // Initialize so that any number will be bigger than this one

    // Get the max of each channel, keeping the largest
    for(unsigned int channel = 0; channel < reader->GetOutput()->GetNumberOfComponentsPerPixel(); channel++)
      {
      // Extract the current channel
      indexSelectionFilter->SetIndex(channel);
      indexSelectionFilter->Update();

      /** Create statistics filter. */
      typename StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
      statistics->SetInput( indexSelectionFilter->GetOutput() );
      statistics->Update();
      if(statistics->GetMaximum() > max)
	{
	max = statistics->GetMaximum();
	}
      }

    /** Create invert filter. */
    typename InvertIntensityFilterType::Pointer invertFilter = InvertIntensityFilterType::New();
    invertFilter->SetMaximum( max );

    // Setup the filter to apply the invert filter to every channel
    typedef itk::ChannelByChannelVectorImageFilter2<VectorImageType, InvertIntensityFilterType> ChannelByChannelInvertType;
    typename ChannelByChannelInvertType::Pointer channelByChannelInvertFilter = ChannelByChannelInvertType::New();
    channelByChannelInvertFilter->SetInput(reader->GetOutput());
    channelByChannelInvertFilter->SetFilter(invertFilter);
    channelByChannelInvertFilter->Update();

    /** Create writer. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( channelByChannelInvertFilter->GetOutput() );
    writer->Update();
  }

}; // end InvertIntensity

//-------------------------------------------------------------------------------------

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  parser->MarkArgumentAsRequired( "-in", "The input filename." );

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
  outputFileName += "INVERTED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }


  /** Class that does the work */
  InvertIntensityBase * invertIntensity = NULL; 

  unsigned int imageDimension = 0;
  itktools::GetImageDimension(inputFileName, imageDimension);

  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName);
    
  try
  {    
    // now call all possible template combinations.
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, char >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, unsigned char >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, short >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, unsigned short >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, float >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 2, double >::New( imageDimension, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, char >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, unsigned char >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, short >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, unsigned short >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, float >::New( imageDimension, componentType );
    if (!invertIntensity) invertIntensity = InvertIntensity< 3, double >::New( imageDimension, componentType );
#endif
    if (!invertIntensity) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << imageDimension << std::endl
        << " pixel type = " << componentType << std::endl
        << std::endl;
      return 1;
    }

    invertIntensity->m_OutputFileName = outputFileName;
    invertIntensity->m_InputFileName = inputFileName;
    
    invertIntensity->Run();
    
    delete invertIntensity;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete invertIntensity;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main

