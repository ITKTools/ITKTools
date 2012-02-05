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
/** \file
 \brief Compute the mean of an image.
 
 \verbinclude computemean.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "math.h"
#include <algorithm>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
     << "Usage:" << std::endl
     << "pxcomputemean" << std::endl
     << "-in      input text file" << std::endl
     << "[-m]     what kind of mean" << std::endl
     << "[-c]     column of which the mean is taken" << std::endl
     << "[-s]     skip: how many rows are skipped" << std::endl
     << "[-p]     output precision" << std::endl
     << "-m should be \"arithmetic\", \"geometric\" or \"median\", the default is \"arithmetic\"." << std::endl
     << "The default output precision is 6." << std::endl
     << "The output for median is: minimum, first quartile, median, third quartile, maximum.";

  return ss.str();
} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments. */
  std::string inputTextFile = "";
  parser->GetCommandLineArgument( "-in", inputTextFile );

  std::string whichMean = "arithmetic";
  parser->GetCommandLineArgument( "-m", whichMean );

  unsigned int skiprow = 0;
  parser->GetCommandLineArgument( "-sr", skiprow );

  unsigned int skipcolumn = 0;
  parser->GetCommandLineArgument( "-sc", skipcolumn );

  unsigned int column = 0;
  parser->GetCommandLineArgument( "-c", column );

  unsigned int precision = 6;
  parser->GetCommandLineArgument( "-p", precision );

  if ( whichMean != "arithmetic" && whichMean != "geometric" && whichMean != "median" )
  {
    std::cerr << "ERROR: \"-m\" should be one of { arithmetic, geometric }." << std::endl;
    return 1;
  }

  /** Create file stream. */
  std::ifstream fileIn( inputTextFile.c_str() );

  /** tmp variables. */
  std::vector<double> values;
  std::string tmpString, line;

  /** Read the values. */
  if ( fileIn.is_open() )
  {

    /** Skip some lines. */
    for ( unsigned int i = 0; i < skiprow; i++ )
    {
      std::getline( fileIn, line );
    }

    /** Read the file line by line. */
    while ( !fileIn.eof() )
    {
      std::vector<double> tmp;
      std::getline( fileIn, line );
      std::istringstream lineSS( line.c_str() );
      for ( unsigned int i = 0; i < skipcolumn; i++ )
      {
        lineSS >> tmpString;
      }
      bool endOfLine = false;
      while ( !endOfLine )
      {
        double tmpd = -1.0;
        lineSS >> tmpd;
        if ( tmpd == -1.0 ) endOfLine = true;
        else tmp.push_back( tmpd );
      }
      /** Fill values. */
      if ( column < tmp.size() && tmp.size() != 0 )
      {
        values.push_back( tmp[ column ] );
      }
      else if ( column >= tmp.size() && tmp.size() != 0 )
      {
        std::cerr << "ERROR: There is no column nr. " << column << "." << std::endl;
        return 1;
      }
    } // end while over file
  } // end if
  else
  {
    std::cerr << "ERROR: The file \"" << inputTextFile << "\" could not be opened." << std::endl;
    return 1;
  }

  /** Close the file stream. */
  fileIn.close();

  /** Calculate mean and standard deviation. */
  double mean = 0.0, std = 0.0, median = 0.0,
    firstquartile = 0.0, thirdquartile = 0.0,
    minimum = 0.0, maximum = 0.0;
  std::string meanString = "", stdString = "";
  if ( whichMean == "arithmetic" )
  {
    /** The arithmetic version. */
    meanString = "Arithmetic mean: ";
    stdString  = "Arithmetic std : ";
    for ( unsigned int i = 0; i < values.size(); i++ )
    {
      mean += values[ i ];
    }
    mean /= values.size();
    for ( unsigned int i = 0; i < values.size(); i++ )
    {
      std += ( values[ i ] - mean ) * ( values[ i ] - mean );
    }
    std = sqrt( std / ( values.size() - 1.0 ) );
  } // end if arithmetic
  else if ( whichMean == "geometric" )
  {
    /** The geometic version. */
    meanString = "Geometric mean: ";
    stdString  = "Geometric std : ";
    for ( unsigned int i = 0; i < values.size(); i++ )
    {
      mean += log( values[ i ] );
    }
    mean /= values.size();
    for ( unsigned int i = 0; i < values.size(); i++ )
    {
      std += ( log( values[ i ] ) - mean ) * ( log( values[ i ] ) - mean );
    }
    mean = exp( mean );
    std = exp( sqrt( std / values.size() ) );
  } // end if geometric
  else if ( whichMean == "median" )
  {
    sort( values.begin(), values.end() );
    if ( values.size() % 2 )
    {
      /** size() is odd. */
      median = values[ ( values.size() + 1 ) / 2 - 1 ];
    }
    else
    {
      /** size() is even. */
      median = ( values[ values.size() / 2 - 1 ] + values[ values.size() / 2 ] ) / 2.0;
    }
    /** Determine first (Q1) and third quartile (Q3), and minimum and maximum.
     * Use the value at position: round( ( n + 1 ) / 4 ) for Q1
     * Use the value at position: round( ( 3n + 3 ) / 4 ) for Q3
     * Subtract 1 for the index, since this is c++.
     */
    unsigned int ind1 = static_cast<unsigned int>( ( values.size() + 1.0 ) / 4.0 + 0.5 );
    unsigned int ind3 = static_cast<unsigned int>( ( 3.0 * values.size() + 3.0 ) / 4.0 + 0.5 );
    minimum = values[ 0 ];
    maximum = values[ values.size() - 1 ];
    firstquartile = values[ ind1 - 1 ];
    thirdquartile = values[ ind3 - 1 ];
  } // end if median

  /** Setup the output format. */
  std::cout << std::fixed;
  std::cout << std::showpoint;
  std::cout << std::setprecision( precision );

  /** Print output to screen. */
  if ( whichMean == "arithmetic" || whichMean == "geometric" )
  {
    std::cout << meanString << mean << std::endl;
    std::cout << stdString  << std  << std::endl;
  }
  else if ( whichMean == "median" )
  {
    std::cout << minimum << " "
      << firstquartile << " "
      << median << " "
      << thirdquartile << " "
      << maximum << std::endl;
  }

  /** Return a value. */
  return 0;

} // end main
