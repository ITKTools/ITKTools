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
 \brief Count non-zero voxels in an image.

 \verbinclude countnonzerovoxels.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxcountnonzerovoxels\n"
    << "  -in      inputFilename";
  return ss.str();

} // end GetHelpString()


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
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
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  // Some consts.
  const unsigned int  Dimension = 3;
  typedef short PixelType;

  // TYPEDEF's
  typedef itk::Image< PixelType, Dimension >          ImageType;
  typedef ImageType::SpacingType                      SpacingType;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageRegionConstIterator< ImageType >  IteratorType;

  /** Read image. */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: caught ITK exception while reading image "
      << inputFileName << "." << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  /** Get the spacing. */
  SpacingType sp = reader->GetOutput()->GetSpacing();
  double voxelVolume = 1.0;
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    voxelVolume *= sp[ i ];
  }

  /** Create iterator and counter. */
  IteratorType it( reader->GetOutput(),
    reader->GetOutput()->GetLargestPossibleRegion() );
  it.GoToBegin();
  std::size_t counter = 0;

  /** Walk over the image. */
  while ( !it.IsAtEnd() )
  {
    if( it.Value() )
    {
      counter++;
    }
    ++it;
  } // end while

  /** Print to screen. */
  std::cout << "count: " << counter << std::endl;
  std::cout << "volume: " << counter * voxelVolume / 1000.0 << std::endl;

  /** End program. Return a value. */
  return EXIT_SUCCESS;

} // end main
