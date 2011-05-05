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
 \brief Perform T-test.
 
 \verbinclude ttest.help
 */
#include "itkCommandLineArgumentParser.h"

#include <vector>
#include <fstream>
#include <iomanip>
#include <itksys/SystemTools.hxx>
#include "itkTDistribution.h"

//-------------------------------------------------------------------------------------

/* Declare ReadInputData. */
bool ReadInputData( const std::string & filename, std::vector<std::vector<double> > & matrix );

/* Declare ComputeTValue. */
bool ComputeTValue( const std::vector<double> & samples1,
    const std::vector<double> & samples2, const unsigned int type,
    double & tValue,
    double & mean1, double & mean2, double & meandiff,
    double & std1, double & std2, double & stddiff );

/* Declare ComputeMeanAndStandardDeviation. */
void ComputeMeanAndStandardDeviation(
  const std::vector<double> & samples1,
  const std::vector<double> & samples2,
  double & mean1, double & mean2, double & meandiff,
  double & std1, double & std2, double & stddiff );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-c", "Columns." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string output = "p";
  parser->GetCommandLineArgument( "-out", output );

  std::vector<unsigned int> columns( 2, 0 );
  parser->GetCommandLineArgument( "-c", columns );

  unsigned int tail = 2;
  parser->GetCommandLineArgument( "-tail", tail );

  unsigned int type = 1;
  parser->GetCommandLineArgument( "-type", type );

  unsigned int precision = 8;
  parser->GetCommandLineArgument( "-p", precision );

  /** Check command line arguments. */
  if ( columns.size() != 2 )
  {
    std::cerr << "ERROR: You should specify two different columns with \"-c\"." << std::endl;
    return 1;
  }
  if ( columns[ 0 ] == columns[ 1 ] )
  {
    std::cerr << "ERROR: You should specify two different columns with \"-c\"." << std::endl;
    return 1;
  }
  if ( output != "p" && output != "all" )
  {
    std::cerr << "ERROR: output should be one of \"p\" or \"all\"." << std::endl;
    return 1;
  }

  /** Read the input file. */
  std::vector< std::vector<double> > matrix;
  bool readSuccess = ReadInputData( inputFileName, matrix );
  if ( !readSuccess)
  {
    std::cerr << "ERROR: Something went wrong reading \""
      << inputFileName << "\"." << std::endl;
    return 1;
  }

  /** Check if there are at least two data points. */
  if ( matrix.size() < 2 )
  {
    std::cerr << "ERROR: Each column should contain at least two samples." << std::endl;
    return 1;
  }

  /** Check if the requested columns exists. */
  if ( matrix[ 0 ].size() - 1 < columns[ 0 ] ||
    matrix[ 0 ].size() - 1 < columns[ 1 ] )
  {
    std::cerr << "ERROR: Requesting an unexisting column. There are only "
      << matrix[ 0 ].size() << " columns." << std::endl;
    return 1;
  }

  /** Extract the two requested columns. */
  std::vector<double> samples1( matrix.size() ), samples2( matrix.size() );
  for ( unsigned int i = 0; i < matrix.size(); ++i )
  {
    samples1[ i ] = matrix[ i ][ columns[ 0 ] ];
    samples2[ i ] = matrix[ i ][ columns[ 1 ] ];
    //std::cout << samples1[ i ] << " " << samples2[ i ] << std::endl;
  }

  /** Compute the t value. */
  double tValue = 0.0;
  double mean1, mean2, meandiff, std1, std2, stddiff;
  mean1 = mean2 = meandiff = std1 = std2 = stddiff = 0.0;
  bool retctv = ComputeTValue( samples1, samples2, type, tValue,
    mean1, mean2, meandiff, std1, std2, stddiff );
  if ( !retctv ) return 1;
  //std::cout << "t: " << tValue << std::endl;

  /** Compute the p-value. */
  typedef itk::Statistics::TDistribution    DistributionType;
  DistributionType::Pointer distributionFunction = DistributionType::New();
  distributionFunction->SetDegreesOfFreedom( samples1.size() - 1 );
  //double pValue = distributionFunction->EvaluateCDF( tValue );
  double pValue = distributionFunction->EvaluateCDF( -vcl_abs( tValue ) );

  /** For a 2-tailed t-test, multiply with 2. */
  if ( tail == 2 ) pValue *= 2.0;

  /** Print the p-value to screen. */
  std::cout << std::fixed << std::showpoint << std::setprecision( precision );
  if ( output == "p" )
  {
    std::cout << pValue << std::endl;
  }
  else if ( output == "all" )
  {
    std::cout << "            mean +/- std" << std::endl;
    std::cout << "samples 1:  " << mean1 << " " << std1 << std::endl;
    std::cout << "samples 2:  " << mean2 << " " << std2 << std::endl;
    std::cout << "difference: " << meandiff << " " << stddiff << std::endl;
    std::cout << "dof = " << samples1.size() - 1
      << ", t = " << tValue
      << ", p = " << pValue << std::endl;
  }

  /** End program. */
  return 0;

} // end main


  /*
   * ******************* ReadInputData *******************
   *
   * This function reads the specified input text file.
   * No error checking is done. Each line of the file should
   * consist of an equal amount of elements. Each column should
   * contain the data samples over which a t-test is performed.
   * The file should not contain text, and no headers.
   */

