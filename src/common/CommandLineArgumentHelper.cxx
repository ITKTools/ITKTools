#ifndef __CommandLineArgumentHelper_cxx
#define __CommandLineArgumentHelper_cxx

#include "CommandLineArgumentHelper.h"

void ReplaceUnderscoreWithSpace( std::string & arg )
{
	/** Get rid of the possible "_" in arg. */
	std::basic_string<char>::size_type pos = arg.find( "_" );
	const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
	if ( pos != npos )
	{
		arg.replace( pos, 1, " " );
	}
	
} // end ReplaceUnderscoreWithSpace

#endif // end #ifndef __CommandLineArgumentHelper_h
