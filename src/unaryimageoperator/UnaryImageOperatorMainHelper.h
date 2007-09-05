#ifndef __UnaryImageOperatorMainHelper_h
#define __UnaryImageOperatorMainHelper_h

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "Unary operations on one image." << std::endl;
	std::cout << "Usage:" << std::endl << "pxunaryimageoperator" << std::endl;
	std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  -ops     UnaryOperator of the following form:\n"
            << "           {+,-,*,/,^,%}\n"
            << "           notation:\n"
            << "             {PLUS,{R,L}MINUS,TIMES,{R,L}DIVIDE,{R,L}POWER,{R,L}MOD,\n"
            << "             NEG,SIGN,ABS,FLOOR,CEIL,ROUND,\n"
            << "             LN,10LOG,NLOG,EXP,[ARC]SIN,[ARC]COS,[ARC]TAN}\n"
            << "           notation examples:\n"
            << "             RMINUS = A - arg\n"
            << "             LMINUS = arg - A\n"
            << "             SIN = sin(A)\n"
            << "             RPOWER = A ^ arg\n";
  std::cout << "  [-arg]   argument, necessary for some ops" << std::endl;
	std::cout << "  [-out]   outputFilename, default in + <ops> + <arg> + .mhd" << std::endl;
  std::cout << "  [-pto]   outputPixelType, default: same as inputimage" << std::endl;
	std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, float." << std::endl;
} // end PrintHelp


  /**
	 * ******************* InputIsInteger *******************
	 */
void InputIsInteger( const std::string & inputComponentType,
  bool & inputIsInteger )
{
  /** Check if the input image is of integer type. */
  inputIsInteger = false;
  if ( inputComponentType == "unsigned_char" || inputComponentType == "char"
    || inputComponentType == "unsigned_short" || inputComponentType == "short"
    || inputComponentType == "unsigned_int" || inputComponentType == "int"
    || inputComponentType == "unsigned_long" || inputComponentType == "long" )
  {
    inputIsInteger = true;
  }
} // end InputIsInteger


  /**
	 * ******************* ArgumentIsInteger *******************
	 */
void ArgumentIsInteger( const std::string & argument, bool & argumentIsInteger )
{
  /** Check if the argument is of integer type. */
  argumentIsInteger = false;
  std::basic_string<char>::size_type pos = argument.find( "." );
	const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
	if ( pos == npos )
  {
    argumentIsInteger = true;
  }

} // end ArgumentIsInteger


  /**
	 * ******************* CheckOps *******************
	 */
int CheckOps( std::string & ops, bool isInteger )
{
  /** A map to store if there are integer and double versions
   * of the functor. */
  std::map< std::string, bool > operatorMap;

  /** Fill the map. */
  operatorMap["PLUS"]     = false;
  operatorMap["RMINUS"]   = false;
  operatorMap["LMINUS"]   = false;
  operatorMap["TIMES"]    = false;
  operatorMap["RDIVIDE"]  = false;
  operatorMap["LDIVIDE"]  = false;
  operatorMap["RPOWER"]   = false;
  operatorMap["LPOWER"]   = false;
  operatorMap["RMOD"]     = true;
  operatorMap["LMOD"]     = true;
  operatorMap["NEG"]      = false;
  operatorMap["SIGN"]     = true;
  operatorMap["ABS"]      = true;
  operatorMap["FLOOR"]    = false;
  operatorMap["CEIL"]     = false;
  operatorMap["ROUND"]    = false;
  operatorMap["LN"]       = false;
  operatorMap["LOG10"]    = false;
  operatorMap["NLOG"]     = false;
  operatorMap["EXP"]      = false;
  operatorMap["SIN"]      = false;
  operatorMap["COS"]      = false;
  operatorMap["TAN"]      = false;
  operatorMap["ARCSIN"]   = false;
  operatorMap["ARCCOS"]   = false;
  operatorMap["ARCTAN"]   = false;

  operatorMap["EQUAL"]    = false;

  /** Append with INT or DOUBLE if necessary. */
  if ( operatorMap.count( ops ) ) // the operator exists
  {
    if ( operatorMap[ops] ) // it has an int and a double version
    {
      if ( isInteger ) ops += "INT"; // it is int
      else ops += "DOUBLE";          // it is double
    }
  }
  else
  {
    std::cerr << "ERROR: No such operator." << std::endl;
    return 1;
  }

  /** Return a value. */
  return 0;
  
} // end CheckOps


  /**
	 * ******************* OperatorNeedsArgument *******************
	 */
