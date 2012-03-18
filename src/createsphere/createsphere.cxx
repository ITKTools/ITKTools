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
 \brief Create a sphere image.

 \verbinclude createsphere.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "createsphere.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
  << "Usage:" << std::endl
  << "pxcreatesphere" << std::endl
    << "-out     outputFilename" << std::endl
    << "-sz      image size (voxels)" << std::endl
    << "[-sp]    image spacing (mm)" << std::endl
    << "-c       center (mm)" << std::endl
    << "-r       radii (mm)" << std::endl
    << "[-dim]   dimension, default 3" << std::endl
    << "[-pt]    pixelType, default short" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";
  return ss.str();
} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-sz", "Size." );
  parser->MarkArgumentAsRequired( "-c", "Center." );
  parser->MarkArgumentAsRequired( "-r", "Radius." );

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
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> size;
  parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> center;
  parser->GetCommandLineArgument( "-c", center );

  double radius = 0.0f;;
  parser->GetCommandLineArgument( "-r", radius );

  unsigned int dim = 3;
  parser->GetCommandLineArgument( "-dim", dim );

  std::string componentTypeAsString = "short";
  parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  std::vector<double> spacing( dim, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  /** String to component type. */
  itk::ImageIOBase::IOComponentType componentType
    = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );

  /** Class that does the work. */
  ITKToolsCreateSphereBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsCreateSphere< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCreateSphere< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateSphere< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputFileName;
    filter->m_Size = size;
    filter->m_Spacing = spacing;
    filter->m_Center = center;
    filter->m_Radius = radius;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. Return a value. */
  return EXIT_SUCCESS;

} // end main
