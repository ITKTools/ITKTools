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
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::string helpText = "Description:\
   This is done by reading in an image, possibly casting of the image,\
   and subsequently writing the image to some format.\
   With converting we mean changing the extension of the image,\
   such as bmp, mhd, etc. With casting we mean changing the component\
   type of a voxel, such as short, unsigned long, float.\
   Casting is currently done for scalar images using the ShiftScaleImageFilter,\
   where values are mapped to itself, leaving the intensity range\
   the same. NOTE that when casting to a component type with a\
   smaller dynamic range, information might get lost. In this case\
   we might use the RescaleIntensityImageFilter to linearly\
   rescale the image values. For multi-component images, such as vector\
   or RGB images, casting is done using the itk::VectorCastImageFilter.\
   Currently supported are the SCALAR pixel types and also multi-\
   component pixel types, such as vector and RGB pixels. For multi-\
   component pixel types, everything is read in as an itk::Vector with\
   the correct pixel component type and number of components. This is\
   also the case for the writer.\
   Input images can be in all file formats ITK supports and for which\
   the ImageFileReader works, and additionally 3D dicom series\
   using the ImageSeriesReader. It is also possible to extract a specific\
   DICOM series from a directory by supplying the seriesUID. The pixel\
   component type should of course be a component type supported by the\
   file format. Output images can be in all file formats ITK supports and\
   for which the ImageFileReader works, so no dicom output is\
   currently supported.\
   Usage:\
     pxcastconvert -in inputFileName -out outputFileName [-opct outputPixelComponentType]\
   OR:\
     pxcastconvert -in dicomDirectoryName -out outputFileName\
                  [-opct outputPixelComponentType] [-s seriesUID]\
   Where outputPixelComponentType is one of: unsigned_char, char,\
   unsigned_short, short, unsigned_int, int, unsigned_long, long,\
   float or double.\
   By default the outputPixelComponentType is set to the inputPixelComponentType.\
   By default the seriesUID is the first UID found.\
  Usage:\n \
  pxcastconvert\n \
    -in      inputfilename\n \
    -out     outputfilename\n \
    [-opct]  outputPixelComponentType\n \
    [-z]     compression flag; if provided, the output image \
  is compressed\n \
  OR pxcastconvert\n \
    -in      dicomDirectory\n \
    -out     outputfilename\n \
    [-opct]  outputPixelComponentType\n \
    [-s]     seriesUID\n \
    [-r]     add restrictions to generate a unique seriesUID\n \
             e.g. \"0020|0012\" to add a check for acquisition number.\n \
    [-z]     compression flag; if provided, the output image \
  is compressed\n \
  where outputPixelComponentType is one of:\n \
    [unsigned_]char, [unsigned_]short, [unsigned_]int,\n \
    [unsigned_]long, float, double,\n \
  provided that the outputPixelComponentType is supported by \
  the output file format.\n \
  By default the outputPixelComponentType is set to the \
  inputPixelComponentType.\n \
  By default the seriesUID is the first UID found.\n \
  The compression flag \"-z\" may be ignored by some output \
  image formats.";

  return helpText;

} // end PrintHelp()


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
