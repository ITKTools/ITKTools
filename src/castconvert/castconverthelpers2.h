/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconverthelpers2.h,v $
  Language:  C++
  Date:      $Date: 2007-06-26 14:23:26 $
  Version:   $Revision: 1.5 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __castconverthelpers2_h__
#define __castconverthelpers2_h__

#include "itkCommandLineArgumentParser.h"
#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"

/**
 * ******************* PrintHelp *******************
 */

std::string PrintHelp( void )
{
  std::string helpText = \
  "Usage:\n \
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
