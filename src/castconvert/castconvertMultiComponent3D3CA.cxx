/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconvertMultiComponent3D3CA.cxx,v $
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

int FileConverterMultiComponent3D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression )
{
  enum { ImageDims = 3 };
	enum { VecDims = 3 };

  if ( inputDimension == ImageDims )
  {
		if ( numberOfComponents == VecDims )
		{
			/** From unsigned char to something else. */
			callCorrectReadWriterMultiComponentMacro( unsigned char, unsigned char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, unsigned short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, unsigned int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, unsigned long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, float, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned char, double, VecDims, ImageDims );

			/** From char to something else. */
			callCorrectReadWriterMultiComponentMacro( char, unsigned char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, unsigned short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, unsigned int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, unsigned long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, float, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( char, double, VecDims, ImageDims );

			/** From unsigned short to something else. */
			callCorrectReadWriterMultiComponentMacro( unsigned short, unsigned char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, unsigned short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, unsigned int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, unsigned long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, float, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned short, double, VecDims, ImageDims );

			/** From short to something else. */
			callCorrectReadWriterMultiComponentMacro( short, unsigned char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, unsigned short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, unsigned int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, unsigned long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, float, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( short, double, VecDims, ImageDims );

			/** From unsigned int to something else. */
			callCorrectReadWriterMultiComponentMacro( unsigned int, unsigned char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, char, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, unsigned short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, short, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, unsigned int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, int, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, unsigned long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, long, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, float, VecDims, ImageDims );
			callCorrectReadWriterMultiComponentMacro( unsigned int, double, VecDims, ImageDims );
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
