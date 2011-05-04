#ifndef __KappaStatisticMainHelper_cxx
#define __KappaStatisticMainHelper_cxx

#include "KappaStatisticMainHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxkappastatistic" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  -type    the type of the kappa test:" << std::endl
  << "             fleiss: unweighted, for many observers" << std::endl
  << "             cohen: weighted, for two observers only" << std::endl
  << "  -c       the data columns on which the kappa test is performed" << std::endl
  << "  [-w]     the weights used in the Cohen kappa test, default linear:" << std::endl
  << "             linear:    1 - | i - j | / ( k - 1 )" << std::endl
  << "             quadratic: 1 - [ (i - j ) / ( k - 1 ) ]^2" << std::endl
  << "             identity:  I_k, the identity matrix" << std::endl
  << "  [-std]   use this option to calculate the standard deviation" << std::endl
  << "  [-cmp]   use this option to specify a kappa to which you want to compare" << std::endl
  << "           the found kappa. The returned standard deviation is different if" << std::endl
  << "           this option is not specified." << std::endl
  << "  [-out]   output, choose one of {kappa,all,ALL}, default all" << std::endl
  << "             kappa: only print the kappa-value" << std::endl
  << "             all: print all" << std::endl
  << "             ALL: print more" << std::endl
  << " [-p]     the output precision, default = 8:" << std::endl
  << "The input file should be in a certain format. No text is allowed." << std::endl
  << "No headers are allowed. The data samples should be displayed in columns." << std::endl
  << "Columns should be separated by a single space or tab." << std::endl
  << "For more information about the kappa statistic and this implementation, read the tex-file found in the repository.";

  return ss.str();
} // end GetHelpString()


/**
 * ******************* ReadInputData *******************
 *
 * This function reads an input text file and puts it in a matrix.
 * No error checking is done. Each line of the file should
 * consist of an equal amount of columns. Each column should
 * contain floating data.
 * The file should not contain text, and no headers.
 *
 * The data from one line is stored in a vector, so it is mirorred
 * compared to the input.
 */

bool ReadInputData( const std::string & filename,
  std::vector<std::vector<double> > & matrix )
{
  /** Open file for reading. */
  std::ifstream file( filename.c_str() );
  std::string line;

  /** Read the file line by line. */
  if ( file.is_open() )
  {
    while ( std::getline( file, line ) )
    {
      std::istringstream lineSS( line.c_str() );
      double tmp = 0.0;
      std::vector<double> linevec;
      while ( lineSS >> tmp )
      {
        linevec.push_back( tmp );
      }
      matrix.push_back( linevec );
    }
  }
  else
  {
    return false;
  }

  /** Close the file and return a value. */
  file.close();
  return true;

} // end ReadInputData()


/**
 * ******************* GetInputData *******************
 *
 * This function:
 * - reads the input text file.
 * - some checks are done
 * - the required columns are extracted
 */

bool GetInputData( const std::string & fileName,
  const std::vector<unsigned int> & columns,
  std::vector<std::vector<unsigned int> > & matrix )
{
  /** Read the input file. */
  std::vector< std::vector<double> > inputMatrix;
  bool retin = ReadInputData( fileName, inputMatrix );
  if ( !retin )
  {
    std::cerr << "ERROR: Something went wrong reading \""
      << fileName << "\"." << std::endl;
    return 1;
  }

  /** Check if there are at least two columns. */
  if ( inputMatrix[ 0 ].size() < 2 )
  {
    std::cerr << "ERROR: The file should contain at least two sample sets." << std::endl;
    return false;
  }

  /** Check that each column contains at least two data points.
   * It is assumed that all columns are of the same length.
   */
  if ( inputMatrix.size() < 2 )
  {
    std::cerr << "ERROR: The columns should contain at least two samples." << std::endl;
    return false;
  }

  /** Check if the requested columns exists. */
  for ( unsigned int i = 0; i < columns.size(); ++i )
  {
    if ( inputMatrix[ 0 ].size() - 1 < columns[ i ] )
    {
      std::cerr << "ERROR: Requesting an unexisting column. There are only "
        << inputMatrix[ 0 ].size() << " columns." << std::endl;
      return false;
    }
  }

  /** Extract the requested columns. */
  matrix.resize( 0 );
  matrix.resize( columns.size() );
  for ( unsigned int i = 0; i < matrix.size(); ++i )
  {
    matrix[ i ].resize( inputMatrix.size() );
    for ( unsigned int j = 0; j < inputMatrix.size(); ++j )
    {
      matrix[ i ][ j ] = static_cast<unsigned int>( inputMatrix[ j ][ columns[ i ] ] );
    }
  }

  /** Return a value. */
  return true;

} // end GetInputData()

#endif // end #ifndef __KappaStatisticMainHelper_cxx

