#ifndef __KappaStatisticMainHelper_h
#define __KappaStatisticMainHelper_h

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------

/** Declare GetInputData. */
bool GetInputData( const std::string & filename,
  const std::vector<unsigned int> & columns,
  std::vector<std::vector<unsigned int> > & matrix );

/** Declare ReadInputData. */
bool ReadInputData( const std::string & filename,
  std::vector<std::vector<double> > & matrix );

#endif // end #ifndef __KappaStatisticMainHelper_h

