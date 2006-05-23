/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconvertMultiComponent.cxx,v $
  Language:  C++
  Date:      $Date: 2006-05-23 12:17:41 $
  Version:   $Revision: 1.1 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "castconverthelpers.h"

/** nD images with 2 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent2D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );

/** nD images with 3 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent2D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );

/** nD images with 4 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent2D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent3D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );
extern int FileConverterMultiComponent4D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension );

int FileConverterMultiComponent( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension )
{
  /** Support for 2D images. */
  if ( inputDimension == 2 )
  {
		if ( numberOfComponents == 2 )
		{
			const int ret_value = 
				FileConverterMultiComponent2D2CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent2D2CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 3 )
		{
			const int ret_value = 
				FileConverterMultiComponent2D3CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent2D3CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 4 )
		{
			const int ret_value = 
				FileConverterMultiComponent2D4CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent2D4CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else
		{
			std::cerr << "numberOfComponents equals " << numberOfComponents << ", which is not supported." << std::endl;
			std::cerr << "Only 2, 3, and 4 numberOfComponents are supported." << std::endl;
			return 1;
		}
  } // end support for 2D images
  else if ( inputDimension == 3 )
  {
		if ( numberOfComponents == 2 )
		{
			const int ret_value = 
				FileConverterMultiComponent3D2CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent3D2CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 3 )
		{
			const int ret_value = 
				FileConverterMultiComponent3D3CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent3D3CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 4 )
		{
			const int ret_value = 
				FileConverterMultiComponent3D4CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent3D4CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else
		{
			std::cerr << "numberOfComponents equals " << numberOfComponents << ", which is not supported." << std::endl;
			std::cerr << "Only 2, 3, and 4 numberOfComponents are supported." << std::endl;
			return 1;
		}
  } // end support for 3D images
  else if ( inputDimension == 4 )
  {
		if ( numberOfComponents == 2 )
		{
			const int ret_value = 
				FileConverterMultiComponent4D2CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent4D2CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 3 )
		{
			const int ret_value = 
				FileConverterMultiComponent4D3CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent4D3CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else if ( numberOfComponents == 4 )
		{
			const int ret_value = 
				FileConverterMultiComponent4D4CA( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension )
				|| FileConverterMultiComponent4D4CB( inputPixelComponentType, outputPixelComponentType,
				numberOfComponents, inputFileName, outputFileName, inputDimension );
			if ( ret_value != 0 )
			{
				return ret_value;
			}
		}
		else
		{
			std::cerr << "numberOfComponents equals " << numberOfComponents << ", which is not supported." << std::endl;
			std::cerr << "Only 2, 3, and 4 numberOfComponents are supported." << std::endl;
			return 1;
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

} // end support for multi-component pixel type

