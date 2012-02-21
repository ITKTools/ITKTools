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
#ifndef __UnaryImageOperatorMainHelper_h_
#define __UnaryImageOperatorMainHelper_h_

#include <map>
#include <utility> // for pair
#include <string>
#include <itksys/SystemTools.hxx>


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
  operatorMap["LINEAR"]   = false;

  /** Append with INT or DOUBLE if necessary. */
  if( operatorMap.count( ops ) ) // the operator exists
  {
    if( operatorMap[ops] ) // it has an int and a double version
    {
      if( isInteger ) ops += "INT"; // it is int
      else ops += "DOUBLE";          // it is double
    }
  }
  else
  {
    std::cerr << "ERROR: No operator \"" << ops << "\" defined!" << std::endl;
    return 1;
  }

  /** Return a value. */
  return EXIT_SUCCESS;

} // end CheckOps()


/**
 * ******************* OperatorNeedsArgument *******************
 */

bool OperatorNeedsArgument( const std::string & ops )
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
  operatorMap["LINEAR"]  = true;

  return operatorMap[ ops ];

} // end OperatorNeedsArgument()


/**
 * ******************* CreateOutputFileName *******************
 */

void CreateOutputFileName( const std::string & inputFileName,
  std::string & outputFileName,
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
  operatorMap["LINEAR"] = PairPairType( false, PairType( true, false ) );

  /** Get parts of file name. */
  std::string path =
    itksys::SystemTools::GetFilenamePath( inputFileName );
  std::string name =
    itksys::SystemTools::GetFilenameName( inputFileName );
  name = itksys::SystemTools::GetFilenameWithoutLastExtension( name );
  std::string ext =
    itksys::SystemTools::GetFilenameLastExtension( inputFileName );

  /** Compose outputFileName. */
  if( operatorMap[ops].first && !operatorMap[ops].second.first )
  {
    /** Example: /path/SINimage.mhd */
    outputFileName = path + ops + name + ext;
  }
  else if( operatorMap[ops].first && operatorMap[ops].second.first )
  {
    /** Example: /path/3LPOWERimage.mhd */
    outputFileName = path + arg + ops + name + ext;
  }
  else
  {
    /** Example: /path/imageRDIVIDE3.mhd */
    outputFileName = path + name + ops + arg + ext;
  }

} // end CreateOutputFileName()


#endif //#ifndef __UnaryImageOperatorMainHelper_h
