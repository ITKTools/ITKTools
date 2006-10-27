#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "math.h"

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check arguments for help. */
	if ( argc < 3 )
	{
		PrintHelp();
		return 1;
	}

  /** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
 	std::string	inputTextFile = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputTextFile );

  std::string	whichMean = "arithmetic";
	bool retm = parser->GetCommandLineArgument( "-m", whichMean );

  unsigned int skiprow = 0;
  bool retsr = parser->GetCommandLineArgument( "-sr", skiprow );

  unsigned int skipcolumn = 0;
  bool retsc = parser->GetCommandLineArgument( "-sc", skipcolumn );

  unsigned int column = 0;
  bool retc = parser->GetCommandLineArgument( "-c", column );

  /** Check if the required arguments are given. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
  if ( whichMean != "arithmetic" && whichMean != "geometric" )
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
    unsigned int l = 0;
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
  double mean = 0.0, std = 0.0;
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
  }
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
  } // end if

  /** Setup the output format. */
  std::cout << std::fixed;
  std::cout << std::showpoint;
  std::cout << std::setprecision( 6 );

  /** Print output to screen. */
  std::cout << meanString << mean << std::endl;
  std::cout << stdString  << std  << std::endl;

	/** Return a value. */
	return 0;

} // end main


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
	std::cout << "Usage:" << std::endl << "pxcomputemean" << std::endl;
	std::cout << "\t-in\tinput text file" << std::endl;
  std::cout << "\t[-m]\twhat kind of mean" << std::endl;
  std::cout << "\t[-c]\tcolumn of which the mean is taken" << std::endl;
  std::cout << "\t[-s]\tskip: how many rows are skipped" << std::endl;
  std::cout << "-m should be \"arithmetic\" or \"geometric\", the default is \"arithmetic\"." << std::endl;
} // end PrintHelp

