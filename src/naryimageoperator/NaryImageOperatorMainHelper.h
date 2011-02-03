#ifndef __NaryImageOperatorMainHelper_h
#define __NaryImageOperatorMainHelper_h

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>
#include "CommandLineArgumentHelper.h"

/**
 * ******************* Macro *******************
 */

/** run: A macro to call a function. */
#define run( function, typeIn, typeOut, dim ) \
if ( ComponentTypeIn == #typeIn && ComponentTypeOut == #typeOut \
  && inputDimension == dim ) \
{ \
  typedef itk::Image< typeIn, dim >   InputImageType; \
  typedef itk::Image< typeOut, dim >  OutputImageType; \
  function< InputImageType, OutputImageType >( \
    inputFileNames, outputFileName, ops, useCompression, numberOfStreams, argument ); \
  supported = true; \
}

/**
 * ******************* PrintHelp *******************
 */

void PrintHelp( void )
{
  std::cout << "Performs n-ary operations on multiple (n) images." << std::endl;
  std::cout << "Usage:\npxnaryimageoperator" << std::endl;
  std::cout << "  -in      inputFilenames, at least 2" << std::endl;
  std::cout << "  -out     outputFilename" << std::endl;
  std::cout << "  -ops     n-ary operator of the following form:\n"
            << "           {+,-,*,/,^,%}\n"
            << "           notation:\n"
            << "             {ADDITION, MINUS, TIMES, DIVIDE,\n"
            << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE,\n"
            << "             NARYMAGNITUDE }\n"
            << "           notation examples:\n"
            << "             MINUS = I_0 - I_1 - ... - I_n \n"
            << "             ABSDIFF = |I_0 - I_1 - ... - I_n|\n"
            << "             MIN = min( I_0, ..., I_n )\n"
            << "             MAGNITUDE = sqrt( I_0 * I_0 + ... + I_n * I_n )\n";
//   std::cout << "  [-arg]   argument, necessary for some ops\n"
//             << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
//             << "             MASK[NEG]: background value, e.g. 0." << std::endl;
  std::cout << "  [-z]     compression flag; if provided, the output image is compressed\n";
  std::cout << "  [-s]     number of streams, default equals number of inputs.\n";
  std::cout << "  [-opct]  output component type, by default the largest of the two input images\n"
            << "             choose one of: {[unsigned_]{char,short,int,long},float,double}" << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;

} // end PrintHelp()


/**
 * ******************* TypeIsInteger *******************
 */

bool TypeIsInteger( const std::string & componentType )
{
  /** Make sure the input has "_" instead of " ". */
  std::string compType = componentType;
  ReplaceSpaceWithUnderscore( compType );

  /** Check if the input image is of integer type. */
  bool typeIsInteger = false;
  if ( compType == "unsigned_char" || compType == "char"
    || compType == "unsigned_short" || compType == "short"
    || compType == "unsigned_int" || compType == "int"
    || compType == "unsigned_long" || compType == "long" )
  {
    typeIsInteger = true;
  }

  return typeIsInteger;

} // end TypeIsInteger()


/**
 * ******************* DetermineImageProperties *******************
 */

int DetermineImageProperties(
  const std::vector<std::string> & inputFileNames,
  std::string & ComponentTypeIn,
  std::string & ComponentTypeOut,
  unsigned int & inputDimension )
{
  /** Determine image properties of image 1. */
  std::string inputPixelType1 = "";
  unsigned int inputDimension1 = 2;
  unsigned int numberOfComponents1 = 1;
  std::vector<unsigned int> imagesize1( inputDimension1, 0 );
  int retgip1 = GetImageProperties(
    inputFileNames[ 0 ],
    inputPixelType1,
    ComponentTypeIn,
    inputDimension1,
    numberOfComponents1,
    imagesize1 );
  if ( retgip1 ) return retgip1;

  /** Determine image properties of other images. */
  std::string inputPixelType_i = "";
  std::string ComponentTypeIn_i = ComponentTypeIn;
  unsigned int inputDimension_i = 2;
  unsigned int numberOfComponents_i = 1;
  std::vector<unsigned int> imagesize_i( inputDimension1, 0 );
  for ( unsigned int i = 1; i < inputFileNames.size(); i++ )
  {
    int retgip_i = GetImageProperties(
      inputFileNames[ 1 ],
      inputPixelType_i,
      ComponentTypeIn_i,
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
    ComponentTypeOut = GetLargestComponentType( ComponentTypeOut, ComponentTypeIn_i );
  }

  /** The input type is set to long or double, depending on the output type. */
  bool outIsInteger = TypeIsInteger( ComponentTypeOut );
  if ( outIsInteger ) ComponentTypeIn = "long";
  else ComponentTypeIn = "double";

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
