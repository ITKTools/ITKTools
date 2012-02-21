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
 \brief Create a random image.
 
 \verbinclude createrandomimage.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "createrandomimage.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program creates a random image.\n"
    << "Usage:\n"
    << "pxcreaterandomimage\n"
    << "  -out     OutputImageFileName\n"
    << "  -pt      PixelType <SHORT, USHORT, INT, UINT, CHAR, UCHAR, FLOAT>\n"
    << "  -id      ImageDimension <2,3>\n"
    << "  [-sd]    SpaceDimension (the number of channels) <1,2,3>\n"
    << "  -d0      Size of dimension 0\n"
    << "  -d1      Size of dimension 1\n"
    << "  [-d2]    Size of dimension 2\n"
    << "  [-r]     The resolution of the random image <unsigned long>.\n"
    << "This determines the number of voxels set to a random value before blurring.\n"
    << "If set to 0, all voxels are set to a random value\n"
    << "  [-sigma] The standard deviation of the blurring filter\n"
    << "  [-min]   Minimum pixel value\n"
    << "  [-max]   Maximum pixel value\n"
    << "  [-seed]  The random seed <int>";
  //<< "\t[-d3]  \tSize of dimension 3\n"
  //<< "\t[-d4]  \tSize of dimension 4\n"
  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  /** Required argument checking. */
  parser->MarkArgumentAsRequired( "-out", "" );

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
  std::string outputImageFileName = "";
  parser->GetCommandLineArgument( "-out", outputImageFileName );

  std::string pixelType = "";
  parser->GetCommandLineArgument( "-pt", pixelType );

  unsigned int dim = 0;
  parser->GetCommandLineArgument( "-id", dim );

  unsigned int spaceDimension = 1;
  parser->GetCommandLineArgument( "-sd", spaceDimension );

  double sigma = -1.0;
  parser->GetCommandLineArgument( "-sigma", sigma );

  double min_value = 0.0;
  parser->GetCommandLineArgument( "-min", min_value );

  double max_value = 0.0;
  parser->GetCommandLineArgument( "-max", max_value );

  int rand_seed = 0;
  parser->GetCommandLineArgument( "-seed", rand_seed );

  unsigned int imageDimension = 0;
  
  /** Checks. */
  if( dim == 0 )
  {
    std::cerr << "ERROR: Image dimension cannot be 0" << std::endl;
    return EXIT_FAILURE;
  }

  unsigned long nrOfPixels = 1;
  std::ostringstream makeString("");
  itk::Array<unsigned int> sizes;
  sizes.SetSize( dim );
  for( unsigned int i = 0; i < dim ; i++ )
  {
    makeString.str("");
    makeString << "-d" << i;
    unsigned int dimsize = 0;
    bool retdimsize = parser->GetCommandLineArgument( makeString.str(), dimsize );
    if( !retdimsize )
    {
      sizes[ i ] = dimsize;
      nrOfPixels *= sizes[ i ];
    }
  }

  unsigned long resolution = nrOfPixels / 64;
  parser->GetCommandLineArgument( "-r", resolution );

  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::GetComponentTypeFromString( pixelType );
  
  /** Class that does the work. */
  ITKToolsCreateRandomImageBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 2, unsigned char >::New( dim, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, unsigned int >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCreateRandomImage< 3, unsigned char >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_OutputFileName = outputImageFileName;
    filter->m_Sizes = sizes;
    filter->m_Min_value = min_value;
    filter->m_Max_value = max_value;
    filter->m_Resolution = resolution;
    filter->m_Sigma = sigma;
    filter->m_Rand_seed = rand_seed;
    filter->m_SpaceDimension = spaceDimension;
    
    filter->Run();
    
    delete filter;  
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

} // end function main
