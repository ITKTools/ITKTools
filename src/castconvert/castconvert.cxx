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
 \brief This program converts and possibly casts images.

 \verbinclude castconvert.help
 */
/*
 * authors:       Marius Staring and Stefan Klein
 *
 * Thanks to Hans J. Johnson for a modification to this program. This
 * modification breaks down the program into smaller compilation units,
 * so that the compiler does not overflow.
 *
 * ENH: on 23-05-2006 we added multi-component support.
 * ENH: on 09-06-2006 we added support for extracting a specific DICOM serie.
 */

#include "itkCommandLineArgumentParser.h"
#include "castconverthelpers.h"
#include "castconverthelpers2.h"

// Some non-standard IO Factories
#include "itkGE4ImageIOFactory.h"
#include "itkGE5ImageIOFactory.h"
#include "itkGEAdwImageIOFactory.h"
#ifdef ITKTOOLS_ITKIOPhilipsREC_Found
#include "itkPhilipsRECImageIOFactory.h"
#endif


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Description:\n"
     << "This program converts between many image formats.\n"
     << "This is done by reading in an image, possibly casting of the image,\n"
     << "and subsequently writing the image to the user-specified format.\n"
     << "\nDefinitions:\n"
     << "- converting: changing the extension of the image, e.g. bmp, mhd, etc.\n"
     << "- casting: changing the component type of a voxel, e.g. short, float,\n"
     << "           unsigned long, etc.\n"
     << "\nNotes:\n"
     << "- Casting of scalar images is done by the itk::ShiftScaleImageFilter,\n"
     << "  where values are mapped to itself, leaving the intensity range\n"
     << "  the same. NB: When casting to a component type with smaller dynamic\n"
     << "  range, information might get lost.\n"
     << "- Casting of multi-component images, such as vector or RGB images, is\n"
     << "  done using the itk::VectorCastImageFilter.\n"
     << "- Input images can be in all file formats ITK supports and for which\n"
     << "  the itk::ImageFileReader works, and additionally 3D dicom series.\n"
     << "  It is also possible to extract a specific DICOM series from a directory\n"
     << "  by supplying the seriesUID.\n"
     << "- Output images can be in all file formats ITK supports and for which\n"
     << "  the itk::ImageFileWriter works. Dicom output is not supported yet.\n"
     << "\n" << std::endl
     << "Usage:\n"
     << "pxcastconvert\n"
     << "  -in      inputfilename\n"
     << "  -out     outputfilename\n"
     << "  [-opct]  outputPixelComponentType, default equal to input\n"
     << "  [-z]     compression flag; if provided, the output image is compressed\n"
     << "OR pxcastconvert\n"
     << "  -in      dicomDirectory\n"
     << "  -out     outputfilename\n"
     << "  [-opct]  outputPixelComponentType, default equal to input\n"
     << "  [-s]     seriesUID, default the first UID found\n"
     << "  [-r]     add restrictions to generate a unique seriesUID\n"
     << "           e.g. \"0020|0012\" to add a check for acquisition number.\n"
     << "  [-z]     compression flag; if provided, the output image is compressed\n\n"
     << "OutputPixelComponentType should be one of {[unsigned_]char, [unsigned_]short,\n"
     << "  [unsigned_]int, [unsigned_]long, float, double}.\n"
     << "NB: Not every image format supports all OutputPixelComponentTypes.\n"
     << "NB2: Not every image format supports the compression flag \"-z\"." << std::endl;

  return ss.str();

} // end GetHelpString()


/** Break the program into smaller compilation units. */
extern void ITKToolsCastConvert2D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert );
extern void ITKToolsCastConvert3D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert );
extern void ITKToolsCastConvert4D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert );
extern void ITKToolsCastConvertDICOM3D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Register some non-standard IO Factories to make the tool more useful.
   * Copied from the Insight Applications.
   */
  itk::GE4ImageIOFactory::RegisterOneFactory();
  itk::GE5ImageIOFactory::RegisterOneFactory();
  itk::GEAdwImageIOFactory::RegisterOneFactory();
