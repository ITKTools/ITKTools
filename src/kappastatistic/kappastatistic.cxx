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
 \brief Compute a kappa statistic.
 
 \verbinclude kappastatistic.help
 */
#include "itkCommandLineArgumentParser.h"
#include "KappaStatisticMainHelper.h"

#include "itkFleissKappaStatistic.h"
#include "itkCohenWeightedKappaStatistic.h"

//-------------------------------------------------------------------------------------

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

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-type", "The type." );
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
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<unsigned int> columns;
  parser->GetCommandLineArgument( "-c", columns );

  std::string type = "";
  parser->GetCommandLineArgument( "-type", type );

  std::string weights = "linear";
  parser->GetCommandLineArgument( "-w", weights );

  std::string output = "all";
  parser->GetCommandLineArgument( "-out", output );

  unsigned int precision = 8;
  parser->GetCommandLineArgument( "-p", precision );

  bool exstd = parser->ArgumentExists( "-std" );

  double kappacmp = 0.0;
  bool retcmp = parser->GetCommandLineArgument( "-cmp", kappacmp );

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
