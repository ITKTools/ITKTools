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
#ifndef __castconverthelpers2_h__
#define __castconverthelpers2_h__

#include "itkCommandLineArgumentParser.h"
#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"

/**
 * ******************* GetCommandLineArguments *******************
 */

bool GetCommandLineArguments( itk::CommandLineArgumentParser::Pointer parser,
  std::string & input,
  std::string & outputFileName,
  std::string & outputPixelComponentType,
  std::string & seriesUID,
  std::vector<std::string> & restrictions,
  bool & useCompression )
{
  /** Get arguments. */
  parser->GetCommandLineArgument( "-in", input );
  parser->GetCommandLineArgument( "-out", outputFileName );
  parser->GetCommandLineArgument( "-opct", outputPixelComponentType );
  parser->GetCommandLineArgument( "-s", seriesUID );
  parser->GetCommandLineArgument( "-r", restrictions );
  useCompression = parser->ArgumentExists( "-z" );

  /** Check outputPixelType. */
  if ( outputPixelComponentType != ""
    && outputPixelComponentType != "unsigned_char"
    && outputPixelComponentType != "char"
    && outputPixelComponentType != "unsigned_short"
    && outputPixelComponentType != "short"
    && outputPixelComponentType != "unsigned_int"
    && outputPixelComponentType != "int"
    && outputPixelComponentType != "unsigned_long"
    && outputPixelComponentType != "long"
    && outputPixelComponentType != "float"
    && outputPixelComponentType != "double" )
  {
    /** In this case an illegal outputPixelComponentType is given. */
    std::cerr << "The given outputPixelComponentType is " << 
      outputPixelComponentType << " which is not supported.";
    return false;
  }

  return true; // everything went well
} // end GetCommandLineArguments()


/**
 * ******************* IsDICOM *******************
 */

int IsDICOM( std::string & input, std::string & errorMessage, bool & isDICOM )
{
  /** Make sure last character of input != "/".
   * Otherwise FileIsDirectory() won't work.
   */
  if ( input.rfind( "/" ) == input.size() - 1 )
  {
    input.erase( input.size() - 1, 1 );
  }

  /** Check if input is a file or a directory. */
  bool exists = itksys::SystemTools::FileExists( input.c_str() );
  bool isDir = itksys::SystemTools::FileIsDirectory( input.c_str() );

  if ( exists && !isDir ) isDICOM = false;
  else if ( exists && isDir ) isDICOM = true;
  else
  {
    /** Something is wrong. */
    errorMessage = "ERROR: " + input + " does not exist.";
    return 1;
  }

  /** Return a value. */
  return 0;

} // end IsDICOM()


/**
 * ******************* GetFileNameFromDICOMDirectory *******************
 */

int GetFileNameFromDICOMDirectory(
  const std::string & inputDirectoryName,
  std::string & fileName,
  const std::string & seriesUID,
  const std::vector<std::string> & restrictions,
  std::string & errorMessage )
{
  typedef itk::GDCMSeriesFileNames                GDCMNamesGeneratorType;
  typedef std::vector< std::string >              FileNamesContainerType;

  /** Create vector of filenames from the DICOM directory. */
  GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails( true );
  for ( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );

  /** The short and fast way. */
  FileNamesContainerType fileNames;
  if ( seriesUID == "" )
  {
    fileNames = nameGenerator->GetInputFileNames();
    if ( fileNames.size() != 0 )
    {
      fileName = fileNames[ 0 ];
      return 0;
    }
    else
    {
      std::cerr << "ERROR: No files found in this directory." << std::endl;
      return 1;
    }
  }

  /** Get all series in this directory. */
  FileNamesContainerType seriesNames = nameGenerator->GetSeriesUIDs();
  if ( !seriesNames.size() )
  {
    errorMessage = "ERROR: no DICOM series in directory " + inputDirectoryName + ".";
    return 1;
  }

  /** Get a list of files in series. */
  fileNames = nameGenerator->GetFileNames( seriesUID );
  if ( !fileNames.size() )
  {
    errorMessage = "ERROR: no DICOM series " + seriesUID
      + " in directory " + inputDirectoryName + ".";
    return 1;
  }

  /** Get a name of a 2D image. */
  fileName = fileNames[ 0 ];

  /** Return a value. */
  return 0;

} // end GetFileNameFromDICOMDirectory()


#endif //__castconverthelpers2_h__
