/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria, Hakim Achterberg. 2011.
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
 \brief Create a summary of segmentation overlap in two images.
 
 \verbinclude computeoverlapsummary.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "computeoverlapsummary.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Computes overlap measures between the set same set of labels of pixels of two images."<< std::endl
    << "Usage:" << std::endl
    << "pxcomputeoverlap summary" << std::endl
    
    << "  -in1		Filename of first input image (Source Image)" << std::endl
    << "  -in2		Filename of second input image (Target Image)" << std::endl
    << "  -out		Filename to write the results to" << std::endl
    << "  -seperator    Seperator to use in csv file; default '\\t'" << std::endl
    << "The results file contains:" << std::endl
    << "  Union Overlap or Jaccard coefficient; Mean Overlap or Dice coefficient;" << std::endl
    << "Background is assumed to be 0. " << std::endl
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short.";

  return ss.str();

} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in1", "Filename of first input image (SourceImage)." );
  parser->MarkArgumentAsRequired( "-in2", "Filename of second input image (Target Image)." );
  parser->MarkArgumentAsRequired( "-out", "Filename to write the results to." );

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
  std::string inputImage1("");
  parser->GetCommandLineArgument( "-in1", inputImage1 );

  std::string inputImage2("");
  parser->GetCommandLineArgument( "-in2", inputImage2 );

  std::string outputFileName("");
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string seperator("\t");
  parser->GetCommandLineArgument( "-seperator", seperator );

  /** Determine image properties. */
  std::string ComponentTypeIn = "char";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputImage1,
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
  ITKToolsComputeOverlapSummaryBase * computeOverlapSummary = 0; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputImage1 );

  try
  {    
    // now call all possible template combinations.
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< char, 2 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< unsigned char, 2 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< short, 2 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< unsigned short, 2 >::New( componentType, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< char, 3 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< unsigned char, 3 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< short, 3 >::New( componentType, dim );
    if (!computeOverlapSummary) computeOverlapSummary = ITKToolsComputeOverlapSummary< unsigned short, 3 >::New( componentType, dim );
#endif
    if (!computeOverlapSummary) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    computeOverlapSummary->m_InputFileName1 = inputImage1;
    computeOverlapSummary->m_InputFileName2 = inputImage2;
    computeOverlapSummary->m_OutputFileName = outputFileName;
    computeOverlapSummary->m_Seperator      = seperator;

    computeOverlapSummary->Run();
    
    delete computeOverlapSummary;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete computeOverlapSummary;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