bool ReadInputData( const std::string & filename, std::vector<std::vector<double> > & matrix )
{
  /** Open file for reading. */
  std::ifstream file( filename.c_str() );
  std::string line;

  /** Read the file line by line. */
  if ( file.is_open() )
  {
    /** Read the first line to find out how long it is. */
    std::getline( file, line );

    /** We use the function:
     * static kwsys_stl::vector<String> SplitString( const char* s,
     *   char separator = '/', bool isPath = false );
     * The columns are assumed to be separated by one space ' ' or one tab '\t'.
     */
    std::vector<itksys::String> linevec1 = itksys::SystemTools::SplitString(
      line.c_str(), ' ', false );
    std::vector<itksys::String> linevec2 = itksys::SystemTools::SplitString(
      line.c_str(), '\t', false );
    unsigned int linelength = linevec1.size() > linevec2.size() ? linevec1.size() : linevec2.size();

    /** Read and convert the first line. */
    std::istringstream lineSS1( line.c_str() );
    std::vector<double> linevec( linelength );
    for ( unsigned int i = 0; i < linelength; i++ )
    {
      lineSS1 >> linevec[ i ];
    }
    matrix.push_back( linevec );

    /** Read the remaining lines. */
    while ( !file.eof() )
    {
      std::getline( file, line );
      std::istringstream lineSS( line.c_str() );
      for ( unsigned int i = 0; i < linelength; i++ )
      {
        lineSS >> linevec[ i ];
      }
      matrix.push_back( linevec );
    } // end reading file

    /** Remove last line. */
    matrix.resize( matrix.size() - 1 );
  }
  else
  {
    return false;
  }

  /** Return a value. */
  return true;

} // end ReadInputData()


  /*
   * ******************* ComputeTValue *******************
   */

bool ComputeTValue( const std::vector<double> & samples1,
    const std::vector<double> & samples2,
    const unsigned int type,
    double & tValue,
    double & mean1, double & mean2, double & meandiff,
    double & std1, double & std2, double & stddiff )
{
  /** Which type of t-test is requested? */
  if ( type == 1 )
  {
    /** This type is a paired t-test.
     *   X = samples1 - samples2, N = X.size()
     *   tValue = mean( X ) * sqrt( N ) / std( X )
     * Check for equal size of samples1 and samples2 (which is in this case always true).
     */
    if ( samples1.size() != samples2.size() )
    {
      std::cerr << "ERROR: requested a paired t-test, but the samples have unequal length." << std::endl;
      return false;
    }

    /** ComputeMeanAndStandardDeviation. */
    ComputeMeanAndStandardDeviation(
      samples1, samples2,
      mean1, mean2, meandiff,
      std1, std2, stddiff );

    /** Compute the t-value. */
    tValue = meandiff * vcl_sqrt( static_cast<double>( samples1.size() ) ) / stddiff;
  }
  else
  {
    std::cerr << "ERROR: This type is not supported. Choose one of {1}." << std::endl;
    return false;
  }

  /** Return a vlue. */
  return true;
} // end ComputeTValue()


  /*
   * ******************* ComputeMeanAndStandardDeviation *******************
   */

void ComputeMeanAndStandardDeviation(
  const std::vector<double> & samples1,
  const std::vector<double> & samples2,
  double & mean1, double & mean2, double & meandiff,
  double & std1, double & std2, double & stddiff )
{
  /** The slow way: two loops. */
  /*
  mean = std = 0.0;
  for ( unsigned int i = 0; i < vec.size(); ++i )
  {
    mean += vec[ i ];
  }
  mean /= vec.size();
  for ( unsigned int i = 0; i < vec.size(); ++i )
  {
    std += ( vec[ i ] - mean ) * ( vec[ i ] - mean );
  }
  std /= ( vec.size() - 1 );
  std = vcl_sqrt( std );
  */
  
  /** Compute mean and std fast, using one loop. */
  double N = static_cast<double>( samples1.size() );
  double s1 = 0.0, ss1 = 0.0;
  double s2 = 0.0, ss2 = 0.0;
  double sd = 0.0, ssd = 0.0;
  for ( unsigned int i = 0; i < static_cast<unsigned int>( N ); ++i )
  {
    s1  += samples1[ i ];
    ss1 += samples1[ i ] * samples1[ i ];
    s2  += samples2[ i ];
    ss2 += samples2[ i ] * samples2[ i ];
    sd  += samples1[ i ] - samples2[ i ];
    ssd += ( samples1[ i ] - samples2[ i ] ) * ( samples1[ i ] - samples2[ i ] );
  }
  mean1 = s1 / N;
  std1 = vcl_sqrt( ( ss1 * N - s1 * s1 ) / ( N * ( N - 1.0 ) ) );
  mean2 = s2 / N;
  std2 = vcl_sqrt( ( ss2 * N - s2 * s2 ) / ( N * ( N - 1.0 ) ) );
  meandiff = sd / N;
  stddiff = vcl_sqrt( ( ssd * N - sd * sd ) / ( N * ( N - 1.0 ) ) );

} // end ComputeMeanAndStandardDeviation()


  /*
   * ******************* GetHelpString *******************
   */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxttest" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   output, choose one of {p,all}, default p" << std::endl
  << "             p: only print the p-value" << std::endl
  << "             all: print all" << std::endl
  << "  -c       the two data sample columns" << std::endl
  << "  [-tail]  one or two tailed, defauls = 2" << std::endl
  << "  [-type]  the type of the t-test, default = 1:" << std::endl
  << "             1: paired" << std::endl
  << "             2: two-sample equal variance" << std::endl
  << "             3: two-sample unequal variance" << std::endl
  << "  [-p]     the output precision, default = 8:" << std::endl
  << "The input file should be in a certain format. No text is allowed." << std::endl
  << "No headers are allowed. The data samples should be displayed in columns." << std::endl
  << "Columns should be separated by a single space or tab.";
    
  return ss.str();
} // end GetHelpString()
