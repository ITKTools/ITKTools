#ifndef __BinaryImageOperatorMainHelper_h
#define __BinaryImageOperatorMainHelper_h

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>
#include "CommandLineArgumentHelper.h"

  /**
   * ******************* Macro *******************
   */

/** run: A macro to call a function. */
#define run( function, typeIn1, typeIn2, typeOut, dim ) \
if ( ComponentTypeIn1 == #typeIn1 && ComponentTypeIn2 == #typeIn2 \
  && ComponentTypeOut == #typeOut && inputDimension == dim ) \
{ \
  typedef itk::Image< typeIn1, dim > InputImage1Type; \
  typedef itk::Image< typeIn2, dim > InputImage2Type; \
  typedef itk::Image< typeOut, dim > OutputImageType; \
  function< InputImage1Type, InputImage2Type, OutputImageType >( \
    inputFileNames[ 0 ], inputFileNames[ 1 ], outputFileName, ops, argument ); \
  supported = true; \
}

  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp()
{
  std::cout << "Performs binary operations on two images." << std::endl;
  std::cout << "Usage:\npxbinaryimageoperator" << std::endl;
  std::cout << "  -in      inputFilenames" << std::endl;
  std::cout << "  [-out]   outputFilename, default in1 + ops + arg + in2 + .mhd" << std::endl;
  std::cout << "  -ops     binary operator of the following form:\n"
            << "           {+,-,*,/,^,%}\n"
            << "           notation:\n"
            << "             {ADDITION, WEIGHTEDADDITION, MINUS, TIMES,DIVIDE,POWER,\n"
            << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE, SQUAREDDIFFERENCE,\n"
            << "             BINARYMAGNITUDE, MASK, MASKNEGATED, LOG}\n"
            << "           notation examples:\n"
            << "             MINUS = A - B\n"
            << "             ABSDIFF = |A - B|\n"
            << "             MIN = min( A, B )\n"
            << "             MAGNITUDE = sqrt( A * A + B * B )\n";
  std::cout << "  [-arg]   argument, necessary for some ops\n"
            << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
            << "             MASK[NEG]: background value, e.g. 0." << std::endl;
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
  std::string & ComponentTypeIn1,
  std::string & ComponentTypeIn2,
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
    ComponentTypeIn1,
    inputDimension1,
    numberOfComponents1,
    imagesize1 );
  if ( retgip1 ) return retgip1;

  /** Determine image properties of image 2. */
  std::string inputPixelType2 = "";
  unsigned int inputDimension2 = 2;
  unsigned int numberOfComponents2 = 1;
  std::vector<unsigned int> imagesize2( inputDimension1, 0 );
  int retgip2 = GetImageProperties(
    inputFileNames[ 1 ],
    inputPixelType2,
    ComponentTypeIn2,
    inputDimension2,
    numberOfComponents2,
    imagesize2 );
  if ( retgip2 ) return retgip2;

  /** Check the input. */
  if ( inputPixelType1 != inputPixelType2 )
  {
    std::cerr << "ERROR: the two input images are of different pixel type (SCALAR, VECTOR, etc)." << std::endl;
    return 1;
  }

  if ( numberOfComponents1 != numberOfComponents2 )
  {
    std::cerr << "ERROR: the two input images have a different number of components." << std::endl;
    return 1;
  }

  if ( inputDimension1 != inputDimension2 )
  {
    std::cerr << "ERROR: the two input images are of different dimension." << std::endl;
    return 1;
  }
  else
  {
    inputDimension = inputDimension1;
  }

  if ( imagesize1 != imagesize2 )
  {
    std::cerr << "ERROR: the two input images have different sizes." << std::endl;
    return 1;
  }

  /** The output type is the largest of the input types. The input types are
   * then set to long or double, depending on the output type.
   */
  ComponentTypeOut = GetLargestComponentType( ComponentTypeIn1, ComponentTypeIn2 );
  bool outIsInteger = TypeIsInteger( ComponentTypeOut );
  if ( outIsInteger ) ComponentTypeIn1 = ComponentTypeIn2 = "long";
  else ComponentTypeIn1 = ComponentTypeIn2 = "double";

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
    std::cerr << "ERROR: No such operator." << std::endl;
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
    /** Example: /path/SINimage.mhd *
    outputFileName = path + ops + name + ext;
  }
  else if ( operatorMap[ops].first && operatorMap[ops].second.first )
  {
    /** Example: /path/3LPOWERimage.mhd *
    outputFileName = path + arg + ops + name + ext;
  }
  else
  {
    /** Example: /path/imageRDIVIDE3.mhd *
    outputFileName = path + name + ops + arg + ext;
  }*/
  
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
