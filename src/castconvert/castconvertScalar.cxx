/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconvertScalar.cxx,v $
  Language:  C++
  Date:      $Date: 2006-05-23 12:17:42 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "castconverthelpers.h"

extern int FileConverterScalar2DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );
extern int FileConverterScalar2DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );
extern int FileConverterScalar3DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );
extern int FileConverterScalar3DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );
extern int FileConverterScalar4DA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );
extern int FileConverterScalar4DB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension );

int FileConverterScalar( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const std::string &inputFileName,
  const std::string &outputFileName, const unsigned int inputDimension )
{
  /** Support for 2D images. */
  if ( inputDimension == 2 )
  {
    const int ret_value = FileConverterScalar2DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension )
    ||                  FileConverterScalar2DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension );
    if ( ret_value != 0 )
    {
      return ret_value;
    }
  }
  else if ( inputDimension == 3 )
  {
    const int ret_value = FileConverterScalar3DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension )
    ||                  FileConverterScalar3DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension );
    if ( ret_value != 0 )
    {
      return ret_value;
    }
  } // end support for 3D images
  else if ( inputDimension == 4 )
  {
    const int ret_value = FileConverterScalar4DA(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension )
    ||                  FileConverterScalar4DB(
      inputPixelComponentType, outputPixelComponentType,
      inputFileName, outputFileName, inputDimension );
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
