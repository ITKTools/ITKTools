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
 \brief Calculates the closest rigid transform (VersorRigid3D) between two sets of landmarks. The two sets should be of equal size.
 
 \verbinclude closestversor3Dtransform.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"

#include "closestversor3Dtransform.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Calculates the closest rigid transform (VersorRigid3D) between\n"
    << "two sets of landmarks. The two sets should be of equal size.\n"
    << "Usage:\n"
    << "pxclosestversor3Dtransform\n"
    << "-f       the file containing the fixed landmarks\n"
    << "-m       the file containing the moving landmarks";
  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-f", "The fixed landmark filename." );
  parser->MarkArgumentAsRequired( "-m", "The moving landmark filename." );

  /** Get arguments. */
  std::string fixedLandmarkFileName = "";
  parser->GetCommandLineArgument( "-f", fixedLandmarkFileName );

  std::string movingLandmarkFileName = "";
  parser->GetCommandLineArgument( "-m", movingLandmarkFileName );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Compute the closest rigid transformation. */
  std::vector<double> parVersor, centerOfRotation;
  try
  {
    ComputeClosestVersor(
      fixedLandmarkFileName, movingLandmarkFileName,
      parVersor, centerOfRotation );
  }
  catch( itk::ExceptionObject &excp )
  {
    std::cerr << "Caught ITK exception: " << excp << std::endl;
    return EXIT_FAILURE;
  }

  /** Convert from versor to Euler angles. */
  std::vector<double> parEuler;
  ConvertVersorToEuler( parVersor, parEuler );

  /** Print. */
  std::cout << std::fixed;
  std::cout << std::showpoint;
  std::cout << std::setprecision( 6 );
  unsigned int nop = parVersor.size();

  std::cout << "versor: ";
  for( unsigned int i = 0; i < nop - 1; i++ )
  {
    std::cout << parVersor[ i ] << " ";
  }
  std::cout << parVersor[ nop - 1 ] << std::endl;

  std::cout << "Euler: ";
  for( unsigned int i = 0; i < nop - 1; i++ )
  {
    std::cout << parEuler[ i ] << " ";
  }
  std::cout << parEuler[ nop - 1 ] << std::endl;

  std::cout << "center of rotation: ";
  for( unsigned int i = 0; i < 2; i++ )
  {
    std::cout << centerOfRotation[ i ] << " ";
  }
  std::cout << centerOfRotation[ 2 ] << std::endl;

  /** End program. */
  return EXIT_SUCCESS;

} // end main
