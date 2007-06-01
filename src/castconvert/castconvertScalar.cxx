/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconvertScalar.cxx,v $
  Language:  C++
  Date:      $Date: 2007-06-01 13:00:06 $
  Version:   $Revision: 1.3 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "castconverthelpers.h"

extern int FileConverterScalar2DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );
extern int FileConverterScalar2DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );
extern int FileConverterScalar3DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );
extern int FileConverterScalar3DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );
extern int FileConverterScalar4DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );
extern int FileConverterScalar4DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression );

int FileConverterScalar( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension, bool useCompression )
{
  /** Support for 2D images. */
  if ( inputDimension == 2 )
  {
    const int ret_value = FileConverterScalar2DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression )
    ||                  FileConverterScalar2DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression );
    if ( ret_value != 0 )
    {
      return ret_value;
    }
  }
  else if ( inputDimension == 3 )
  {
    const int ret_value = FileConverterScalar3DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression  )
    ||                  FileConverterScalar3DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression  );
    if ( ret_value != 0 )
    {
      return ret_value;
    }
  } // end support for 3D images
  else if ( inputDimension == 4 )
  {
    const int ret_value = FileConverterScalar4DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression )
    ||                  FileConverterScalar4DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension, useCompression );
    if ( ret_value != 0 )
    {
      return ret_value;
    }
  } // end support for 4D images
  else
  {
    std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
    std::cerr << "Only 2D, 3D, and 4D images are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a succes value. */
  return 0;

} // end support for SCALAR pixel type
