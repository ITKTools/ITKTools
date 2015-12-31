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
#ifndef __NaryImageOperatorMainHelper_h_
#define __NaryImageOperatorMainHelper_h_

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>
#include "ITKToolsImageProperties.h"
#include "ITKToolsHelpers.h"

/**
 * ******************* DetermineImageProperties *******************
 */

int DetermineImageProperties(
  const std::vector<std::string> & inputFileNames,
  itk::ImageIOBase::IOComponentType & componentTypeIn,
  itk::ImageIOBase::IOComponentType & componentTypeOut,
  unsigned int & inputDimension )
{
  /** Determine image properties of image 0. */
  itk::ImageIOBase::IOPixelType inputPixelType0;
  unsigned int inputDimension0 = 2;
  unsigned int numberOfComponents0 = 1;
  std::vector<unsigned int> imagesize0( inputDimension0, 0 );
  bool retgip0 = itktools::GetImageProperties(
    inputFileNames[ 0 ],
    inputPixelType0,
    componentTypeIn,
    inputDimension0,
    numberOfComponents0,
    imagesize0 );
  /** If false, it means there was an error. Return 1. 
   * \todo: the various GetImageProperties functions in ITKToolsImageProperties
   * return either bool or int, which is very confusing. Harmonize.
   */
  if( !retgip0 ) return 1;

  /** Determine image properties of other images. */
  itk::ImageIOBase::IOPixelType inputPixelType_i;
  itk::ImageIOBase::IOComponentType componentTypeIn_i = componentTypeIn;
  unsigned int inputDimension_i = 2;
  unsigned int numberOfComponents_i = 1;
  std::vector<unsigned int> imagesize_i;
  for( unsigned int i = 1; i < inputFileNames.size(); i++ )
  {
    bool retgip_i = itktools::GetImageProperties(
      inputFileNames[ i ],
      inputPixelType_i,
      componentTypeIn_i,
      inputDimension_i,
      numberOfComponents_i,
      imagesize_i );
	/** If false, it means there was an error. Return 1. */
    if( !retgip_i ) return 1;

    /** Check the input. */
    if( inputPixelType0 != inputPixelType_i )
    {
      std::cerr << "ERROR: the input images are not of equal pixel type (SCALAR, VECTOR, etc)." << std::endl;
      return EXIT_FAILURE;
    }

    if( numberOfComponents0 != numberOfComponents_i )
    {
      std::cerr << "ERROR: the input images have a different number of components." << std::endl;
      return EXIT_FAILURE;
    }

    if( inputDimension0 != inputDimension_i )
    {
      std::cerr << "ERROR: the input images are of different dimension." << std::endl;
      return EXIT_FAILURE;
    }
    else
    {
      inputDimension = inputDimension0;
    }

    if( imagesize0 != imagesize_i )
    {
      std::cerr << "ERROR: the input images have different sizes." << std::endl;
      return EXIT_FAILURE;
    }

    /** The output type is the largest of the input types. */
    componentTypeOut = itktools::GetLargestComponentType( componentTypeOut, componentTypeIn_i );
  }

  /** The input type is set to long or double, depending on the output type. */
  bool outIsInteger = itktools::ComponentTypeIsInteger( componentTypeOut );
  if( outIsInteger )
  {
    componentTypeIn = itk::ImageIOBase::LONG;
  }
  else
  {
    componentTypeIn = itk::ImageIOBase::DOUBLE;
  }

  /** Return a value indicating success. */
  return 0;

} // end DetermineImageProperties()


/**
 * ******************* CheckOperator *******************
 */