bool OperatorNeedsArgument( std::string & ops )
{
  /** A map to store if OperatorNeedsArgument. */
  std::map< std::string, bool > operatorMap;

  /** Fill the map. */
  operatorMap["PLUS"]    = true;
  operatorMap["RMINUS"]  = true;
  operatorMap["LMINUS"]  = true;
  operatorMap["TIMES"]   = true;
  operatorMap["RDIVIDE"] = true;
  operatorMap["LDIVIDE"] = true;
  operatorMap["RPOWER"]  = true;
  operatorMap["LPOWER"]  = true;
  operatorMap["RMOD"]    = true;
  operatorMap["LMOD"]    = true;
  operatorMap["NEG"]     = false;
  operatorMap["SIGN"]    = false;
  operatorMap["ABS"]     = false;
  operatorMap["FLOOR"]   = false;
  operatorMap["CEIL"]    = false;
  operatorMap["ROUND"]   = false;
  operatorMap["LN"]      = false;
  operatorMap["LOG10"]   = false;
  operatorMap["NLOG"]    = true;
  operatorMap["EXP"]     = false;
  operatorMap["SIN"]     = false;
  operatorMap["COS"]     = false;
  operatorMap["TAN"]     = false;
  operatorMap["ARCSIN"]  = false;
  operatorMap["ARCCOS"]  = false;
  operatorMap["ARCTAN"]  = false;

  operatorMap["EQUAL"]   = true;

  return operatorMap[ ops ];

} // end OperatorNeedsArgument

  
  /**
	 * ******************* CreateOutputFileName *******************
	 */
void CreateOutputFileName( const std::string & inputFileName, std::string & outputFileName,
  const std::string & ops, const std::string & arg )
{
  /** A map to store if ops is before, if arg is needed, and if arg is before. */
  typedef std::pair< bool, bool >       PairType;
  typedef std::pair< bool, PairType >   PairPairType;
  std::map< std::string, PairPairType > operatorMap;

  /** Fill the map. */
  operatorMap["PLUS"]   = PairPairType( false, PairType( true, false ) );
  operatorMap["RMINUS"] = PairPairType( false, PairType( true, false ) );
  operatorMap["LMINUS"] = PairPairType( true, PairType( true, true ) );
  operatorMap["TIMES"]  = PairPairType( false, PairType( true, false ) );
  operatorMap["RDIVIDE"] = PairPairType( false, PairType( true, false ) );
  operatorMap["LDIVIDE"] = PairPairType( true, PairType( true, true ) );
  operatorMap["RPOWER"] = PairPairType( false, PairType( true, false ) );
  operatorMap["LPOWER"] = PairPairType( true, PairType( true, true ) );
  operatorMap["RMOD"]   = PairPairType( false, PairType( true, false ) );
  operatorMap["LMOD"]   = PairPairType( true, PairType( true, true ) );
  operatorMap["NEG"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["SIGN"]   = PairPairType( true, PairType( false, true ) );
  operatorMap["ABS"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["FLOOR"]  = PairPairType( true, PairType( false, true ) );
  operatorMap["CEIL"]   = PairPairType( true, PairType( false, true ) );
  operatorMap["ROUND"]  = PairPairType( true, PairType( false, true ) );
  operatorMap["LN"]     = PairPairType( true, PairType( false, true ) );
  operatorMap["LOG10"]  = PairPairType( true, PairType( false, true ) );
  operatorMap["NLOG"]   = PairPairType( true, PairType( false, true ) );
  operatorMap["EXP"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["SIN"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["COS"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["TAN"]    = PairPairType( true, PairType( false, true ) );
  operatorMap["ARCSIN"] = PairPairType( true, PairType( false, true ) );
  operatorMap["ARCCOS"] = PairPairType( true, PairType( false, true ) );
  operatorMap["ARCTAN"] = PairPairType( true, PairType( false, true ) );

  operatorMap["EQUAL"]  = PairPairType( false, PairType( true, false ) );

	/** Get parts of file name. */
  std::string path =
    itksys::SystemTools::GetFilenamePath( inputFileName );
  std::string name =
    itksys::SystemTools::GetFilenameName( inputFileName );
  name = itksys::SystemTools::GetFilenameWithoutLastExtension( name );
  std::string ext =
    itksys::SystemTools::GetFilenameLastExtension( inputFileName );

  /** Compose outputFileName. */
  if ( operatorMap[ops].first && !operatorMap[ops].second.first )
  {
    /** Example: /path/SINimage.mhd */
    outputFileName = path + ops + name + ext;
  }
  else if ( operatorMap[ops].first && operatorMap[ops].second.first )
  {
    /** Example: /path/3LPOWERimage.mhd */
    outputFileName = path + arg + ops + name + ext;
  }
  else
  {
    /** Example: /path/imageRDIVIDE3.mhd */
    outputFileName = path + name + ops + arg + ext;
  }
	
} // end CreateOutputFileName

#endif //#ifndef __UnaryImageOperatorMainHelper_h