#ifdef ITKTOOLS_ITKIOPhilipsREC_Found
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();
#endif

  /** Construct the command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  /** Validate the command line arguments. */
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if ( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if ( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Get the command line arguments. */
  std::string input = "";
  parser->GetCommandLineArgument( "-in", input );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string outputPixelComponentType = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", outputPixelComponentType );

  std::string seriesUID = "";
  parser->GetCommandLineArgument( "-s", seriesUID );

  std::vector<std::string> restrictions;
  parser->GetCommandLineArgument( "-r", restrictions );

  bool useCompression = parser->ArgumentExists( "-z" );

  /** Check -opct. */
  if ( retopct )
  {
    if ( !itktools::ComponentTypeIsValid(
      itk::ImageIOBase::GetComponentTypeFromString( outputPixelComponentType ) ) )
    {
      std::cerr << "The user-provided \"-opct\" is "
        << outputPixelComponentType
        << ", which is not supported." << std::endl;
      return EXIT_FAILURE;
    }
  }

  /** Are we dealing with an image or a DICOM series? */
  bool isDICOM = false;
  bool allOK = IsDICOM( input, isDICOM );
  if ( !allOK )
  {
    std::cout << "ERROR: " << input << " does not exist." << std::endl;
    return EXIT_FAILURE;
  }

  /** Class that does the work */
  ITKToolsCastConvertBase * castConvert = NULL;

  /** Get image information. */
  std::string inputFileName = "";
  std::string inputDirectoryName = "";
  unsigned int dim = 0;
  if ( !isDICOM )
  {
    inputFileName = input;
  }
  else
  {
    inputDirectoryName = input;

    /** Get the first DICOM image file name, to extract information from. */
    std::string fileNameOfFirstDICOMImage = "";
    std::string errorMessage = "";
    bool allOK = GetFileNameFromDICOMDirectory(
      inputDirectoryName, fileNameOfFirstDICOMImage,
      seriesUID, restrictions, errorMessage );
    if ( !allOK )
    {
      std::cerr << errorMessage << std::endl;
      return EXIT_FAILURE;
    }

    inputFileName = fileNameOfFirstDICOMImage;
  }

  /** Get dimension and component type. */
  itktools::GetImageDimension( inputFileName, dim );
  itktools::ComponentType componentType
    = itktools::GetImageComponentType( inputFileName );
  if ( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( outputPixelComponentType );
  }

  /** Construct a castconvert class of the correct type. */
  try
  {
    if ( !isDICOM )
    {
      if ( !castConvert ) ITKToolsCastConvert2D( componentType, dim, castConvert );

#ifdef ITKTOOLS_3D_SUPPORT
      if ( !castConvert ) ITKToolsCastConvert3D( componentType, dim, castConvert );
#endif

#ifdef ITKTOOLS_4D_SUPPORT
      if ( !castConvert ) ITKToolsCastConvert4D( componentType, dim, castConvert );
#endif

    }
    else
    {
#ifdef ITKTOOLS_3D_SUPPORT
      if ( !castConvert ) ITKToolsCastConvertDICOM3D( componentType, dim, castConvert );
#endif
    }

    if ( !castConvert )
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "  pixel (component type = "
        << itk::ImageIOBase::GetComponentTypeAsString( componentType )
        << "\n  dimension = " << dim
        << std::endl;
      return EXIT_FAILURE;
    }

    /** Set the arguments. */
    castConvert->m_InputFileName = inputFileName;
    castConvert->m_OutputFileName = outputFileName;
    castConvert->m_UseCompression = useCompression;

    castConvert->m_InputDirectoryName = inputDirectoryName;
    castConvert->m_DICOMSeriesUID = seriesUID;
    castConvert->m_DICOMSeriesRestrictions = restrictions;

    castConvert->Run();

    delete castConvert;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete castConvert;
    return EXIT_FAILURE;
  }

  std::cout << "Successful conversion!" << std::endl;

  /** End  program. Return success. */
  return EXIT_SUCCESS;

}  // end main
