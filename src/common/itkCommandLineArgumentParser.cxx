#ifndef __itkCommandLineArgumentParser_cxx
#define __itkCommandLineArgumentParser_cxx

#include "itkCommandLineArgumentParser.h"

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
    if ( this->m_Argv[ i ].substr( 0, 1 ) == "-" )
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
  if ( this->m_ArgumentMap.count( key ) == 0 )
  {
    return false;
  }
  return true;

} // end ArgumentExists()


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
    if ( !keyFound && this->m_Argv[ i ] == key )
    {
      keyFound = true;
      keyIndex = i;
      continue;
    }
    if ( keyFound && this->m_Argv[ i ].substr(0,1) == "-" )
    {
      if ( !this->IsANumber( this->m_Argv[ i ] ) )
      {
        nextKeyIndex = i;
        break;
      }
    }
  } // end for loop

  /** Check if the key was found and if the next argument is not also a key. */
  if ( !keyFound ) return false;
  if ( nextKeyIndex - keyIndex == 1 ) return false;

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
  if ( arg.size() > 1 )
  {
    if ( npos != number.find( arg.substr( 1, 1 ) ) )
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


} // end namespace itk

#endif // end #ifndef __itkCommandLineArgumentParser_h
