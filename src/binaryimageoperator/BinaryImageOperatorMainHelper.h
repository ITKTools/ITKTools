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
#ifndef __BinaryImageOperatorMainHelper_h
#define __BinaryImageOperatorMainHelper_h

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>
#include "CommandLineArgumentHelper.h"
#include "itkImageIOBase.h"


/**
 * ******************* DetermineImageProperties *******************
 */

int DetermineComponentTypes(
  const std::vector<std::string> & inputFileNames,
  itktools::ComponentType & componentType1,
  itktools::ComponentType & componentType2,
  itktools::ComponentType & componentTypeOut )
{
  // Note: a bit of an ugly combination between itktools
  // and itk::ImageIOBase functionality.
  itk::ImageIOBase::Pointer imageIOBaseTmp;

  /** Determine image properties of image 1. */
  std::string pixelType1AsString = "1";
  std::string componentType1AsString = "";
  unsigned int inputDimension1 = 0;
  unsigned int numberOfComponents1 = 0;
  std::vector<unsigned int> imagesize1;
  std::vector<double> dummy;
  itktools::GetImageProperties(
    inputFileNames[ 0 ],
    pixelType1AsString, componentType1AsString,
    inputDimension1, numberOfComponents1,
    imagesize1, dummy, dummy, dummy );
  itktools::ReplaceSpaceWithUnderscore( componentType1AsString );
  componentType1 = imageIOBaseTmp->GetComponentTypeFromString( componentType1AsString );

  /** Determine image properties of image 2. */
  std::string pixelType2AsString = "2";
  std::string componentType2AsString = "";
  unsigned int inputDimension2 = 0;
  unsigned int numberOfComponents2 = 0;
  std::vector<unsigned int> imagesize2;
  itktools::GetImageProperties(
    inputFileNames[ 1 ],
    pixelType2AsString, componentType2AsString,
    inputDimension2, numberOfComponents2,
    imagesize2, dummy, dummy, dummy );
  itktools::ReplaceSpaceWithUnderscore( componentType2AsString );
  componentType2 = imageIOBaseTmp->GetComponentTypeFromString( componentType2AsString );

  // Properties of both images
  unsigned int inputDimension = 0;

  /** Check the input. */
  if ( pixelType1AsString != pixelType2AsString )
  {
    std::cerr << "ERROR: the two input images are of different pixel type (SCALAR, VECTOR, etc)."
      << "\n  Image " << inputFileNames[ 0 ] << " is of type " << pixelType1AsString
      << "\n  Image " << inputFileNames[ 1 ] << " is of type " << pixelType2AsString
      << std::endl;
    return 1;
  }

  if ( numberOfComponents1 != numberOfComponents2 )
  {
    std::cerr << "ERROR: the two input images have a different number of components."
      << "\n  Image " << inputFileNames[ 0 ] << " has " << numberOfComponents1
      << "\n  Image " << inputFileNames[ 1 ] << " has " << numberOfComponents2
      << std::endl;
    return 1;
  }

  if ( inputDimension1 != inputDimension2 )
  {
    std::cerr << "ERROR: the two input images are of different dimension."
      << "\n  Image " << inputFileNames[ 0 ] << " has dimension " << inputDimension1
      << "\n  Image " << inputFileNames[ 1 ] << " has dimension " << inputDimension2
      << std::endl;
    return 1;
  }
  else
  {
    inputDimension = inputDimension1;
  }

  for ( unsigned int i = 0; i < inputDimension; ++i )
  {
    if ( imagesize1[ i ] != imagesize2[ i ] )
    {
      std::cerr << "ERROR: the two input images have different sizes."
        << "\n  Image " << inputFileNames[ 0 ] << " has size [ ";
      for ( unsigned int j = 0; j < inputDimension; ++j )
      {
        std::cerr << imagesize1[ j ] << " ";
      }
      std::cerr << "]"
        << "\n  Image " << inputFileNames[ 1 ] << " has size [ ";
      for ( unsigned int j = 0; j < inputDimension; ++j )
      {
        std::cerr << imagesize2[ j ] << " ";
      }
      std::cerr << "]" << std::endl;
      return 1;
    }
  }

  /** The output type is the largest of the input types. The input types are
   * then set to long or double, depending on the output type.
   */
  componentTypeOut = itktools::GetLargestComponentType( componentType1, componentType2 );
  bool outIsInteger = itktools::ComponentTypeIsInteger( componentTypeOut );
  if ( outIsInteger )
  {
    componentType1 = componentType2 = itk::ImageIOBase::LONG;
  }
  else
  {
    componentType1 = componentType2 = itk::ImageIOBase::DOUBLE;
  }

  /** Return a value. */
  return 0;

} // end DetermineImageProperties()


