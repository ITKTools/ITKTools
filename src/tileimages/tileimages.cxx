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
 \brief Either tiles a stack of 2D images into a 3D image, or tiles nD images to form another nD image.

 This program tiles a stacks of 2D images into a 3D image.
 This is done by employing an itk::SeriesFileReader.
 \verbinclude tileimages.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "TileImages.h"
#include "TileImages2D3D.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "pxtileimages EITHER tiles a stack of 2D images into a 3D image,\n"
    << "OR tiles nD images to form another nD image.\n"
    << "In the last case the way to tile is specified by a layout.\n"
    << "To stack a pile of 2D images an itk::SeriesFileReader is employed.\n"
    << "If no layout is specified with \"-ly\" 2D-3D tiling is done,\n"
    << "otherwise 2D-2D or 3D-3D tiling is performed.\n"
    << "Usage:  \npxtileimages\n"
    << "  -in      input image filenames, at least 2\n"
    << "  -out     output image filename\n"
    << "  [-pt]    pixel type of input and output images\n"
    << "           default: automatically determined from the first input image\n"
    << "  [-sp]    spacing in last direction for N-D to (N+1)-D tiling [double];\n"
    << "           if omitted, the origins of the N-D images are used to find the spacing;\n"
    << "           if these are identical, a spacing of 1.0 is assumed by default\n"
    << "  [-ly]    layout of the nD-nD tiling\n"
    << "           example: in 2D for 4 images \"-ly 2 2\" results in\n"
    << "             im1 im2\n"
    << "             im3 im4\n"
    << "           example: in 2D for 4 images \"-ly 4 1\" (or \"-ly 0 1\") results in\n"
    << "             im1 im2 im3 im4\n"
    << "  [-d]     default value, by default 0.\n"
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
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Get the input file names. */
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  if( inputFileNames.size() < 2 )
  {
    std::cout << "ERROR: You should specify at least two input images." << std::endl;
    return EXIT_FAILURE;
  }

  /** Get the outputFileName. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Read the z-spacing. */
  double lastSpacing = -1.0;
  parser->GetCommandLineArgument( "-sp", lastSpacing );

  /** Get the layout. */
  std::vector< unsigned int > layout;
  bool retly = parser->GetCommandLineArgument( "-ly", layout );

  /** Get the layout. */
  double defaultvalue = 0.0;
  parser->GetCommandLineArgument( "-d", defaultvalue );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ], pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Let the user overrule this. */
  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Run the program. */
  if( !retly )
  {
    /** Class that does the work. */
    ITKToolsTileImages2D3DBase * filterTile2D3D = nullptr;

    try
    {
      // now call all possible template combinations.
#ifdef ITKTOOLS_3D_SUPPORT
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 3, unsigned char >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 3, char >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 3, unsigned short >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 3, short >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 3, float >::New( dim + 1, componentType );
#endif
#ifdef ITKTOOLS_4D_SUPPORT
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 4, unsigned char >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 4, char >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 4, unsigned short >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 4, short >::New( dim + 1, componentType );
      if( !filterTile2D3D ) filterTile2D3D = ITKToolsTileImages2D3D< 4, float >::New( dim + 1, componentType );
#endif

      /** Check if filter was instantiated. */
      bool supported = itktools::IsFilterSupportedCheck( filterTile2D3D, dim, componentType );
      if( !supported ) return EXIT_FAILURE;

      /** Set the filter arguments. */
      filterTile2D3D->m_InputFileNames = inputFileNames;
      filterTile2D3D->m_OutputFileName = outputFileName;
      filterTile2D3D->m_LastSpacing = lastSpacing;

      filterTile2D3D->Run();

      delete filterTile2D3D;
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
      delete filterTile2D3D;
      return EXIT_FAILURE;
    }
  }
  else
  {
    /** Class that does the work. */
    ITKToolsTileImagesBase * filter = nullptr;

    try
    {
      // now call all possible template combinations.
      if( !filter ) filter = ITKToolsTileImages< 2, unsigned char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 2, char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 2, unsigned short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 2, short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 2, float >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
      if( !filter ) filter = ITKToolsTileImages< 3, unsigned char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 3, char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 3, unsigned short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 3, short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 3, float >::New( dim, componentType );
#endif
#ifdef ITKTOOLS_4D_SUPPORT
      if( !filter ) filter = ITKToolsTileImages< 4, unsigned char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 4, char >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 4, unsigned short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 4, short >::New( dim, componentType );
      if( !filter ) filter = ITKToolsTileImages< 4, float >::New( dim, componentType );
#endif
      /** Check if filter was instantiated. */
      bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
      if( !supported ) return EXIT_FAILURE;

      /** Set the filter arguments. */
      filter->m_InputFileNames = inputFileNames;
      filter->m_OutputFileName = outputFileName;
      filter->m_Layout = layout;
      filter->m_Defaultvalue = defaultvalue;

      filter->Run();

      delete filter;
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
      delete filter;
      return EXIT_FAILURE;
    }
  }

  /** Return a value. */
  return EXIT_SUCCESS;

} // end main
