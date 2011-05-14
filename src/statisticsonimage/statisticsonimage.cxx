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
 \brief Compute statistics on an image. For vector images, the magnitude is used.
 
 \verbinclude statisticsonimage.help
 */

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "ITKToolsBase.h"

#include "statisticsonimage.h"

/** This program determines the minimum, maximum,
 * mean, sigma, variance, and sum of an image, or its magnitude/jacobian.
 */

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim,nrofcomp) \
if ( ComponentType == #type && Dimension == dim && NumberOfComponents == nrofcomp) \
{ \
  function< type, dim, nrofcomp >( inputFileName, maskFileName, histogramOutputFileName,\
    numberOfBins, select ); \
  supported = true; \
}

/** Declare GetHelpString, implemented at the bottom of this file. */
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

  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  std::string histogramOutputFileName = "";
  parser->GetCommandLineArgument( "-out", histogramOutputFileName );

  unsigned int numberOfBins = 100;
  parser->GetCommandLineArgument( "-b", numberOfBins );

  std::string select = "";
  bool rets = parser->GetCommandLineArgument( "-s", select );

  /** Check selection. */
  if ( rets && ( select != "arithmetic" && select != "geometric"
    && select != "histogram" ) )
  {
    std::cerr << "ERROR: -s should be one of {arithmetic, geometric, histogram}"
      << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tComponentType:      " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** force images to sneaky be converted to doubles */
  
  
  /** Class that does the work */
  StatisticsOnImageBase * statisticsOnImage = NULL; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  //itktools::EnumComponentType componentType = itktools::GetImageComponentType(inputFileName);
  itktools::EnumComponentType componentType = itk::ImageIOBase::FLOAT;
  
  unsigned int numberOfComponents = 0;
  GetImageNumberOfComponents(inputFileName, numberOfComponents);
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;
    
  try
  {    
    // now call all possible template combinations.
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 2, 1 >::New( componentType, dim, numberOfComponents );
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 2, 2 >::New( componentType, dim, numberOfComponents );
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 2, 3 >::New( componentType, dim, numberOfComponents );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 3, 1 >::New( componentType, dim, numberOfComponents );
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 3, 2 >::New( componentType, dim, numberOfComponents );
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 3, 3 >::New( componentType, dim, numberOfComponents );
#endif
#ifdef ITKTOOLS_4D_SUPPORT
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 4, 1 >::New( componentType, dim, numberOfComponents );
    if (!statisticsOnImage) statisticsOnImage = StatisticsOnImage< float, 4, 4 >::New( componentType, dim, numberOfComponents );
#endif
    if (!statisticsOnImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    statisticsOnImage->m_InputFileName = inputFileName;
    statisticsOnImage->m_MaskFileName = maskFileName;
    statisticsOnImage->m_HistogramOutputFileName = histogramOutputFileName;
    statisticsOnImage->m_NumberOfBins = numberOfBins;
    statisticsOnImage->m_Select = select;
  
    statisticsOnImage->Run();
    
    delete statisticsOnImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete statisticsOnImage;
    return 1;
  }


  /** End program. */
  return 0;

} // end main


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Compute statistics on an image. For vector images, the magnitude is used." << std::endl
  << "Usage:" << std::endl
  << "pxstatisticsonimage" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFileName for histogram;" << std::endl
  << "           if omitted, no histogram is written; default: <empty>" << std::endl
  << "  [-mask]  MaskFileName, mask should have the same size as the input image" << std::endl
  << "           and be of pixeltype (convertable to) unsigned char," << std::endl
  << "           1 = within mask, 0 = outside mask;" << std::endl
  << "  [-b]     NumberOfBins to use for histogram, default: 100;" << std::endl
  << "           for an accurate estimate of median and quartiles" << std::endl
  << "           for integer images, choose the number of bins" << std::endl
  << "           much larger (~100x) than the number of gray values." << std::endl
  << "           if equal 0, then the intensity range (max - min) is chosen." << std::endl
  << "  [-s]     select which to compute {arithmetic, geometric, histogram}, default all;" << std::endl
  << "Supported: 2D, 3D, 4D, float, (unsigned) short, (unsigned) char, 1, 2 or 3 components per pixel." << std::endl
  << "For 4D, only 1 or 4 components per pixel are supported.";

  return ss.str();

} // end GetHelpString()
