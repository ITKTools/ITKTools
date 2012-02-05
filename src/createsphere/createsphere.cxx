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

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  parser->GetCommandLineArgument( "-pt", PixelType );

  std::vector<double> spacing( Dimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", spacing );

  /** Get rid of the possible "_" in PixelType. */
  itktools::ReplaceUnderscoreWithSpace( PixelType );

  /** Run the program. */
  
  /** Class that does the work */
  ITKToolsCreateSphereBase * createSphere = 0; 

  /** Short alias */
  unsigned int dim = Dimension;

  itktools::ComponentType componentType = itk::ImageIOBase::GetComponentTypeFromString( PixelType );

  try
  {    
    // now call all possible template combinations.
    if (!createSphere) createSphere = ITKToolsCreateSphere< unsigned char, 2 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< char, 2 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< unsigned short, 2 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< short, 2 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< float, 2 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< double, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createSphere) createSphere = ITKToolsCreateSphere< unsigned char, 3 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< char, 3 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< unsigned short, 3 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< short, 3 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< float, 3 >::New( componentType, dim );
    if (!createSphere) createSphere = ITKToolsCreateSphere< double, 3 >::New( componentType, dim );
#endif
    if (!createSphere) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    createSphere->m_OutputFileName = outputFileName;
    createSphere->m_Size = size;
    createSphere->m_Spacing = spacing;
    createSphere->m_Center = center;
    createSphere->m_Radius = radius;
    

    createSphere->Run();
    
    delete createSphere;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createSphere;
    return 1;
  }
 
  /** End program. Return a value. */
  return 0;

} // end main
