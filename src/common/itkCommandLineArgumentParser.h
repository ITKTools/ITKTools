#ifndef __itkCommandLineArgumentParser_h
#define __itkCommandLineArgumentParser_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include <vector>
#include <string>
#include <map>

namespace itk
{

	/**
	 * \class CommandLineArgumentParser
	 *
	 * \brief This class parses command line arguments. This
	 * makes it easy to get the desired argument.
	 *
	 */

class CommandLineArgumentParser :
	public Object
{
public:
	/** Standard class typedefs. */
  typedef CommandLineArgumentParser			Self;
  typedef Object											  Superclass;
  typedef SmartPointer<Self>						Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

	/** Method for creation through the object factory. */
  itkNewMacro( Self );
 
  /** Run-time type information (and related methods). */
  itkTypeMacro( CommandLineArgumentParser, Object );

	/** Set the command line arguments in a vector of strings. */
	void SetCommandLineArguments( int argc, char **argv );

  /** Map to store the arguments and their indices. */
  typedef std::map< std::string, unsigned int >   ArgumentMapType;
  typedef ArgumentMapType::value_type					    EntryType;
  void CreateArgumentMap( void );
  bool ArgumentExists( std::string key );

	/** Get command line argument if it is not a vector type. */
	template <class T>
		bool GetCommandLineArgument( const std::string & key, T & arg )
	{
		std::vector<T> vec;
		bool returnvalue = this->GetCommandLineArgument( key, vec );
		if ( returnvalue ) arg = vec[ 0 ];

		return returnvalue;

	}; // end GetCommandLineArgument()

	/** Get command line argument if it is a String type. */
	template <class T>
		bool GetCommandLineArgumentString( const std::string & key, std::vector<T> & arg )
	{
		/** Check for the key. */
		unsigned int keyIndex, nextKeyIndex;
		keyIndex = nextKeyIndex = 0;
		bool keyFound = this->FindKey( key, keyIndex, nextKeyIndex );
		if ( !keyFound ) return false;
		
    /** If a vector of size oldSize > 1 is given to this function, and if
     * only one (1) argument is provided in the command line, we create
     * a vector of size oldSize and fill it with the single argument.
     */
    unsigned int oldSize = arg.size();
    if ( oldSize > 1 && nextKeyIndex - keyIndex == 2 )
    {
      arg.clear();
      arg.resize( oldSize, m_argv[ keyIndex + 1 ] );
      return true;
    }
    
    /** Otherwise, gather the arguments and put them in arg. */
    unsigned int newSize = nextKeyIndex - keyIndex - 1;
    newSize = newSize > oldSize ? newSize : oldSize;
    arg.resize( newSize );
    unsigned int j = 0;
		for ( unsigned int i = keyIndex + 1; i < nextKeyIndex; i++ )
		{
			arg[ j ] = m_argv[ i ];
      ++j;
		}
		return true;

	}; // end GetCommandLineArgumentString()

	/** Get command line argument if it is an Integer type. */
	template <class T>
		bool GetCommandLineArgumentInteger( const std::string & key, std::vector<T> & arg )
	{
		/** Check for the key. */
		unsigned int keyIndex, nextKeyIndex;
		keyIndex = nextKeyIndex = 0;
		bool keyFound = this->FindKey( key, keyIndex, nextKeyIndex );
		if ( !keyFound ) return false;
		
    /** If a vector of size oldSize > 1 is given to this function, and if
     * only one (1) argument is provided in the command line, we create
     * a vector of size oldSize and fill it with the single argument.
     */
    unsigned int oldSize = arg.size();
    if ( oldSize > 1 && nextKeyIndex - keyIndex == 2 )
    {
      arg.clear();
      arg.resize( oldSize, static_cast<T>( atoi( m_argv[ keyIndex + 1 ].c_str() ) ) );
      return true;
    }
    
    /** Otherwise, gather the arguments and put them in arg. */
    unsigned int newSize = nextKeyIndex - keyIndex - 1;
    newSize = newSize > oldSize ? newSize : oldSize;
    arg.resize( newSize );
    unsigned int j = 0;
		for ( unsigned int i = keyIndex + 1; i < nextKeyIndex; i++ )
		{
			arg[ j ] = static_cast<T>( atoi( m_argv[ i ].c_str() ) );
      ++j;
		}
		return true;

	}; // end GetCommandLineArgumentInteger()

	/** Get command line argument if it is a Real type. */
	template <class T>
		bool GetCommandLineArgumentReal( const std::string & key, std::vector<T> & arg )
	{
		/** Check for the key. */
		unsigned int keyIndex, nextKeyIndex;
		keyIndex = nextKeyIndex = 0;
		bool keyFound = this->FindKey( key, keyIndex, nextKeyIndex );
		if ( !keyFound ) return false;
		
    /** If a vector of size oldSize > 1 is given to this function, and if
     * only one (1) argument is provided in the command line, we create
     * a vector of size oldSize and fill it with the single argument.
     */
    unsigned int oldSize = arg.size();
    if ( oldSize > 1 && nextKeyIndex - keyIndex == 2 )
    {
      arg.clear();
      arg.resize( oldSize, static_cast<T>( atof( m_argv[ keyIndex + 1 ].c_str() ) ) );
      return true;
    }
    
    /** Otherwise, gather the arguments and put them in arg. */
    unsigned int newSize = nextKeyIndex - keyIndex - 1;
    newSize = newSize > oldSize ? newSize : oldSize;
    arg.resize( newSize );
    unsigned int j = 0;
		for ( unsigned int i = keyIndex + 1; i < nextKeyIndex; i++ )
		{
			arg[ j ] = static_cast<T>( atof( m_argv[ i ].c_str() ) );
      ++j;
		}
		return true;

	}; // end GetCommandLineArgumentReal()

	/** Specialisations for String type. */
//	bool GetCommandLineArgument( const std::string & key, std::vector<*char> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<std::string> & arg );

	/** Specialisations for Integer type. */
  bool GetCommandLineArgument( const std::string & key, std::vector<char> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<unsigned char> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<int> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<unsigned int> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<short> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<unsigned short> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<long> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<unsigned long> & arg );

	/** Specialisations for Real type. */
	bool GetCommandLineArgument( const std::string & key, std::vector<float> & arg );
	bool GetCommandLineArgument( const std::string & key, std::vector<double> & arg );

protected:
  CommandLineArgumentParser() {}
  virtual ~CommandLineArgumentParser() {}

	/** General functionality. */
	bool FindKey( const std::string & key, unsigned int & keyIndex, unsigned int & nextKeyIndex );

	bool IsANumber( const std::string & arg );

	/** A vector of strings to store the command line arguments. */
	std::vector<std::string> m_argv;

  /** A map to store the arguments and their indices. */
  ArgumentMapType m_ArgumentMap;

private:
	CommandLineArgumentParser(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end class CommandLineArgumentParser

} // end namespace itk

#endif // end #ifndef __itkCommandLineArgumentParser_h

