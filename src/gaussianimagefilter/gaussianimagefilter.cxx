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
 \brief Perform a Gaussian filtering on an image.

 \verbinclude gaussianimagefilter.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "gaussianimagefilter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxgaussianimagefilter\n"
    << "  -in      inputFilename\n"
    << "  [-out]   outputFilename, default in + BLURRED.mhd\n"
    << "  [-std]   sigma, for each dimension, default 1.0\n"
    << "  [-ord]   order, for each dimension, default zero\n"
    << "             0: zero order = blurring\n"
    << "             1: first order = gradient\n"
    << "             2: second order derivative\n"
    << "  [-mag]   compute the magnitude of the separate blurrings, default false\n"
    << "  [-lap]   compute the laplacian, default false\n"
    << "  [-inv]   compute invariants, choose one of\n"
    << "           {LiLi, LiLijLj, LiLijLjkLk, Lii, LijLji, LijLjkLki}\n"
    << "  [-opct]  output pixel type, default equal to input\n"
    << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
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
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::vector<float> sigma;
  sigma.push_back( 1.0 ); // default 1.0 for each resolution
  parser->GetCommandLineArgument( "-std", sigma );

  std::vector<unsigned int> order;
  parser->GetCommandLineArgument( "-ord", order );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "BLURRED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  bool retmag = parser->ArgumentExists( "-mag" );

  bool retlap = parser->ArgumentExists( "-lap" );

  std::string invariant = "LiLi";
  bool retinv = parser->GetCommandLineArgument( "-inv", invariant );

  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  /** Check options. */
  for( unsigned int i = 0; i < order.size(); ++i )
  {
    if( order[ i ] > 2 )
    {
      std::cerr << "ERROR: The order should not be higher than 2." << std::endl;
      std::cerr << "Only zeroth, first and second order derivatives are supported." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check that not both mag and lap are given. */
  if( retmag && retlap )
  {
    std::cerr << "ERROR: only one of \"-mag\" and \"-lap\" should be given!" << std::endl;
    return EXIT_FAILURE;
  }

  /** Check which operation is requested. */
  std::string whichOperation = "Gaussian";
  if( retmag ) whichOperation = "Magnitude";
  else if( retlap ) whichOperation = "Laplacian";
  else if( retinv ) whichOperation = "Invariants";
  else whichOperation = "Gaussian";

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

  /** The default output is equal to the input, but can be overridden by
   * specifying -opct in the command line.
   */
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Check order. */
  if( !retlap && !retinv )
  {
    if( order.size() != dim )
    {
      std::cerr << "ERROR: the # of orders should be equal to the image dimension!" << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Check sigma. */
  if( sigma.size() != 1 && sigma.size() != dim )
  {
    std::cerr << "ERROR: the # of sigmas should be equal to 1 or the image dimension!" << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work. */
  ITKToolsGaussianBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsGaussian< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsGaussian< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsGaussian< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_WhichOperation = whichOperation;
    filter->m_Sigma = sigma;
    filter->m_Order = order;
    filter->m_Invariant = invariant;

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
