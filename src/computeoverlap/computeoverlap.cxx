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
 \brief Compute the overlap of two images.
 
 \verbinclude computeoverlap.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "ComputeOverlapOld.h"
//#include "ComputeOverlap2.h"
#include "ComputeOverlap3.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
    << "pxcomputeoverlap" << std::endl
    << "This program computes the overlap of two images." << std::endl
    << "By default the overlap of nonzero regions is computed." << std::endl
    << "Masks of a valid region are also taken into account." << std::endl
    << "If the images are not binary, you can specify threshold values." << std::endl
    << "The results is computed as:" << std::endl
    << "   2 * L1( (im1 AND mask2) AND (im2 AND mask1) )" << std::endl
    << "  ----------------------------------------------" << std::endl
    << "       L1(im1 AND mask2) + L1(im2 AND mask1)" << std::endl
    << "  -in      inputFilename1 inputFilename2" << std::endl
    << "  [-mask1] maskFilename1" << std::endl
    << "  [-mask2] maskFilename2" << std::endl
    << "  [-t1]    threshold1" << std::endl
    << "  [-t2]    threshold2" << std::endl
    << "  [-l]     alternative implementation using label values" << std::endl
    << "          the overlap of exactly corresponding labels is computed" << std::endl
    << "           if \"-l\" is specified with no arguments, all labels in im1 are used," << std::endl
    << "           otherwise (e.g. \"-l 1 6 19\") the specified labels are used." << std::endl
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  /** Get arguments. */
  std::vector<std::string> inputFileNames;
  bool retin = parser->GetCommandLineArgument( "-in", inputFileNames );

  parser->MarkArgumentAsRequired( "-in", "Two input filenames." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  std::string maskFileName1 = "";
  parser->GetCommandLineArgument( "-mask1", maskFileName1 );
  std::string maskFileName2 = "";
  parser->GetCommandLineArgument( "-mask2", maskFileName2 );

  unsigned int t1 = 0;
  parser->GetCommandLineArgument( "-t1", t1 );
  unsigned int t2 = 0;
  parser->GetCommandLineArgument( "-t2", t2 );

  bool retlabel = parser->ArgumentExists( "-l" ); // default all labels
  std::vector<unsigned int> labels( 0 );
  parser->GetCommandLineArgument( "-l", labels );

  /** Checks. */
  if ( !retin || inputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify two input file names with \"-in\"." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string PixelType = "VECTOR";
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = Dimension;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Checks. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentType );

  /** Run the program. */

  unsigned int dim = Dimension;
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileNames[0] );

  if ( retlabel )
  {
    ITKToolsComputeOverlap3Base * computeOverlap3 = 0; 
    try
    {
      // now call all possible template combinations.
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< char, 2 >::New( componentType, dim );
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< short, 2 >::New( componentType, dim );
#ifdef ITKTOOLS_3D_SUPPORT
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< char, 3 >::New( componentType, dim );
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< unsigned char, 3 >::New( componentType, dim );
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< short, 3 >::New( componentType, dim );
      if (!computeOverlap3) computeOverlap3 = ITKToolsComputeOverlap3< unsigned short, 3 >::New( componentType, dim );
#endif
      if (!computeOverlap3) 
      {
        std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
        std::cerr
          << "pixel (component) type = "
          << itk::ImageIOBase::GetComponentTypeAsString( componentType )
          << " ; dimension = " << Dimension
          << std::endl;
        return 1;
      }

      computeOverlap3->m_InputFileNames = inputFileNames;
      computeOverlap3->m_Labels = labels;

      computeOverlap3->Run();
      
      delete computeOverlap3;  
    }
    catch( itk::ExceptionObject &e )
    {
      std::cerr << "Caught ITK exception: " << e << std::endl;
      delete computeOverlap3;
      return 1;
    }

  }
  else
  {
    ITKToolsComputeOverlapOldBase * computeOverlapOld = 0; 
    try
    {
      // now call all possible template combinations.
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< char, 2 >::New( componentType, dim );
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< short, 2 >::New( componentType, dim );
#ifdef ITKTOOLS_3D_SUPPORT
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< char, 3 >::New( componentType, dim );
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< unsigned char, 3 >::New( componentType, dim );
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< short, 3 >::New( componentType, dim );
      if (!computeOverlapOld) computeOverlapOld = ITKToolsComputeOverlapOld< unsigned short, 3 >::New( componentType, dim );
#endif
      if (!computeOverlapOld) 
      {
        std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
        std::cerr
          << "pixel (component) type = " << componentType
          << " ; dimension = " << Dimension
          << std::endl;
        return 1;
      }

      computeOverlapOld->m_InputFileNames = inputFileNames;
      computeOverlapOld->m_MaskFileName1 = maskFileName1;
      computeOverlapOld->m_MaskFileName2 = maskFileName2;
      computeOverlapOld->m_T1 = t1;
      computeOverlapOld->m_T2 = t2;

      computeOverlapOld->Run();
      
      delete computeOverlapOld;  
    }
    catch( itk::ExceptionObject &e )
    {
      std::cerr << "Caught ITK exception: " << e << std::endl;
      delete computeOverlapOld;
      return 1;
    }
  }

  /** End program. */
  return 0;

} // end main
