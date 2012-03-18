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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "thresholdimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program thresholds an image.\n"
    << "Usage:\n"
    << "pxthresholdimage\n"
    << "  -in        inputFilename\n"
    << "  [-out]     outputFilename; default in + THRESHOLDED.mhd\n"
    << "  [-mask]    maskFilename, optional for \"OtsuThreshold\", required for \"KappaSigmaThreshold\"\n"
    << "  [-m]       method, choose one of \n"
    << "               {Threshold, OtsuThreshold, OtsuMultipleThreshold,\n"
    << "               AdaptiveOtsuThreshold, RobustAutomaticThreshold,\n"
    << "               KappaSigmaThreshold, MinErrorThreshold }\n"
    << "             default \"Threshold\"\n"
    << "  [-t1]      lower threshold, for \"Threshold\", default -infinity\n"
    << "  [-t2]      upper threshold, for \"Threshold\", default 1.0\n"
    << "  [-inside]  inside value, default 1\n"
    << "  [-outside] outside value, default 0\n"
    << "  [-t]       number of thresholds, for \"OtsuMultipleThreshold\", default 1\n"
    << "  [-b]       number of histogram bins, for \"OtsuThreshold\", \"MinErrorThreshold\"\n"
    << "               and \"AdaptiveOtsuThreshold\", default 128\n"
    << "  [-r]       radius, for \"AdaptiveOtsuThreshold\", default 8\n"
    << "  [-cp]      number of control points, for \"AdaptiveOtsuThreshold\", default 50\n"
    << "  [-l]       number of levels, for \"AdaptiveOtsuThreshold\", default 3\n"
    << "  [-s]       number of samples, for \"AdaptiveOtsuThreshold\", default 5000\n"
    << "  [-o]       spline order, for \"AdaptiveOtsuThreshold\", default 3\n"
    << "  [-p]       power, for \"RobustAutomaticThreshold\", default 1\n"
    << "  [-sigma]   sigma factor, for \"KappaSigmaThreshold\", default 2\n"
    << "  [-iter]    number of iterations, for \"KappaSigmaThreshold\", default 2\n"
    << "  [-mv]      mask value, for \"KappaSigmaThreshold\", default 1\n"
    << "  [-mt]      mixture type (1 - Gaussians, 2 - Poissons), for \"MinErrorThreshold\", default 1\n"
    << "  [-z]       compression flag; if provided, the output image is compressed\n\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

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

  bool useCompression = parser->ArgumentExists( "-z" );

  /** Checks. */
  if( method != "Threshold"
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
    return EXIT_FAILURE;
  }
  if( method == "KappaSigmaThreshold" && maskFileName == "" )
  {
    std::cerr << "ERROR: the method \"KappaSigmaThreshold\" requires setting a mask using \"-mask\"." << std::endl;
    return EXIT_FAILURE;
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsThresholdImageBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsThresholdImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsThresholdImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsThresholdImage< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_Bins = bins;
    filter->m_InputFileName = inputFileName;
    filter->m_Inside = inside;
    filter->m_Iterations = iterations;
    filter->m_MaskFileName = maskFileName;
    filter->m_MaskValue = maskValue;
    filter->m_Method = method;
    filter->m_MixtureType = mixtureType;
    filter->m_NumThresholds = numThresholds;
    filter->m_OutputFileName = outputFileName;
    filter->m_Outside = outside;
    filter->m_Pow = pow;
    filter->m_Sigma = sigma;
    filter->m_Threshold1 = threshold1;
    filter->m_Threshold2 = threshold2;
    filter->m_UseCompression = useCompression;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
