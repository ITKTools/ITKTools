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
 \brief Threshold an image.
 
 \verbinclude thresholdimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "ThresholdWrappers.h"

//-------------------------------------------------------------------------------------


/*
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "This program thresholds an image." << std::endl
  << "Usage:" << std::endl
  << "pxthresholdimage" << std::endl
  << "  -in        inputFilename" << std::endl
  << "  [-out]     outputFilename; default in + THRESHOLDED.mhd" << std::endl
  << "  [-mask]    maskFilename, optional for \"OtsuThreshold\"," << std::endl
  << "required for \"KappaSigmaThreshold\"" << std::endl
  << "  [-m]       method, choose one of { Threshold, OtsuThreshold," << std::endl
  << "OtsuMultipleThreshold, AdaptiveOtsuThreshold, RobustAutomaticThreshold," << std::endl
  << "KappaSigmaThreshold, MinErrorThreshold }" << std::endl
  << "             default \"Threshold\"" << std::endl
  << "  [-t1]      lower threshold, for \"Threshold\", default -infinity" << std::endl
  << "  [-t2]      upper threshold, for \"Threshold\", default 1.0" << std::endl
  << "  [-inside]  inside value, default 1" << std::endl
  << "  [-outside] outside value, default 0" << std::endl
  << "  [-t]       number of thresholds, for \"OtsuMultipleThreshold\", default 1" << std::endl
  << "  [-b]       number of histogram bins, for \"OtsuThreshold\", \"MinErrorThreshold\"" << std::endl
  << "and \"AdaptiveOtsuThreshold\", default 128" << std::endl
  << "  [-r]       radius, for \"AdaptiveOtsuThreshold\", default 8" << std::endl
  << "  [-cp]      number of control points, for \"AdaptiveOtsuThreshold\", default 50" << std::endl
  << "  [-l]       number of levels, for \"AdaptiveOtsuThreshold\", default 3" << std::endl
  << "  [-s]       number of samples, for \"AdaptiveOtsuThreshold\", default 5000" << std::endl
  << "  [-o]       spline order, for \"AdaptiveOtsuThreshold\", default 3" << std::endl
  << "  [-p]       power, for \"RobustAutomaticThreshold\", default 1" << std::endl
  << "  [-sigma]   sigma factor, for \"KappaSigmaThreshold\", default 2" << std::endl
  << "  [-iter]    number of iterations, for \"KappaSigmaThreshold\", default 2" << std::endl
  << "  [-mv]      mask value, for \"KappaSigmaThreshold\", default 1" << std::endl
  << "  [-mt]      mixture type (1 - Gaussians, 2 - Poissons), for \"MinErrorThreshold\", default 1" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

  return ss.str();
} // end GetHelpString()


/** ThresholdImage */

class ITKToolsThresholdImageBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsThresholdImageBase()
  {
    m_Bins = 0;
    m_InputFileName = "";
    m_Inside = 0.0f;
    m_Iterations = 0;
    m_MaskFileName = "";
    m_MaskValue = 0;
    m_Method = "";
    m_MixtureType = 0;
    m_NumThresholds = 0;
    m_OutputFileName = "";
    m_Outside = 0.0f;
    m_Pow = 0.0f;
    m_Sigma = 0.0f;
    m_Supported = false;
    m_Threshold1 = 0.0f;
    m_Threshold2 = 0.0f;
  };
  ~ITKToolsThresholdImageBase(){};

  /** Input parameters */
  unsigned int m_Bins;
  std::string m_InputFileName;
  double m_Inside;
  unsigned int m_Iterations;
  std::string m_MaskFileName;
  unsigned int m_MaskValue;
  std::string m_Method;
  unsigned int m_MixtureType;
  unsigned int m_NumThresholds;
  std::string m_OutputFileName;
  double m_Outside;
  double m_Pow;
  double m_Sigma;
  bool m_Supported;
  double m_Threshold1;
  double m_Threshold2;
    
}; // end ThresholdImageBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsThresholdImageSelector : public ITKToolsThresholdImageBase
{
public:
  typedef ITKToolsThresholdImageSelector Self;

  ITKToolsThresholdImageSelector(){};
  ~ITKToolsThresholdImageSelector(){};

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
    typedef itk::Image< TComponentType, VDimension > InputImageType;
    if ( m_Method == "Threshold" )
    {
      ThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName,
      m_Inside, m_Outside,
      m_Threshold1, m_Threshold2 );
    }
    else if ( m_Method == "OtsuThreshold" )
    {
      OtsuThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName, m_MaskFileName,
      m_Inside, m_Outside,
      m_Bins );
    }
    else if ( m_Method == "OtsuMultipleThreshold" )
    {
      OtsuMultipleThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName, m_MaskFileName,
      m_Inside, m_Outside,
      m_Bins, m_NumThresholds );
    }
    else if ( m_Method == "RobustAutomaticThreshold" )
    {
      RobustAutomaticThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName,
      m_Inside, m_Outside,
      m_Pow );
    }
    else if ( m_Method == "KappaSigmaThreshold" )
    {
      KappaSigmaThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName, m_MaskFileName,
      m_Inside, m_Outside,
      m_MaskValue, m_Sigma, m_Iterations );
    }
    else if ( m_Method == "MinErrorThreshold" )
    {
      MinErrorThresholdImage< InputImageType >( m_InputFileName, m_OutputFileName,
      m_Inside, m_Outside,
      m_Bins, m_MixtureType );
    }
    else
    {
      std::cerr << "Not supported!" << std::endl;
      return;
    }
  }

}; // end ThresholdImage

