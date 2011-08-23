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
 \brief This program creates a signed distance transform.
 
 \verbinclude distancetransform.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "distancetransform.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "This program creates a signed distance transform." << std::endl
    << "Usage:\n"
    << "pxdistancetransform\n"
    << "  -in      inputFilename: the input image (a binary mask\n"
    << "           threshold at 0 is performed if the image is not binary).\n"
    << "  -out     outputFilename: the output of distance transform\n"
    << "  [-s]     flag: if set, output squared distances instead of distances\n"
    << "  [-m]     method, one of {Maurer, Danielsson, Morphological, MorphologicalSigned}, default Maurer\n"
    << "Note: voxel spacing is taken into account. Voxels inside the\n"
    << "object (=1) receive a negative distance.\n"
    << "Supported: 2D/3D. input: unsigned char, output: float";
  //std::cout << "  [-m]     method, one of {Maurer, Danielsson, OrderK}, default Maurer\n";
  //std::cout << "  [-K]     for method \"OrderK\", specify K, default 5\n";
  return ss.str();

} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get the input segmentation file name (mandatory). */
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get whether the squared distance should be returned instead of the
   * distance. Default: false, which is faster.
   */
  bool outputSquaredDistance = parser->ArgumentExists( "-s" );

  /** Get the outputFileName */
  std::vector<std::string> outputFileNames;
  parser->GetCommandLineArgument( "-out", outputFileNames );

  std::string method = "Maurer";
  parser->GetCommandLineArgument( "-m", method );

  unsigned int K = 5;
  parser->GetCommandLineArgument( "-k", K );

  /** Checks. */
  if ( method != "Maurer" && method != "Danielsson"
    && method != "Morphological" && method != "MorphologicalSigned" )
  {
    std::cerr << "ERROR: the method should be one of { Maurer, Danielsson, Morphological, MorphologicalSigned }!"
      << std::endl;
    return 1;
  }

  if ( method == "OrderK" && outputFileNames.size() != 3 )
  {
    std::cerr << "ERROR: the method OrderK requires three output file names!\n";
    std::cerr << "  You only specified " << outputFileNames.size() << "."
      << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "unsigned char";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
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

  /** Check for scalar image. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: vector images are not supported." << std::endl;
    return 1;
  }

  /** Check for dimension. */
  if ( Dimension != 2 && Dimension != 3 )
  {
    std::cerr
      << "ERROR: images of dimension "
      << Dimension
      << " are not supported!"
      << std::endl;
    return 1;
  }

  /** Run the program. */
  try
  {
    if ( Dimension == 2 )
    {
      DistanceTransform<2>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }
    if ( Dimension == 3 )
    {
      DistanceTransform<3>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }

  }
  catch( itk::ExceptionObject & e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  catch( std::exception & e )
  {
    std::cerr << "Caught std::exception: " << e.what() << std::endl;
    return 1;
  }
  catch ( ... )
  {
    std::cerr << "Caught unknown exception" << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
