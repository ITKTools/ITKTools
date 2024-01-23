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
 \brief Extracts a 2D slice from a 3D image.

 \verbinclude extractslice.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "extractslice.h"

#include <string>
#include <vector>
#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "pxextractslice extracts an (n-1)D slice from an nD image.\n"
    << "Usage:\n"
    << "pxextractslice\n"
    << "  -in      input image filename\n"
    << "  [-out]   output image filename\n"
    << "  [-opct]  pixel type of input and output images;\n"
    << "           default: automatically determined from the first input image.\n"
    << "  -sn      slice number\n"
    << "  [-d]     the dimension from which a slice is extracted, default the last dimension\n"
    << "  [-z]     compression flag; if provided, the output image is compressed\n"
    << "Supported pixel types: (unsigned) char, (unsigned) short, float.";

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
  parser->MarkArgumentAsRequired( "-sn", "The slice number." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Get the input file name. */
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get the slicenumber which is to be extracted. */
  unsigned int slicenumber = 0;
  parser->GetCommandLineArgument( "-sn", slicenumber );

  std::string slicenumberstring;
  parser->GetCommandLineArgument( "-sn", slicenumberstring );

  bool useCompression = parser->ArgumentExists( "-z" );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  std::vector<unsigned int> imageSize;
  bool retgip = itktools::GetImageProperties(
    inputFileName, pixelType, componentType, dim, numberOfComponents, imageSize );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Let the user overrule this. */
  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );
  if ( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Get the dimension in which the slice is to be extracted.
   * The default is the last direction.
   */
  unsigned int which_dimension = dim - 1;
  parser->GetCommandLineArgument( "-d", which_dimension );

  /** Sanity check. */
  if( slicenumber > imageSize[ which_dimension ] )
  {
    std::cerr << "ERROR: You selected slice number "
      << slicenumber
      << ", where the input image only has "
      << imageSize[ which_dimension ]
      << " slices in dimension "
      << which_dimension << "." << std::endl;
    return EXIT_FAILURE;
  }

  /** Sanity check. */
  if( which_dimension > dim - 1 )
  {
    std::cerr << "ERROR: You selected to extract a slice from dimension "
      << which_dimension + 1
      << ", where the input image is "
      << dim
      << "D." << std::endl;
    return EXIT_FAILURE;
  }

  /** Get the outputFileName. */
  std::string direction = "t";
  if( which_dimension == 0 ) direction = "x";
  else if( which_dimension == 1 ) direction = "y";
  else if( which_dimension == 2 ) direction = "z";
  std::string part1 = itksys::SystemTools::GetFilenameWithoutLastExtension( inputFileName );
  std::string part2 = itksys::SystemTools::GetFilenameLastExtension( inputFileName );
  std::string outputFileName = part1 + "_slice_" + direction + "=" + slicenumberstring + part2;
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Class that does the work. */
  ITKToolsExtractSliceBase * filter = 0;

  try
  {
    // now call all possible template combinations.
#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsExtractSlice< 3, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 3, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 3, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 3, float >::New( dim, componentType );
#endif
#ifdef ITKTOOLS_4D_SUPPORT
    if( !filter ) filter = ITKToolsExtractSlice< 4, unsigned char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 4, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 4, unsigned short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 4, short >::New( dim, componentType );
    if( !filter ) filter = ITKToolsExtractSlice< 4, float >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_WhichDimension = which_dimension;
    filter->m_Slicenumber = slicenumber;
    filter->m_UseCompression = useCompression;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** Return a value. */
  return EXIT_SUCCESS;

} // end main
