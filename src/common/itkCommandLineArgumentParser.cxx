#ifndef __itkCommandLineArgumentParser_cxx
#define __itkCommandLineArgumentParser_cxx

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
 * ******************* MarkExactlyOneOfArgumentsAsRequired *******************
 */

void
CommandLineArgumentParser
::MarkExactlyOneOfArgumentsAsRequired( const std::vector<std::string> & arguments )
{
  m_RequiredExactlyOneArguments.push_back(arguments);
} // end MarkExactlyOneOfArgumentsAsRequired()

/**
 * ******************* ExactlyOneExists *******************
 */

bool
CommandLineArgumentParser
::ExactlyOneExists( const std::vector<std::string> & keys ) const
{
  unsigned int counter = 0;
  for(unsigned int i = 0; i < keys.size(); i++)
  {
    if ( ArgumentExists(keys[i]) )
    {
      counter++;
    }
  }

  if(counter == 1)
  {
    return true;
  }
  else
  {
    return false;
  }

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

/**
 * **************** MarkArgumentAsRequired ***************
 */

void
CommandLineArgumentParser
::MarkArgumentAsRequired(const std::string & argument, const std::string & helpText)
{
  std::pair<std::string, std::string> requiredArgument;
  requiredArgument.first = argument;
  requiredArgument.second = helpText;
  m_RequiredArguments.push_back(requiredArgument);
} // end MarkArgumentAsRequired()

/**
 * **************** CheckForRequiredArguments ***************
 */

bool
CommandLineArgumentParser
::CheckForRequiredArguments() const
{
  // If no arguments were specified at all, display the help text.
  if(m_Argv.size() == 1)
    {
    std::cerr << m_ProgramHelpText << std::endl;
    return false;
    }
    
  // Display the help text if the user asked for it.
  if(ArgumentExists("--help") || ArgumentExists("-help") || ArgumentExists("--h"))
    {
    std::cerr << m_ProgramHelpText << std::endl;
    return false;
    }

  // Loop through all required arguments. Check them all even if one fails.

  bool allRequiredArgumentsSpecified = true;

  for(unsigned int i = 0; i < m_RequiredArguments.size(); ++i)
    {
    if(!ArgumentExists(m_RequiredArguments[i].first))
      {
      std::cout << "Argument " << m_RequiredArguments[i].first << " is required but not specified." << std::endl
                << "This argument is: " << m_RequiredArguments[i].second << std::endl;
      allRequiredArgumentsSpecified = false;
      }
    }

  // Loop through ExactlyOneOf argument sets
  for(unsigned int i = 0; i < m_RequiredExactlyOneArguments.size(); ++i)
  {
    if(!ExactlyOneExists(m_RequiredExactlyOneArguments[i]))
    {
      allRequiredArgumentsSpecified = false;
    }
  }
  
  return allRequiredArgumentsSpecified;
} // end CheckForRequiredArguments()

} // end namespace itk

#endif // end #ifndef __itkCommandLineArgumentParser_h
