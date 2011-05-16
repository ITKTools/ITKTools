#ifndef __NaryImageOperatorMainHelper_h
#define __NaryImageOperatorMainHelper_h

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
  itktools::ComponentType & componentTypeIn,
  itktools::ComponentType & componentTypeOut,
  unsigned int & inputDimension )
{
  /** Determine image properties of image 1. */
  itk::ImageIOBase::IOPixelType inputPixelType1;
  unsigned int inputDimension1 = 2;
  unsigned int numberOfComponents1 = 1;
  std::vector<unsigned int> imagesize1( inputDimension1, 0 );
  int retgip1 = itktools::GetImageProperties(
    inputFileNames[ 0 ],
    inputPixelType1,
    componentTypeIn,
    inputDimension1,
    numberOfComponents1,
    imagesize1 );
  if ( retgip1 ) return retgip1;

  /** Determine image properties of other images. */
  itk::ImageIOBase::IOPixelType inputPixelType_i;
  itktools::ComponentType componentTypeIn_i = componentTypeIn;
  unsigned int inputDimension_i = 2;
  unsigned int numberOfComponents_i = 1;
  std::vector<unsigned int> imagesize_i;
  for ( unsigned int i = 1; i < inputFileNames.size(); i++ )
  {
    int retgip_i = itktools::GetImageProperties(
      inputFileNames[ 1 ],
      inputPixelType_i,
      componentTypeIn_i,
      inputDimension_i,
      numberOfComponents_i,
      imagesize_i );
    if ( retgip_i ) return retgip_i;

    /** Check the input. */
    if ( inputPixelType1 != inputPixelType_i )
    {
      std::cerr << "ERROR: the input images are not of equal pixel type (SCALAR, VECTOR, etc)." << std::endl;
      return 1;
    }

    if ( numberOfComponents1 != numberOfComponents_i )
    {
      std::cerr << "ERROR: the input images have a different number of components." << std::endl;
      return 1;
    }

    if ( inputDimension1 != inputDimension_i )
    {
      std::cerr << "ERROR: the input images are of different dimension." << std::endl;
      return 1;
    }
    else
    {
      inputDimension = inputDimension1;
    }

    if ( imagesize1 != imagesize_i )
    {
      std::cerr << "ERROR: the input images have different sizes." << std::endl;
      return 1;
    }

    /** The output type is the largest of the input types. */
    componentTypeOut = itktools::GetLargestComponentType( componentTypeOut, componentTypeIn_i );
  }

  /** The input type is set to long or double, depending on the output type. */
  bool outIsInteger = itktools::ComponentTypeIsInteger( componentTypeOut );
  if ( outIsInteger )
  {
    componentTypeIn = itk::ImageIOBase::LONG;
  }
  else
  {
    componentTypeIn = itk::ImageIOBase::DOUBLE;
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
  else if ( operatoR == "MEAN" || operatoR == "AVERAGE" )
  {
    operatoR = "MEAN";
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
  if ( operatorMap.count( operatoR ) ) return operatorMap[ operatoR ];
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


#endif //#ifndef __NaryImageOperatorMainHelper_h
