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
#include "castconverthelpers.h"

/** nD images with 2 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent2D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D2CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D2CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );

/** nD images with 3 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent2D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D3CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D3CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );

/** nD images with 4 components. n = {2,3,4}. */
extern int FileConverterMultiComponent2D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent2D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent3D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D4CA( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );
extern int FileConverterMultiComponent4D4CB( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression );

int FileConverterMultiComponent( const std::string &inputPixelComponentType,
  const std::string &outputPixelComponentType, const unsigned int numberOfComponents,
  const std::string &inputFileName, const std::string &outputFileName,
  const unsigned int inputDimension, bool useCompression )
{
  /** Support for 2D images. */
  if ( inputDimension == 2 )
  {
    if ( numberOfComponents == 2 )
    {
      const int ret_value =
        FileConverterMultiComponent2D2CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent2D2CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 3 )
    {
      const int ret_value =
        FileConverterMultiComponent2D3CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent2D3CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 4 )
    {
      const int ret_value =
        FileConverterMultiComponent2D4CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent2D4CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
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
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent3D2CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 3 )
    {
      const int ret_value =
        FileConverterMultiComponent3D3CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent3D3CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 4 )
    {
      const int ret_value =
        FileConverterMultiComponent3D4CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent3D4CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
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
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent4D2CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 3 )
    {
      const int ret_value =
        FileConverterMultiComponent4D3CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent4D3CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    }
    else if ( numberOfComponents == 4 )
    {
      const int ret_value =
        FileConverterMultiComponent4D4CA( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression )
        || FileConverterMultiComponent4D4CB( inputPixelComponentType, outputPixelComponentType,
        numberOfComponents, inputFileName, outputFileName, inputDimension, useCompression );
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

