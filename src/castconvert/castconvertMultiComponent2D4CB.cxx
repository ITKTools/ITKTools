/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconvertMultiComponent2D4CB.cxx,v $
  Language:  C++
  Date:      $Date: 2007-06-01 13:00:06 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "castconverthelpers.h"

int FileConverterMultiComponent2D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression )
{
  enum { ImageDims = 2 };
  enum { VecDims = 4 };

  if ( inputDimension == ImageDims )
  {
    if ( numberOfComponents == VecDims )
    {
      /** From int to something else. */
      callCorrectReadWriterMultiComponentMacro( int, unsigned char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, unsigned short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, unsigned int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, unsigned long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, float, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( int, double, VecDims, ImageDims );

      /** From unsigned long to something else. */
      callCorrectReadWriterMultiComponentMacro( unsigned long, unsigned char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, unsigned short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, unsigned int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, unsigned long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, float, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( unsigned long, double, VecDims, ImageDims );

      /** From long to something else. */
      callCorrectReadWriterMultiComponentMacro( long, unsigned char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, unsigned short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, unsigned int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, unsigned long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, float, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( long, double, VecDims, ImageDims );

      /** From float to something else. */
      callCorrectReadWriterMultiComponentMacro( float, unsigned char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, unsigned short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, unsigned int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, unsigned long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, float, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( float, double, VecDims, ImageDims );

      /** From double to something else. */
      callCorrectReadWriterMultiComponentMacro( double, unsigned char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, char, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, unsigned short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, short, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, unsigned int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, int, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, unsigned long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, long, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, float, VecDims, ImageDims );
      callCorrectReadWriterMultiComponentMacro( double, double, VecDims, ImageDims );
    } // end if over VecDims
  }
  else
  {
    std::cerr << "The image dimension equals " << inputDimension
      << " and the number of components equals " << numberOfComponents
      << ", which is not supported." << std::endl;
    std::cerr << "Only "<< ImageDims << "D images with " << VecDims << " components are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a value. */
  return 0;

} // end support for multi-component pixel type
