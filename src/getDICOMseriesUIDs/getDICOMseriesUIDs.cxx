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
 \brief Takes a directory and searches for dicom files, from which the seriesUID's are extracted.
 
 \verbinclude getDICOMseriesUIDs.help
 */

#include "itkCommandLineArgumentParser.h"
#include <iostream>
#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxgetDICOMseriesUIDs" << std::endl
  << "  -in      inputDirectoryName" << std::endl
  << "  [-r]     add restrictions to generate a unique seriesUID" << std::endl
  << "           e.g. \"0020|0012\" to add a check for acquisition" << std::endl
  << "number.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input directory name." );

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
  std::string inputDirectoryName;
  parser->GetCommandLineArgument( "-in", inputDirectoryName );

  std::vector<std::string> restrictions;
  parser->GetCommandLineArgument( "-r", restrictions );

  /** Make sure last character of inputDirectoryName != "/".
   * Otherwise FileIsDirectory() won't work.
   */
  if ( inputDirectoryName.rfind( "/" ) == inputDirectoryName.size() - 1 )
  {
    inputDirectoryName.erase( inputDirectoryName.size() - 1, 1 );
  }

  /** Check if inputDirectoryName is a directory. */
  bool exists = itksys::SystemTools::FileExists( inputDirectoryName.c_str() );
  bool isDir = itksys::SystemTools::FileIsDirectory( inputDirectoryName.c_str() );
  isDir &= exists;
  if ( !isDir )
  {
    std::cerr << "ERROR: " << inputDirectoryName
      << " does not exist or is no directory." << std::endl;
    return 1;
  }

  typedef itk::GDCMSeriesFileNames                GDCMNamesGeneratorType;
  typedef std::vector< std::string >              FileNamesContainerType;

  /** Get the seriesUIDs from the DICOM directory. */
  GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails( true );
  for ( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );
  FileNamesContainerType seriesNames = nameGenerator->GetSeriesUIDs();

  /** Check. */
  if ( !seriesNames.size() )
  {
    std::cerr << "ERROR: no DICOM series in directory "
      << inputDirectoryName << "." << std::endl;
    return 1;
  }

  /** Print series. */
  for ( unsigned int i = 0; i < seriesNames.size(); i++ )
  {
    std::cout << seriesNames[ i ] << std::endl;
  }

  /** End  program. Return success. */
  return 0;

}  // end main
