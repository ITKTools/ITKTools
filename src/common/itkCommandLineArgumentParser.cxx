#ifndef __itkCommandLineArgumentParser_cxx
#define __itkCommandLineArgumentParser_cxx

#include "itkCommandLineArgumentParser.h"

namespace itk
{

	/**
	 * ******************* SetCommandLineArguments *******************
	 */

	void CommandLineArgumentParser::
		SetCommandLineArguments( int argc, char **argv )
	{
		m_argv.resize( argc );
		for ( unsigned int i = 0; i < static_cast<unsigned int>( argc ); i++ )
		{
			m_argv[ i ] = argv [ i ];
		}
	} // end SetCommandLineArguments


	/**
	 * ******************* FindKey *******************
	 */

	bool CommandLineArgumentParser::
		FindKey( const std::string & key, unsigned int & keyIndex, unsigned int & nextKeyIndex )
		{
		/** Loop once over the arguments, to get the index of the key,
		 * and that of the next key. */
		bool keyFound = false;
		keyIndex = 0;
		nextKeyIndex = m_argv.size();
		for ( unsigned int i = 0; i < m_argv.size(); i++ )
		{
			if ( !keyFound && m_argv[ i ] == key )
			{
				keyFound = true;
				keyIndex = i;
				continue;
			}
			if ( keyFound && m_argv[ i ].substr(0,1) == "-" )
			{
				if ( !this->IsANumber( m_argv[ i ] ) )
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
	 * Checks if something startin with a "-" is a key or a negative number.
	 */

	bool CommandLineArgumentParser::
		IsANumber( const std::string & arg )
		{
			std::string number = "0123456789";
			if ( arg.size() > 1 )
			{
				return number.find( arg.substr(1,1) );
			}

			return false;
		} // end IsANumber()

	/**
	 * ******************* GetCommandLineArgument *******************
	 *
	 * Different specialisations for String types.
	 */

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<std::string> & arg )
	{
		return this->GetCommandLineArgumentString( key, arg );
	}


	/**
	 * ******************* GetCommandLineArgument *******************
	 *
	 * Different specialisations for Integer types.
	 */

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<int> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<unsigned int> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<short> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<unsigned short> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<long> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<unsigned long> & arg )
	{
		return this->GetCommandLineArgumentInteger( key, arg );
	}


	/**
	 * ******************* GetCommandLineArgument *******************
	 *
	 * Different specialisations for Real types.
	 */

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<float> & arg )
	{
		return this->GetCommandLineArgumentReal( key, arg );
	}

	bool CommandLineArgumentParser::
		GetCommandLineArgument( const std::string & key, std::vector<double> & arg )
	{
		return this->GetCommandLineArgumentReal( key, arg );
	}

} // end name space itk

#endif // end #ifndef __itkCommandLineArgumentParser_h

