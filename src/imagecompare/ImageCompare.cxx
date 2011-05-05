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
 \brief Compare two images.
 
 \verbinclude imagecompare.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkNumericTraits.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkDifferenceImageFilter.h"
#include "itksys/SystemTools.hxx"

#include "itkImageSource.h" // This should not be necessary after ITK patch is merged
#include "itkTestingComparisonImageFilter.h"

// This comparison works on all image types by reading images in a 6D double images. If images > 6 dimensions
// must be compared, change this variable.
static const unsigned int ITK_TEST_DIMENSION_MAX = 6;

std::string GetHelpString( void );

int main( int argc, char **argv )
{
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-test", "The input filename." );
  parser->MarkArgumentAsRequired( "-base", "The baseline image filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  std::string testImageFileName;
  parser->GetCommandLineArgument( "-test", testImageFileName );

  std::string baselineImageFileName;
  parser->GetCommandLineArgument( "-base", baselineImageFileName );

  // Read images
  typedef itk::Image<double,ITK_TEST_DIMENSION_MAX>           ImageType;
  typedef itk::ImageFileReader<ImageType>                     ReaderType;

  // Read the baseline file
  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName( baselineImageFileName );
  try
  {
    baselineReader->Update();
  }
  catch ( itk::ExceptionObject & err )
  {
    std::cerr << "Error during reading baseline image: " << err << std::endl;
    return EXIT_FAILURE;
  }

  // Read the file to test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( testImageFileName );
  try
  {
    testReader->Update();
  }
  catch ( itk::ExceptionObject & err )
  {
    std::cerr << "Error during reading test image: " << err << std::endl;
    return EXIT_FAILURE;
  }

  // The sizes of the baseline and test image must match
  ImageType::SizeType baselineSize;
  baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  ImageType::SizeType testSize;
  testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( baselineSize != testSize )
  {
    std::cerr << "The size of the Baseline image and Test image do not match!" << std::endl;
    std::cerr << "Baseline image: " << baselineImageFileName
      << " has size " << baselineSize << std::endl;
    std::cerr << "Test image:     " << testImageFileName
      << " has size " << testSize << std::endl;
    return EXIT_FAILURE;
  }

  // Now compare the two images
  typedef itk::Testing::ComparisonImageFilter< ImageType, ImageType > ComparisonFilterType;
  ComparisonFilterType::Pointer comparisonFilter = ComparisonFilterType::New();
  comparisonFilter->SetTestInput(testReader->GetOutput());
  comparisonFilter->SetValidInput(baselineReader->GetOutput());
  try
  {
    comparisonFilter->Update();
  }
  catch ( itk::ExceptionObject & err )
  {
    std::cerr << "Error during comparing image: " << err << std::endl;
    return EXIT_FAILURE;
  }

  itk::SizeValueType numberOfDifferentPixels = comparisonFilter->GetNumberOfPixelsWithDifferences();

  if(numberOfDifferentPixels > 0)
  {
    std::cerr << "There are " << numberOfDifferentPixels << " different pixels!" << std::endl;
    
    // If there are discrepencies, create a difference image
    typedef itk::DifferenceImageFilter<ImageType,ImageType>   DiffType;
    DiffType::Pointer diff = DiffType::New();
    diff->SetValidInput( baselineReader->GetOutput() );
    diff->SetTestInput( testReader->GetOutput() );
    try
    {
      diff->Update();
    }
    catch ( itk::ExceptionObject & err )
    {
      std::cerr << "Error during computing difference image: " << err << std::endl;
      return EXIT_FAILURE;
    }

    // Create name for diff image
    std::string diffImageFileName =
      itksys::SystemTools::GetFilenameWithoutLastExtension( testImageFileName );
    diffImageFileName += "_DIFF";
    diffImageFileName += itksys::SystemTools::GetFilenameLastExtension( testImageFileName );

    typedef itk::ImageFileWriter<ImageType>                    WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( diffImageFileName );
    writer->SetInput( diff->GetOutput() );
    try
    {
      writer->Write();
    }
    catch ( itk::ExceptionObject & err )
    {
      std::cerr << "Error during writing difference image: " << err << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_FAILURE;

  } // end if discrepancies

  return EXIT_SUCCESS;

} // end main

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pximagecompare" << std::endl
  << "  -test      image filename to test against baseline" << std::endl
  << "  -base      baseline image filename";
  return ss.str();
} // end GetHelpString

