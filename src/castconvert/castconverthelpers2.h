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
#ifndef __castconverthelpers2_h_
#define __castconverthelpers2_h_

#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"


// NOTE that these functions can not be moved to castconverthelpers.h,
// otherwise we get linker errors.


/**
 * ******************* IsDICOM *******************
 */

bool IsDICOM( std::string & input, bool & isDICOM )
{
  /** Make sure last character of input != "/".
   * Otherwise FileIsDirectory() won't work.
   */
  if( input.rfind( "/" ) == input.size() - 1 )
  {
    input.erase( input.size() - 1, 1 );
  }

  /** Check if input is a file or a directory. */
  bool exists = itksys::SystemTools::FileExists( input.c_str() );
  bool isDir = itksys::SystemTools::FileIsDirectory( input.c_str() );

  if( exists && !isDir ) isDICOM = false;
  else if( exists && isDir ) isDICOM = true;
  /** Something is wrong. */
  else return false;

  /** Return a value. */
  return true;

} // end IsDICOM()


/**
 * ******************* GetFileNameFromDICOMDirectory *******************
 */

bool GetFileNameFromDICOMDirectory(
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
  for( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );

  /** The short and fast way. */
  FileNamesContainerType fileNames;
  if( seriesUID == "" )
  {
    fileNames = nameGenerator->GetInputFileNames();
    if( fileNames.size() != 0 )
    {
      fileName = fileNames[ 0 ];
      return true;
    }
    else
    {
      std::cerr << "ERROR: No files found in this directory." << std::endl;
      return false;
    }
  }

  /** Get all series in this directory. */
  FileNamesContainerType seriesNames = nameGenerator->GetSeriesUIDs();
  if( !seriesNames.size() )
  {
    errorMessage = "ERROR: no DICOM series in directory " + inputDirectoryName + ".";
    return false;
  }

  /** Get a list of files in series. */
  fileNames = nameGenerator->GetFileNames( seriesUID );
  if( !fileNames.size() )
  {
    errorMessage = "ERROR: no DICOM series " + seriesUID
      + " in directory " + inputDirectoryName + ".";
    return false;
  }

  /** Get a name of a 2D image. */
  fileName = fileNames[ 0 ];

  /** Return a value. */
  return true;

} // end GetFileNameFromDICOMDirectory()


#endif //__castconverthelpers2_h_