int CheckOperator( std::string & operatoR )
{
  if( operatoR == "ADDITION" || operatoR == "ADD" || operatoR == "PLUS" )
  {
    operatoR = "ADDITION";
    return 0;
  }
  else if( operatoR == "MEAN" || operatoR == "AVERAGE" )
  {
    operatoR = "MEAN";
    return 0;
  }
  else if( operatoR == "WEIGHTEDADDITION" || operatoR == "WEIGHTEDADD"
    || operatoR == "WEIGHTEDPLUS" )
  {
    operatoR = "WEIGHTEDADDITION";
    return 0;
  }
  else if( operatoR == "MINUS" || operatoR == "DIFF" )
  {
    operatoR = "MINUS";
    return 0;
  }
  else if( operatoR == "TIMES" || operatoR == "MULTIPLY" )
  {
    operatoR = "TIMES";
    return 0;
  }
  else if( operatoR == "DIVIDE" )
  {
    return 0;
  }
  else if( operatoR == "POWER" )
  {
    return 0;
  }
  else if( operatoR == "MAXIMUM" || operatoR == "MAX" )
  {
    operatoR = "MAXIMUM";
    return 0;
  }
  else if( operatoR == "MINIMUM" || operatoR == "MIN" )
  {
    operatoR = "MINIMUM";
    return 0;
  }
  else if( operatoR == "ABSOLUTEDIFFERENCE" || operatoR == "ABSDIFFERENCE"
    || operatoR == "ABSOLUTEDIFF" || operatoR == "ABSDIFF"
    || operatoR == "ABSOLUTEMINUS" || operatoR == "ABSMINUS" )
  {
    operatoR = "ABSOLUTEDIFFERENCE";
    return 0;
  }
  else if( operatoR == "SQUAREDDIFFERENCE" || operatoR == "SQUAREDDIFF"
    || operatoR == "SQUAREDMINUS" )
  {
    operatoR = "SQUAREDDIFFERENCE";
    return 0;
  }
  else if( operatoR == "BINARYMAGNITUDE" || operatoR == "BINARYMAG"
    || operatoR == "BINMAGNITUDE" || operatoR == "BINMAG"
    || operatoR == "MAGNITUDE" || operatoR == "MAG" )
  {
    operatoR = "BINARYMAGNITUDE";
    return 0;
  }
  else if( operatoR == "MASK" )
  {
    operatoR = "MASK";
    return 0;
  }
  else if( operatoR == "MASKNEGATED" || operatoR == "MASKNEG" )
  {
    operatoR = "MASKNEGATED";
    return 0;
  }
  else if( operatoR == "MODULO" || operatoR == "MOD" )
  {
    operatoR = "MODULO";
    return 0;
  }
  else if( operatoR == "LOG" || operatoR == "LOGN" )
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
  operatorMap["MEAN"]               = false;
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
  if( operatorMap.count( operatoR ) ) return operatorMap[ operatoR ];
  return false;

} // end OperatorNeedsArgument()


/**
 * ******************* CheckOperatorAndArgument *******************
 */

bool CheckOperatorAndArgument(
  const std::string & operatoR,
  const std::string & argument,
  const bool & retarg )
{
  bool operatorNeedsArgument = OperatorNeedsArgument( operatoR );
  double arg = atof( argument.c_str() );

  /** If no argument is needed. */
  if( !operatorNeedsArgument && retarg )
  {
    std::cerr << "WARNING: operator " << operatoR << " does not need an argument." << std::endl;
    std::cerr << "The argument (" << argument << ") is ignored." << std::endl;
    return true;
  }

  /** If an argument is needed, but not provided. */
  if( operatorNeedsArgument && !retarg )
  {
    std::cerr << "ERROR: operator " << operatoR << " needs an argument." << std::endl;
    std::cerr << "Specify the argument with \"-arg\"." << std::endl;
    return false;
  }

  /** If an argument is needed and provided. */
  if( operatorNeedsArgument )
  {
    if( operatoR == "WEIGHTEDADDITION" )
    {
      if( arg < 0.0 || arg > 1.0 )
      {
        std::cerr << "ERROR: the weight should be between 0.0 and 1.0." << std::endl;
        return false;
      }
    }
  }

  return true;

} // end CheckOperatorAndArgument()


#endif //#ifndef __NaryImageOperatorMainHelper_h_
