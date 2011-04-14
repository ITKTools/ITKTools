#include "itkCommandLineArgumentParser.h"
#include "KappaStatisticMainHelper.h"

#include "itkFleissKappaStatistic.h"
#include "itkCohenWeightedKappaStatistic.h"

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 6 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<unsigned int> columns;
  bool retc = parser->GetCommandLineArgument( "-c", columns );

  std::string type = "";
  bool rettype = parser->GetCommandLineArgument( "-type", type );

  std::string weights = "linear";
  parser->GetCommandLineArgument( "-w", weights );

  std::string output = "all";
  parser->GetCommandLineArgument( "-out", output );

  unsigned int precision = 8;
  parser->GetCommandLineArgument( "-p", precision );

  bool exstd = parser->ArgumentExists( "-std" );

  double kappacmp = 0.0;
  bool retcmp = parser->GetCommandLineArgument( "-cmp", kappacmp );

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }
  if ( !rettype )
  {
    std::cerr << "ERROR: You should specify \"-type\"." << std::endl;
    return 1;
  }
  if ( !retc )
  {
    std::cerr << "ERROR: You should specify \"-c\"." << std::endl;
    return 1;
  }

  /** Check command line arguments. */
  type = itksys::SystemTools::LowerCase( type );
  if ( type != "fleiss" && type != "cohen" )
  {
    std::cerr << "ERROR: type should be one of {fleiss,cohen}." << std::endl;
    return 1;
  }

  if ( columns.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two columns with \"-c\"." << std::endl;
    return 1;
  }

  std::string smallOut = itksys::SystemTools::LowerCase( output );
  if ( smallOut != "kappa" && smallOut != "all" )
  {
    std::cerr << "ERROR: output should be one of \"kappa\" or \"all\"." << std::endl;
    return 1;
  }

  if ( retcmp ) exstd = true;

  /** Read the input file. */
  std::vector< std::vector<unsigned int> > matrix;
  retin = GetInputData( inputFileName, columns, matrix );
  if ( !retin ) return 1;

  /** Typedefs. */
  typedef itk::Statistics::FleissKappaStatistic         FleissType;
  typedef itk::Statistics::CohenWeightedKappaStatistic  CohenType;
  typedef FleissType::SamplesType                       SamplesType;

  /** Create the kappa calculators. */
  FleissType::Pointer fleiss = FleissType::New();
  CohenType::Pointer cohen = CohenType::New();
  unsigned int n = 0, N = 0, k = 0;
  double Po, Pe, kappa, std;

  /** Compute kappa. */
  try
  {
    if ( type == "fleiss" )
    {
      fleiss->SetObservations( matrix );

      n = fleiss->GetNumberOfObservers();
      N = fleiss->GetNumberOfObservations();
      k = fleiss->GetNumberOfCategories();

      if ( exstd )
      {
        fleiss->ComputeKappaStatisticValueAndStandardDeviation( Po, Pe, kappa, std, retcmp );
      }
      else
      {
        fleiss->ComputeKappaStatisticValue( Po, Pe, kappa );
      }
    }
    else if ( type == "cohen" )
    {
      cohen->SetObservations( matrix );

      n = cohen->GetNumberOfObservers();
      N = cohen->GetNumberOfObservations();
      k = cohen->GetNumberOfCategories();

      cohen->SetWeights( weights );
      if ( exstd )
      {
        cohen->ComputeKappaStatisticValueAndStandardDeviation( Po, Pe, kappa, std, retcmp );
      }
      else
      {
        cohen->ComputeKappaStatisticValue( Po, Pe, kappa );
      }
    }
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** Print the output. */
  std::cout << std::fixed << std::showpoint << std::setprecision( precision );
  if ( smallOut == "kappa" )
  {
    std::cout << kappa << std::endl;
  }
  else if ( smallOut == "all" )
  {
    std::cout << "# observers:    " << n << std::endl;
    std::cout << "# observations: " << N << std::endl;
    std::cout << "# categories:   " << k << std::endl;
    if ( type == "cohen" )
    {
      std::cout << "WeightsName:    " << cohen->GetWeightsName() << std::endl;
    }
    std::cout << "Observed agreement Po: " << Po << std::endl;
    std::cout << "Expected agreement Pe: " << Pe << std::endl;
    std::cout << "kappa:                 " << kappa << std::endl;

    if ( exstd )
    {
      std::cout << "standard deviation:    " << std << std::endl;
    }

    if ( output == "ALL" )
    {
      if ( type == "fleiss" ) fleiss->Print( std::cout );
      else if ( type == "cohen" ) cohen->Print( std::cout );
    }
  }

  /** End program. */
  return 0;

} // end main

