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
 \brief Rescale the intensities of an image.
 
 \verbinclude rescaleintensityimagefilter.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "vnl/vnl_math.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxrescaleintensityimagefilter" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + INTENSITYRESCALED.mhd" << std::endl
  << "  [-mm]    minimum maximum, default: range of pixeltype" << std::endl
  << "  [-mv]    mean variance, default: 0.0 1.0" << std::endl
  << "  [-pt]    pixel type of input and output images;" << std::endl
  << "           default: automatically determined from the first input image." << std::endl
  << "Either \"-mm\" or \"-mv\" need to be specified." << std::endl
  << "Supported: 2D, 3D, (unsigned) short, (unsigned) char, float." << std::endl
  << "When applied to vector images, this program performs the operation on each channel separately.";

  return ss.str();

} // end GetHelpString()


/** RescaleIntensityImageFilter */

class ITKToolsRescaleIntensityImageFilterBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsRescaleIntensityImageFilterBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    //std::vector<double> this->m_Values;
    this->m_ValuesAreExtrema = false;
  };
  ~ITKToolsRescaleIntensityImageFilterBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<double> m_Values;
  bool m_ValuesAreExtrema;

    
}; // end RescaleIntensityImageFilterBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsRescaleIntensityImageFilter : public ITKToolsRescaleIntensityImageFilterBase
{
public:
  typedef ITKToolsRescaleIntensityImageFilter Self;

  ITKToolsRescaleIntensityImageFilter(){};
  ~ITKToolsRescaleIntensityImageFilter(){};

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
    /** TYPEDEF's. */
    typedef itk::Image<TComponentType, VDimension>        ScalarImageType;
    typedef itk::VectorImage<TComponentType, VDimension>  VectorImageType;

    typedef itk::ImageFileReader< VectorImageType >        ReaderType;
    typedef itk::RescaleIntensityImageFilter<
      ScalarImageType, ScalarImageType >                  RescalerType;
    typedef itk::StatisticsImageFilter< ScalarImageType > StatisticsType;
    typedef itk::ShiftScaleImageFilter<
      ScalarImageType, ScalarImageType >                  ShiftScalerType;
    typedef itk::ImageFileWriter< VectorImageType >        WriterType;
    typedef typename ScalarImageType::PixelType           PixelType;
    typedef typename StatisticsType::RealType             RealType;

    /** DECLARATION'S. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();
    typename RescalerType::Pointer    rescaler;
    typename StatisticsType::Pointer  statistics;
    typename ShiftScalerType::Pointer shiftscaler;

    /** Read in the inputImage. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    // Setup type to disassemble the components
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexSelectionType;
    
    // Setup the filter to reassemble the components
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
      
    for(unsigned int component = 0; component < reader->GetOutput()->GetNumberOfComponentsPerPixel(); ++component)
    {
      typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
      indexSelectionFilter->SetIndex(component);
      indexSelectionFilter->SetInput(reader->GetOutput());
      indexSelectionFilter->Update();
      
      /** If the input values are extrema (minimum and maximum),
      * then an IntensityRescaler is used. Otherwise, the values represent
      * the desired mean and variance and a ShiftScaler is used.
      */
      if ( this->m_ValuesAreExtrema )
      {
	/** Create instance. */
	rescaler = RescalerType::New();

	/** Define the extrema. */
	PixelType min, max;
	if ( this->m_Values[ 0 ] == 0.0 && this->m_Values[ 1 ] == 0.0 )
	{
	  min = itk::NumericTraits<PixelType>::NonpositiveMin();
	  max = itk::NumericTraits<PixelType>::max();
	}
	else
	{
	  min = static_cast<PixelType>( this->m_Values[ 0 ] );
	  max = static_cast<PixelType>( this->m_Values[ 1 ] );
	}

	/** Setup the rescaler. */
	rescaler->SetInput( indexSelectionFilter->GetOutput() );
	rescaler->SetOutputMinimum( min );
	rescaler->SetOutputMaximum( max );
	rescaler->Update();

	/** Setup the recombining. */
	imageToVectorImageFilter->SetNthInput(component, rescaler->GetOutput());

      } // end if values are extrema
      else
      {
	/** Create instances. */
	statistics = StatisticsType::New();
	shiftscaler = ShiftScalerType::New();

	/** Calculate image statistics. */
	statistics->SetInput( indexSelectionFilter->GetOutput() );
	statistics->Update();

	/** Get mean and variance of input image. */
	RealType mean = statistics->GetMean();
	RealType sigma = statistics->GetSigma();

	/** Setup the shiftscaler. */
	shiftscaler->SetInput( indexSelectionFilter->GetOutput() );
	shiftscaler->SetShift( this->m_Values[ 0 ] * sigma / vcl_sqrt( this->m_Values[ 1 ] ) - mean );
	shiftscaler->SetScale( vcl_sqrt( this->m_Values[ 1 ] ) / sigma );
	shiftscaler->Update();

	/** Setup the recombining. */
	imageToVectorImageFilter->SetNthInput(component, shiftscaler->GetOutput());

      } // end if values are mean and variance
    }// end component loop

    imageToVectorImageFilter->Update();
    writer->SetInput(imageToVectorImageFilter->GetOutput());
    
    /** Write the output image. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();
  }

}; // end RescaleIntensityImageFilter

//-------------------------------------------------------------------------------------

/* Declare RescaleIntensity. */
template< class InputImageType >
void RescaleIntensity(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<double> & values,
  const bool & valuesAreExtrema );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-mm");
  exactlyOneArguments.push_back("-mv");
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
  outputFileName += "INTENSITYRESCALED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<double> extrema( 2, 0.0 );
  bool retmm = parser->GetCommandLineArgument( "-mm", extrema );

  std::vector<double> meanvariance( 2, 0.0 );
  meanvariance[ 1 ] = 1.0;
  bool retmv = parser->GetCommandLineArgument( "-mv", meanvariance );

  /** Check if the extrema are given (correctly). */
  if ( retmm )
  {
    if ( extrema.size() != 2 )
    {
      std::cerr << "ERROR: You should specify \"-mm\" with two values." << std::endl;
      return 1;
    }
    if ( extrema[ 1 ] <= extrema[ 0 ] )
    {
      std::cerr << "ERROR: You should specify \"-mm\" with two values:" << std::endl;
      std::cerr << "minimum maximum, where it should hold that maximum > minimum." << std::endl;
      return 1;
    }
  }

  /** Check if the mean and variance are given correctly. */
  if ( retmv )
  {
    if ( meanvariance.size() != 2 )
    {
      std::cerr << "ERROR: You should specify \"-mv\" with two values." << std::endl;
      return 1;
    }
    if ( meanvariance[ 1 ] <= 1e-5 )
    {
      std::cerr << "ERROR: The variance should be strictly positive." << std::endl;
      return 1;
    }
  }

  /** Check which option is selected. */
  bool valuesAreExtrema = true;
  if ( retmv ) valuesAreExtrema = false;

  /** Determine input image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Let the user overrule this. */
  if ( retmv )
  {
    ComponentType = "float";
  }
  parser->GetCommandLineArgument( "-pt", ComponentType );

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentType );

  /** Get the values. */
  std::vector<double> values;
  if ( valuesAreExtrema )
  {
    values = extrema;
  }
  else
  {
    values = meanvariance;
  }


  /** Class that does the work */
  ITKToolsRescaleIntensityImageFilterBase * rescaleIntensityImageFilter = 0; 

  /** Short alias */
  unsigned int imageDimension = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< unsigned char, 2 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< char, 2 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< unsigned short, 2 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< short, 2 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< float, 2 >::New( componentType, imageDimension );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< unsigned char, 3 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< char, 3 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< unsigned short, 3 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< short, 3 >::New( componentType, imageDimension );
    if (!rescaleIntensityImageFilter) rescaleIntensityImageFilter = ITKToolsRescaleIntensityImageFilter< float, 3 >::New( componentType, imageDimension );
#endif
    if (!rescaleIntensityImageFilter) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    rescaleIntensityImageFilter->m_InputFileName = inputFileName;
    rescaleIntensityImageFilter->m_OutputFileName = outputFileName;
    rescaleIntensityImageFilter->m_Values = values;
    rescaleIntensityImageFilter->m_ValuesAreExtrema = valuesAreExtrema;

    rescaleIntensityImageFilter->Run();
    
    delete rescaleIntensityImageFilter;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete rescaleIntensityImageFilter;
    return 1;
  }
  

  /** End program. */
  return 0;

} // end main()

