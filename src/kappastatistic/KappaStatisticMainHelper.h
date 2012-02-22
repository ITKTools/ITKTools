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
#ifndef __KappaStatisticMainHelper_h_
#define __KappaStatisticMainHelper_h_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <itksys/SystemTools.hxx>

/** Declare GetInputData. */
bool GetInputData( const std::string & filename,
  const std::vector<unsigned int> & columns,
  std::vector<std::vector<unsigned int> > & matrix );

/** Declare ReadInputData. */
bool ReadInputData( const std::string & filename,
  std::vector<std::vector<double> > & matrix );


#endif // end #ifndef __KappaStatisticMainHelper_h_
