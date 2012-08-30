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
 \brief Perform enhancement on an image. Mainly vessel and sheetness enhancement.

 \verbinclude enhancement.help
 * \authors Changyan Xiao, Marius Staring, Denis Shamonin,
 * Johan H.C. Reiber, Jan Stolk, Berend C. Stoel
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "enhancement.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxenhancement\n"
    << "  -in      inputFilename\n"
    << "  -out     outputFilename[s]: enhancement [and optionally optimal scales]\n"
    << "  [-std]   Gaussian smoothing standard deviation\n"
    << "             1 value: sigma\n"
    << "             3 values: sigmaMin, sigmaMax, nrOfSteps\n"
    << "  [-ssm]   Sigma step method. Choose one of\n"
    << "             {0 - Equispaced sigma steps, 1 - Logarithmic sigma steps }\n"
    << "             default: 1 - Logarithmic sigma steps\n"
    << "  [-rescaleoff]   Rescale off. Default on.\n"
    << "  [-threads] maximum number of threads used, default all.\n"
    << std::endl
    << "  [-m]     method, choose one of:\n"
    << "             FrangiVesselness       - Frangi vesselness [1]\n"
    << "             StrainEnergyVesselness - Strain energy vesselness [2]\n"
    << "             ModifiedKrissianVesselness - vesselness based on Krissian paper [3,4]\n"
    << "             FrangiSheetness        - Frangi sheetness [1]\n"
    << "             DescoteauxSheetness    - Descoteaux sheetness [5]\n"
    << "             StrainEnergySheetness  - Strain energy sheetness [2]\n"
    << "             FrangiXiaoSheetness    - Frangi-Xiao sheetness [?]\n"
    << "             DescoteauxXiaoSheetness - Descoteaux-Xiao sheetness [?]\n"
    << std::endl
    << "  Options for method FrangiVesselness:\n"
    << "  [-alpha] alpha, distinguish sheets from lines, default 0.5\n"
    << "  [-beta]  beta, blob-ness, default 0.5\n"
    << "  [-C]     C, noise\n"
    << std::endl
    << "  Options for method StrainEnergyVesselness:\n"
    << "  [-alpha] alpha, brightness contrast, [0,1[\n"
    << "  [-nu]    nu, structure strength, [-1,0.5], default 0\n"
    << "  [-kappa] kappa, sharpness of vesselness function, ]0,infty[\n"
    << "  [-beta]  beta, step edge suppression, ]0,infty[\n"
    << std::endl
    << "  Options for method ModifiedKrissianVesselness:\n"
    << "           has no parameters\n"
    << std::endl
    << "  Options for method FrangiSheetness:\n"
    << "  [-alpha] alpha, distinguish sheets from lines, default 0.5\n"
    << "  [-beta]  beta, blob-ness, default 0.5\n"
    << "  [-C]     C, noise\n"
    << std::endl
    << "  Options for method DescoteauxSheetness:\n"
    << "  [-alpha] alpha, distinguish sheets from lines, default 0.5\n"
    << "  [-beta]  beta, blob-ness, default 0.5\n"
    << "  [-C]     C, noise\n"
    << std::endl
    << "  Options for method StrainEnergySheetness:\n"
    << "           same as StrainEnergyVesselness\n"
    << "  Options for method FrangiXiaoSheetness: ...\n"
    << "           same as Frangi + kappa (step edge) \n"
    << "  Options for method DescoteauxXiaoSheetness: ...\n"
    << "           same as Descoteaux + kappa (step edge) \n"
    << "Supported: 3D, float, double.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-m", "The enhancement method." );

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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<std::string> outputFileNames;
  parser->GetCommandLineArgument( "-out", outputFileNames );

  //parameters.GenerateScalesOutput = ( parameters.outputFileNames.size() == 2 );

  std::string method;
  bool retmethod = parser->GetCommandLineArgument( "-m", method );

  std::vector<double> std( 1, 1.0 );
  bool retstd = parser->GetCommandLineArgument( "-std", std );

  unsigned int sigmaStepMethod = 1;
  bool retssm = parser->GetCommandLineArgument( "-ssm", sigmaStepMethod );

  bool retrescale = parser->ArgumentExists( "-rescaleoff" );

  unsigned int maxThreads = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  parser->GetCommandLineArgument( "-threads", maxThreads );
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads( maxThreads );

  // Enhancement filter parameters
  double alpha = 0.5;
  bool retalpha = parser->GetCommandLineArgument( "-alpha", alpha );

  double beta = 0.5;
  bool retbeta = parser->GetCommandLineArgument( "-beta", beta );

  double c = 500;
  bool retC = parser->GetCommandLineArgument( "-C", c );

  double nu = 0.0;
  bool retnu = parser->GetCommandLineArgument( "-nu", nu );

  double kappa = 1.0;
  bool retkappa = parser->GetCommandLineArgument( "-kappa", kappa );

  /** Sanity checks. */
  if ( retstd && ( std.size() != 1 && std.size() != 3 ) )
  {
    std::cerr << "ERROR: You should specify 1 or 3 values for \"-std\"." << std::endl;
    return EXIT_FAILURE;
  }
  if ( outputFileNames.size() != 1 && outputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify 1 or 2 values for \"-out\"." << std::endl;
    return EXIT_FAILURE;
  }
  if ( retssm && ( sigmaStepMethod != 0 && sigmaStepMethod != 1 ) )
  {
    std::cerr << "ERROR: \"-ssm\" should be one of {0, 1}." << std::endl;
    return EXIT_FAILURE;
  }

  /*if ( ( parameters.Method == 1 || parameters.Method == 2 || parameters.Method == 3 )
    && (!retsn || !retbn || !retnn) )
  {
    std::cerr << "ERROR: You should specify \"-sn\" \"-bn\" \"-nn\"." << std::endl;
    return EXIT_FAILURE;
  }

  if ( parameters.Method == 0 && (!retalpha || !retnu || !retkappa || !retbeta) )
  {
    std::cerr << "ERROR: You should specify \"-alpha\" \"-nu\" \"-kappa\" \"-beta\"." << std::endl;
    return EXIT_FAILURE;
  }

  if ( ( parameters.Method == 4 )
    && (!retsn || !retbn || !retnn || !retses ) )
  {
    std::cerr << "ERROR: You should specify \"-sn\" \"-bn\" \"-nn\" \"-ses\"." << std::endl;
    return EXIT_FAILURE;
  }*/

  /** Get the range of sigma values. */
  double sigmaMinimum = std[ 0 ];
  double sigmaMaximum = std[ 0 ];
  unsigned int numberOfSigmaSteps = 1;
  if ( std.size() == 3 )
  {
    sigmaMaximum = std[ 1 ];
    numberOfSigmaSteps = static_cast<unsigned int>( std[ 2 ] );
  }

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Component type should be at least float. */
  if ( componentType != itk::ImageIOBase::FLOAT && componentType != itk::ImageIOBase::DOUBLE )
  {
    componentType = itk::ImageIOBase::FLOAT;
  }

  /** Class that does the work. */
  ITKToolsEnhancementBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    // 2D not supported

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsEnhancement< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsEnhancement< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileNames = outputFileNames;
    filter->m_Method = method;
    filter->m_Rescale = !retrescale;
    filter->m_SigmaStepMethod = sigmaStepMethod;
    filter->m_SigmaMinimum = sigmaMinimum;
    filter->m_SigmaMaximum = sigmaMaximum;
    filter->m_NumberOfSigmaSteps = numberOfSigmaSteps;
    filter->m_Alpha = alpha;
    filter->m_Beta = beta;
    filter->m_C = c;
    filter->m_Nu = nu;
    filter->m_Kappa = kappa;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main

