#include "itkCommandLineArgumentParser.h"

#include <vector>
#include <fstream>
#include <iomanip>
#include <itksys/SystemTools.hxx>
#include "itkTDistribution.h"

//-------------------------------------------------------------------------------------

/** Declare ReadInputData. */
bool ReadInputData( const std::string & filename, std::vector<std::vector<double> > & matrix );

/** Declare ComputeTValue. */
bool ComputeTValue( const std::vector<double> & samples1,
    const std::vector<double> & samples2, const unsigned int type, double & tValue );

/** Declare ComputeMeanAndStandardDeviation. */
void ComputeMeanAndStandardDeviation( const std::vector<double> & vec,
  double & mean, double & std );

/** Declare PrintHelp. */
void PrintHelp( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
	if ( argc < 5 || argc > 13 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::string	inputFileName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<unsigned int> columns( 2, 0 );
  bool retc = parser->GetCommandLineArgument( "-c", columns );

  unsigned int tail = 2;
  bool rettail = parser->GetCommandLineArgument( "-tail", tail );

  unsigned int type = 1;
  bool rettype = parser->GetCommandLineArgument( "-type", type );

  unsigned int precision = 8;
  bool retp = parser->GetCommandLineArgument( "-p", precision );

  /** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
	if ( !retc )
	{
		std::cerr << "ERROR: You should specify \"-c\"." << std::endl;
		return 1;
	}

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

  /** Read the input file. */
  std::vector< std::vector<double> > matrix;
  retin = ReadInputData( inputFileName, matrix );
  if ( !retin )
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
  double tValue;
  bool retctv = ComputeTValue( samples1, samples2, type, tValue );
  if ( !retctv ) return 1;
  //std::cout << "t: " << tValue << std::endl;

  /** Compute the p-value. */
  typedef itk::Statistics::TDistribution    DistributionType;
  DistributionType::Pointer distributionFunction = DistributionType::New();
  distributionFunction->SetDegreesOfFreedom( samples1.size() - 1 );
  double pValue = distributionFunction->EvaluateCDF( tValue );

  /** For a 2-tailed t-test, multiply with 2. */
  if ( tail == 2 ) pValue *= 2.0;

  /** Print the p-value to screen. */
  std::cout << std::fixed << std::showpoint << std::setprecision( precision );
  std::cout << pValue << std::endl;

	/** End program. */
	return 0;

} // end main


	/**
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


  /**
	 * ******************* ComputeTValue *******************
	 */

bool ComputeTValue( const std::vector<double> & samples1,
    const std::vector<double> & samples2,
    const unsigned int type,
    double & tValue )
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

    /** Compute the difference. */
    std::vector<double> diff( samples1.size() );
    for ( unsigned int i = 0; i < diff.size(); ++i )
    {
      diff[ i ] = samples1[ i ] - samples2[ i ];
    }

    /** ComputeMeanAndStandardDeviation. */
    double mean, std;
    ComputeMeanAndStandardDeviation( diff, mean, std );

    /** Compute the t-value. */
    tValue = mean * vcl_sqrt( static_cast<double>( diff.size() ) ) / std;
  }
  else
  {
    std::cerr << "ERROR: This type is not supported. Choose one of {1}." << std::endl;
    return false;
  }

  /** Return a vlue. */
  return true;
} // end ComputeTValue()


  /**
	 * ******************* ComputeMeanAndStandardDeviation *******************
	 */

void ComputeMeanAndStandardDeviation(
  const std::vector<double> & vec,
  double & mean, double & std )
{
  /** The slow way: two loops. *
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

  /** Compute mean and std fast, using one loop. */
  double s = 0.0, ss = 0.0;
  for ( unsigned int i = 0; i < vec.size(); ++i )
  {
    s += vec[ i ];
    ss += vec[ i ] * vec[ i ];
  }
  mean = s / vec.size();
  std = ( ss * vec.size() - s * s ) / ( vec.size() * ( vec.size() - 1.0 ) );
  std = vcl_sqrt( std );

} // end ComputeMeanAndStandardDeviation()


  /**
	 * ******************* PrintHelp *******************
	 */

void PrintHelp( void )
{
	std::cout << "Usage:" << std::endl << "pxttest" << std::endl;
	std::cout << "  -in     inputFilename" << std::endl;
  std::cout << "  -c      the two data sample columns" << std::endl;
	std::cout << "  [-tail] one or two tailed, defauls = 2" << std::endl;
  std::cout << "  [-type] the type of the t-test, default = 1:" << std::endl;
  std::cout << "            1: paired" << std::endl;
  std::cout << "            2: two-sample equal variance" << std::endl;
  std::cout << "            3: two-sample unequal variance" << std::endl;
  std::cout << "  [-p]    the output precision, default = 8:" << std::endl;
  std::cout << "The input file should be in a certain format. No text is allowed." << std::endl;
  std::cout << "No headers are allowed. The data samples should be displayed in columns." << std::endl;
  std::cout << "Columns should be separated by a single space or tab." << std::endl;
} // end PrintHelp()