//-------------------------------------------------------------------------------------


/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
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
  outputFileName += "THRESHOLDED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string method = "Threshold";
  parser->GetCommandLineArgument( "-m", method );

  double threshold1 = itk::NumericTraits<double>::NonpositiveMin();
  parser->GetCommandLineArgument( "-t1", threshold1 );

  double threshold2 = itk::NumericTraits<double>::One;
  parser->GetCommandLineArgument( "-t2", threshold2 );

  double inside = itk::NumericTraits<double>::One;
  parser->GetCommandLineArgument( "-inside", inside );

  double outside = itk::NumericTraits<double>::Zero;
  parser->GetCommandLineArgument( "-outside", outside );

  unsigned int radius = 8;
  parser->GetCommandLineArgument( "-r", radius );

  unsigned int bins = 128;
  parser->GetCommandLineArgument( "-b", bins );

  unsigned int numThresholds = 1;
  parser->GetCommandLineArgument( "-t", numThresholds );

  unsigned int controlPoints = 50;
  parser->GetCommandLineArgument( "-cp", controlPoints );

  unsigned int levels = 3;
  parser->GetCommandLineArgument( "-l", levels );

  unsigned int samples = 5000;
  parser->GetCommandLineArgument( "-s", samples );

  unsigned int splineOrder = 3;
  parser->GetCommandLineArgument( "-o", splineOrder );

  double pow = 1.0;
  parser->GetCommandLineArgument( "-p", pow );

  double sigma = 2.0;
  parser->GetCommandLineArgument( "-sigma", sigma );

  unsigned int iterations = 2;
  parser->GetCommandLineArgument( "-iter", iterations );

  unsigned int maskValue = 1;
  parser->GetCommandLineArgument( "-mv", maskValue );

  unsigned int mixtureType = 1;
  parser->GetCommandLineArgument( "-mt", mixtureType );

  /** Checks. */
  if ( method != "Threshold"
    && method != "OtsuThreshold"
    && method != "OtsuMultipleThreshold"
    && method != "AdaptiveOtsuThreshold"
    && method != "RobustAutomaticThreshold"
    && method != "KappaSigmaThreshold"
    && method != "MinErrorThreshold" )
  {
    std::cerr << "ERROR: method \"-m\" should be one of { Threshold, "
      << "OtsuThreshold, OtsuMultipleThreshold, AdaptiveOtsuThreshold, "
      << "RobustAutomaticThreshold, KappaSigmaThreshold, MinErrorThreshold }." << std::endl;
    return 1;
  }
  if ( method == "KappaSigmaThreshold" && maskFileName == "" )
  {
    std::cerr << "ERROR: the method \"KappaSigmaThreshold\" requires setting a mask using \"-mask\"." << std::endl;
    return 1;
  }

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
  ITKToolsThresholdImageBase * thresholdImage = 0; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName);
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< char, 2 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< unsigned char, 2 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< short, 2 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< unsigned short, 2 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< float, 2 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< double, 2 >::New( componentType, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< char, 3 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< unsigned char, 3 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< short, 3 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< unsigned short, 3 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< float, 3 >::New( componentType, dim );
    if (!thresholdImage) thresholdImage = ITKToolsThresholdImageSelector< double, 3 >::New( componentType, dim );
#endif
    if (!thresholdImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    thresholdImage->m_Bins = bins;
    thresholdImage->m_InputFileName = inputFileName;
    thresholdImage->m_Inside = inside;
    thresholdImage->m_Iterations = iterations;
    thresholdImage->m_MaskFileName = maskFileName;
    thresholdImage->m_MaskValue = maskValue;
    thresholdImage->m_Method = method;
    thresholdImage->m_MixtureType = mixtureType;
    thresholdImage->m_NumThresholds = numThresholds;
    thresholdImage->m_OutputFileName = outputFileName;
    thresholdImage->m_Outside = outside;
    thresholdImage->m_Pow = pow;
    thresholdImage->m_Sigma = sigma;
    thresholdImage->m_Threshold1 = threshold1;
    thresholdImage->m_Threshold2 = threshold2;

    thresholdImage->Run();
    
    delete thresholdImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete thresholdImage;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
