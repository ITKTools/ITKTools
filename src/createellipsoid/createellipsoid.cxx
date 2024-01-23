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
 \brief Create an ellipsoid image.

 \verbinclude createellipsoid.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "createellipsoid.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxcreateellipsoid\n"
    << "-out     outputFilename\n"
    << "-sz      image size (voxels)\n"
    << "[-sp]    image spacing (mm)\n"
    << "-c       center (mm)\n"
    << "-r       radii (mm)\n"
    << "[-o]     orientation, default xyz\n"
    << "[-dim]   dimension, default 3\n"
    << "[-pt]    pixelType, default short\n"
    << "The orientation is a dim*dim matrix, specified in row order.\n"
    << "The user should take care of supplying an orthogonal matrix.\n"
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
  parser->MarkArgumentAsRequired( "-sz", "The size." );
  parser->MarkArgumentAsRequired( "-c", "The center." );
  parser->MarkArgumentAsRequired( "-r", "The radius." );

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

  std::vector<double> radius;
  parser->GetCommandLineArgument( "-r", radius );

  unsigned int dim = 3;
  parser->GetCommandLineArgument( "-dim", dim );

  std::string componentTypeAsString = "short";
  parser->GetCommandLineArgument( "-opct", componentTypeAsString );
  itk::ImageIOBase::IOComponentEnum componentType
    = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );

  std::vector<double> spacing( dim, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> orientation( dim * dim, 0.0 );
  bool reto = parser->GetCommandLineArgument( "-o", orientation );

  if( !reto )
  {
    for( unsigned int i = 0; i < dim; i++ )
    {
      orientation[ i * ( dim + 1 ) ] = 1.0;
    }
  }

  /** Class that does the work. */
  ITKToolsCreateEllipsoidBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateEllipsoid< 3, double >::New( dim, componentType );
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
    filter->m_Orientation = orientation;

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
