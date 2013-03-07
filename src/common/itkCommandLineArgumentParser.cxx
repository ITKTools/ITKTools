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
#ifndef __itkCommandLineArgumentParser_cxx_
#define __itkCommandLineArgumentParser_cxx_

#include "itkCommandLineArgumentParser.h"

#include <limits>

namespace itk
{

/**
 * ******************* Constructor *******************
 */

CommandLineArgumentParser
::CommandLineArgumentParser()
{
  this->m_Argv.clear();
  this->m_ArgumentMap.clear();
  this->m_ProgramHelpText = "No help text provided.";

} // end Constructor


/**
 * ******************* SetCommandLineArguments *******************
 */

void
CommandLineArgumentParser
::SetCommandLineArguments( int argc, char **argv )
{
  this->m_Argv.resize( argc );
  for ( IndexType i = 0; i < static_cast<IndexType>( argc ); i++ )
  {
    this->m_Argv[ i ] = argv [ i ];
  }
  this->CreateArgumentMap();

} // end SetCommandLineArguments()


/**
 * ******************* CreateArgumentMap *******************
 */

void
CommandLineArgumentParser
::CreateArgumentMap( void )
{
  for ( IndexType i = 1; i < this->m_Argv.size(); ++i )
  {
    if( this->m_Argv[ i ].substr( 0, 1 ) == "-" )
    {
      /** All key entries are removed, the latest is stored. */
      this->m_ArgumentMap.erase( this->m_Argv[ i ] );
      this->m_ArgumentMap.insert( EntryType( this->m_Argv[ i ], i ) );
    }
  }
} // end CreateArgumentMap()


/**
 * ******************* ArgumentExists *******************
 */

bool
CommandLineArgumentParser
::ArgumentExists( const std::string & key ) const
{
  if( this->m_ArgumentMap.count( key ) == 0 )
  {
    return false;
  }
  return true;

} // end ArgumentExists()


/**
 * ******************* PrintAllArguments *******************
 */

void
CommandLineArgumentParser
::PrintAllArguments() const
{
  ArgumentMapType::const_iterator it = this->m_ArgumentMap.begin();

  for( ; it != this->m_ArgumentMap.end(); ++it )
  {
   std::cout << it->first << std::endl;
  }

} // end PrintAllArguments()


/**
 * ******************* ExactlyOneExists *******************
 */

bool
CommandLineArgumentParser
::ExactlyOneExists( const std::vector<std::string> & keys ) const
{
  unsigned int counter = 0;
  for( unsigned int i = 0; i < keys.size(); i++ )
  {
    if( this->ArgumentExists( keys[ i ] ) )
    {
      counter++;
    }
  }

  if( counter == 1 ) return true;
  else               return false;

} // end ExactlyOneExists()


/**
 * ******************* FindKey *******************
 */

bool
CommandLineArgumentParser
::FindKey( const std::string & key,
  IndexType & keyIndex, IndexType & nextKeyIndex ) const
{
  /** Loop once over the arguments, to get the index of the key,
   * and that of the next key.
   */
  bool keyFound = false;
  keyIndex = 0;
  nextKeyIndex = this->m_Argv.size();
  for ( IndexType i = 0; i < this->m_Argv.size(); i++ )
  {
    if( !keyFound && this->m_Argv[ i ] == key )
    {
      keyFound = true;
      keyIndex = i;
      continue;
    }
    if( keyFound && this->m_Argv[ i ].substr(0,1) == "-" )
    {
      if( !this->IsANumber( this->m_Argv[ i ] ) )
      {
        nextKeyIndex = i;
        break;
      }
    }
  } // end for loop

  /** Check if the key was found and if the next argument is not also a key. */
  if( !keyFound ) return false;
  if( nextKeyIndex - keyIndex == 1 ) return false;

  return true;

} // end FindKey()


/**
 * ******************* IsANumber *******************
 *
 * Checks if something starting with a "-" is a key or a negative number.
 */

bool CommandLineArgumentParser::
IsANumber( const std::string & arg ) const
{
  std::string number = "0123456789";
  static const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if( arg.size() > 1 )
  {
    if( npos != number.find( arg.substr( 1, 1 ) ) )
    {
      return true;
    }
  }

  return false;

} // end IsANumber()


/**
 * **************** StringCast ***************
 */

bool
CommandLineArgumentParser
::StringCast( const std::string & parameterValue, std::string & casted ) const
{
  casted = parameterValue;
  return true;

} // end StringCast()


/**
 * **************** MarkArgumentAsRequired ***************
 */

void
CommandLineArgumentParser
::MarkArgumentAsRequired(
  const std::string & argument, const std::string & helpText )
{
  std::pair<std::string, std::string> requiredArgument;
  requiredArgument.first = argument;
  requiredArgument.second = helpText;
  this->m_RequiredArguments.push_back( requiredArgument );

} // end MarkArgumentAsRequired()


/**
 * ******************* MarkExactlyOneOfArgumentsAsRequired *******************
 */

void
CommandLineArgumentParser
::MarkExactlyOneOfArgumentsAsRequired(
  const std::vector<std::string> & arguments, const std::string & helpText )
{
  std::pair< std::vector<std::string>, std::string> requiredArguments;
  requiredArguments.first = arguments;
  requiredArguments.second = helpText;
  this->m_RequiredExactlyOneArguments.push_back( requiredArguments );

} // end MarkExactlyOneOfArgumentsAsRequired()


/**
 * **************** CheckForRequiredArguments ***************
 */

CommandLineArgumentParser::ReturnValue
CommandLineArgumentParser
::CheckForRequiredArguments() const
{
  /** If no arguments were specified at all, display the help text. */
  if( this->m_Argv.size() == 1 )
  {
    std::cerr << this->m_ProgramHelpText << std::endl;
    return HELPREQUESTED;
  }

  /** Display the help text if the user asked for it. */
  if(  this->ArgumentExists( "--help" )
    || this->ArgumentExists( "-help" )
    || this->ArgumentExists( "--h" ) )
  {
    std::cerr << this->m_ProgramHelpText << std::endl;
    return HELPREQUESTED;
  }

  /** Loop through all required arguments. Check them all even if one fails. */
  bool allRequiredArgumentsSpecified = true;
  for( std::size_t i = 0; i < this->m_RequiredArguments.size(); ++i )
  {
    if( !this->ArgumentExists( this->m_RequiredArguments[ i ].first ) )
    {
      std::cerr << "ERROR: Argument "
        << this->m_RequiredArguments[ i ].first
        << " is required but not specified.\n  "
        << this->m_RequiredArguments[ i ].second << std::endl;
      allRequiredArgumentsSpecified = false;
    }
  }

  /** Loop through ExactlyOneOf argument sets. */
  for( std::size_t i = 0; i < this->m_RequiredExactlyOneArguments.size(); ++i )
  {
    std::vector<std::string> exactlyOneOf
      = this->m_RequiredExactlyOneArguments[ i ].first;
    if( !this->ExactlyOneExists( exactlyOneOf ) )
    {
      std::cerr << "ERROR: Exactly one (1) of the arguments in {";
      for ( std::size_t j = 0; j < exactlyOneOf.size() - 1; j++ )
      {
        std::cerr << exactlyOneOf[ j ] << ", ";
      }
      std::cerr << exactlyOneOf[ exactlyOneOf.size() - 1 ]
        << "} is required, but none or multiple are specified.\n  "
        << this->m_RequiredExactlyOneArguments[ i ].second << std::endl;

      allRequiredArgumentsSpecified = false;
    }
  }

  if( !allRequiredArgumentsSpecified ) return FAILED;

  return PASSED;

} // end CheckForRequiredArguments()


/**
 * ******************* PrintSelf *******************
 */

void
CommandLineArgumentParser
::PrintSelf( std::ostream & os, Indent indent ) const
{
  os << "ProgramHelpText\n" << this->m_ProgramHelpText << std::endl;

  os << std::endl << "CommandLine arguments:" << std::endl;
  ArgumentMapType::const_iterator it = this->m_ArgumentMap.begin();
  for( ; it != this->m_ArgumentMap.end(); ++it )
  {
    std::string key = it->first;
    os << indent << key;

    std::vector< std::string > arg;
    this->GetCommandLineArgument( key, arg );

    if( arg.size() > 0 ) os << ":\t";
    for( std::size_t i = 0; i < arg.size(); ++i )
    {
      os << arg[ i ] << " ";
    }
    os << std::endl;
  }

} // end PrintSelf()


} // end namespace itk

#endif // end #ifndef __itkCommandLineArgumentParser_h_