/**
 * ******************* CheckOperator *******************
 */

int CheckOperator( std::string & operatoR )
{
  if ( operatoR == "ADDITION" || operatoR == "ADD" || operatoR == "PLUS" )
  {
    operatoR = "ADDITION";
    return 0;
  }
  else if ( operatoR == "WEIGHTEDADDITION" || operatoR == "WEIGHTEDADD"
    || operatoR == "WEIGHTEDPLUS" )
  {
    operatoR = "WEIGHTEDADDITION";
    return 0;
  }
  else if ( operatoR == "MINUS" || operatoR == "DIFF" )
  {
    operatoR = "MINUS";
    return 0;
  }
  else if ( operatoR == "TIMES" || operatoR == "MULTIPLY" )
  {
    operatoR = "TIMES";
    return 0;
  }
  else if ( operatoR == "DIVIDE" )
  {
    return 0;
  }
  else if ( operatoR == "POWER" )
  {
    return 0;
  }
  else if ( operatoR == "MAXIMUM" || operatoR == "MAX" )
  {
    operatoR = "MAXIMUM";
    return 0;
  }
  else if ( operatoR == "MINIMUM" || operatoR == "MIN" )
  {
    operatoR = "MINIMUM";
    return 0;
  }
  else if ( operatoR == "ABSOLUTEDIFFERENCE" || operatoR == "ABSDIFFERENCE"
    || operatoR == "ABSOLUTEDIFF" || operatoR == "ABSDIFF"
    || operatoR == "ABSOLUTEMINUS" || operatoR == "ABSMINUS" )
  {
    operatoR = "ABSOLUTEDIFFERENCE";
    return 0;
  }
  else if ( operatoR == "SQUAREDDIFFERENCE" || operatoR == "SQUAREDDIFF"
    || operatoR == "SQUAREDMINUS" )
  {
    operatoR = "SQUAREDDIFFERENCE";
    return 0;
  }
  else if ( operatoR == "BINARYMAGNITUDE" || operatoR == "BINARYMAG"
    || operatoR == "BINMAGNITUDE" || operatoR == "BINMAG"
    || operatoR == "MAGNITUDE" || operatoR == "MAG" )
  {
    operatoR = "BINARYMAGNITUDE";
    return 0;
  }
  else if ( operatoR == "MASK" )
  {
    operatoR = "MASK";
    return 0;
  }
  else if ( operatoR == "MASKNEGATED" || operatoR == "MASKNEG" )
  {
    operatoR = "MASKNEGATED";
    return 0;
  }
  else if ( operatoR == "MODULO" || operatoR == "MOD" )
  {
    operatoR = "MODULO";
    return 0;
  }
  else if ( operatoR == "LOG" || operatoR == "LOGN" )
  {
    operatoR = "LOG";
    return 0;
  }
  else
  {
    std::cerr << "ERROR: No operator \"" << operatoR << "\" defined!" << std::endl;
    return 1;
  }

  /** Return a value. */
  return 0;

} // end CheckOperator()


/**
 * ******************* OperatorNeedsArgument *******************
 */

