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
 \brief Calculate the average magnitude of the vectors in a vector image.
 
 \verbinclude averagevectormagnitude.help
 */

#include "ITKToolsImageProperties.h"
#include "itkCommandLineArgumentParser.h"

#include "averagevectormagnitude.h"
#include "itkExceptionObject.h"

#include <iostream>
#include <string>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Calculate the average magnitude of the vectors in a vector image." << std::endl
     << "Usage:" << std::endl
     << "AverageVectorMagnitude" << std::endl
     << "-in InputVectorImageFileName" << std::endl
     << "[-out OutputImageFileName]" << std::endl
     << "-id ImageDimension" << std::endl
     << "-sd SpaceDimension (the dimension of the vectors)" << std::endl;

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  std::string inputFileName("");
  std::string outputFileName(inputFileName + "AverageVectorMagnitude.mhd");
  unsigned int imageDimension = 2;
  unsigned int spaceDimension = 1;

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-id", "Image dimension." );
  parser->MarkArgumentAsRequired( "-sd", "Space dimension." );

  parser->GetCommandLineArgument( "-in", inputFileName );
  parser->GetCommandLineArgument( "-out", outputFileName );
  parser->GetCommandLineArgument( "-sd", spaceDimension );
  parser->GetCommandLineArgument( "-id", imageDimension );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Determine image properties. */
  unsigned int numberOfComponents = 0;
  itktools::GetImageNumberOfComponents( inputFileName, numberOfComponents );

  unsigned int dimension = 0;
  itktools::GetImageDimension( inputFileName, dimension );

  ITKToolsAverageVectorMagnitudeBase * averageVectorMagnitude = 0;

  float averageMagnitude = 0.0f; // Initialize output to zero
  try
  {
    // 2D
    if (!averageVectorMagnitude) averageVectorMagnitude = ITKToolsAverageVectorMagnitude< 2, 2 >::New( numberOfComponents, dimension );
    if (!averageVectorMagnitude) averageVectorMagnitude = ITKToolsAverageVectorMagnitude< 3, 2 >::New( numberOfComponents, dimension );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!averageVectorMagnitude) averageVectorMagnitude = ITKToolsAverageVectorMagnitude< 2, 3 >::New( numberOfComponents, dimension );
    if (!averageVectorMagnitude) averageVectorMagnitude = ITKToolsAverageVectorMagnitude< 3, 3 >::New( numberOfComponents, dimension );
#endif
    if (!averageVectorMagnitude)
    {
      std::cerr << "ERROR: this combination of numberOfComponents and dimension is not supported!" << std::endl;
      std::cerr
        << "numberOfComponents = " << numberOfComponents
        << " ; dimension = " << dimension
        << std::endl;
      return 1;
    }

    averageVectorMagnitude->m_InputFileName = inputFileName;
    averageVectorMagnitude->m_OutputFileName = outputFileName;

    averageVectorMagnitude->Run();

    averageMagnitude = averageVectorMagnitude->m_AverageMagnitude;
    delete averageVectorMagnitude;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete averageVectorMagnitude;
    return 1;
  }

  return 0;

} // end function main