bool OperatorNeedsArgument( const std::string & operatoR )
{
  /** A map to store if OperatorNeedsArgument. */
  std::map< std::string, bool > operatorMap;

  /** Fill the map. */
  operatorMap["ADDITION"]           = false;
  operatorMap["WEIGHTEDADDITION"]   = true;
  operatorMap["MINUS"]              = false;
  operatorMap["TIMES"]              = false;
  operatorMap["DIVIDE"]             = false;
  operatorMap["POWER"]              = false;
  operatorMap["MAXIMUM"]            = false;
  operatorMap["MINIMUM"]            = false;
  operatorMap["ABSOLUTEDIFFERENCE"] = false;
  operatorMap["SQUAREDDIFFERENCE"]  = false;
  operatorMap["BINARYMAGNITUDE"]    = false;
  operatorMap["MASK"]               = true;
  operatorMap["MASKNEGATED"]        = true;
  operatorMap["MODULO"]             = false;
  operatorMap["LOG"]                = false;

  /** Return true or false. */
  if ( operatorMap.count( operatoR ) ) return operatorMap[ operatoR ];
  return false;

} // end OperatorNeedsArgument()


/**
 * ******************* CreateOutputFileName *******************
 */

void CreateOutputFileName( const std::vector<std::string> & inputFileNames,
  std::string & outputFileName,
  const std::string & ops,
  const std::string & arg )
{
  /** Get parts of file name. */
  std::string path =
    itksys::SystemTools::GetFilenamePath( inputFileNames[ 0 ] );
  std::string name1 =
    itksys::SystemTools::GetFilenameName( inputFileNames[ 0 ] );
  std::string name2 =
    itksys::SystemTools::GetFilenameName( inputFileNames[ 1 ] );
  name1 = itksys::SystemTools::GetFilenameWithoutLastExtension( name1 );
  name2 = itksys::SystemTools::GetFilenameWithoutLastExtension( name2 );
  std::string ext =
    itksys::SystemTools::GetFilenameLastExtension( inputFileNames[ 0 ] );

  /** Compose outputFileName. */
  outputFileName = path + name1 + ops + name2 + ext;
/*
  if ( operatorMap[ops].first && !operatorMap[ops].second.first )
  {
    // Example: /path/SINimage.mhd *
    outputFileName = path + ops + name + ext;
  }
  else if ( operatorMap[ops].first && operatorMap[ops].second.first )
  {
    // Example: /path/3LPOWERimage.mhd *
    outputFileName = path + arg + ops + name + ext;
  }
  else
  {
    // Example: /path/imageRDIVIDE3.mhd *
    outputFileName = path + name + ops + arg + ext;
  }
*/

} // end CreateOutputFileName


/**
 * ******************* CheckOperatorAndArgument *******************
 */

bool CheckOperatorAndArgument( const std::string & operatoR,
  const std::string & argument, const bool & retarg )
{
  bool operatorNeedsArgument = OperatorNeedsArgument( operatoR );
  double arg = atof( argument.c_str() );

  /** If no argument is needed. */
  if ( !operatorNeedsArgument && retarg )
  {
    std::cerr << "WARNING: operator " << operatoR << " does not need an argument." << std::endl;
    std::cerr << "The argument (" << argument << ") is ignored." << std::endl;
    return true;
  }

  /** If an argument is needed, but not provided. */
  if ( operatorNeedsArgument && !retarg )
  {
    std::cerr << "ERROR: operator " << operatoR << " needs an argument." << std::endl;
    std::cerr << "Specify the argument with \"-arg\"." << std::endl;
    return false;
  }

  /** If an argument is needed and provided. */
  if ( operatorNeedsArgument )
  {
    if ( operatoR == "WEIGHTEDADDITION" )
    {
      if ( arg < 0.0 || arg > 1.0 )
      {
        std::cerr << "ERROR: the weight should be between 0.0 and 1.0." << std::endl;
        return false;
      }
    }
  }

  return true;

} // end CheckOperatorAndArgument()


#endif //#ifndef __BinaryImageOperatorMainHelper_h
