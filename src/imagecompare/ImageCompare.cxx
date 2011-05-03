#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkNumericTraits.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkDifferenceImageFilter.h"

#include "itkImageSource.h" // This should not be necessary after ITK patch is merged
#include "itkTestingComparisonImageFilter.h"

// This comparison works on all image types by reading images in a 6D double images. If images > 6 dimensions
// must be compared, change this variable.
static const unsigned int ITK_TEST_DIMENSION_MAX = 6;

std::string GetHelpText( void );

int main( int argc, char **argv )
{
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpText());

  parser->MarkArgumentAsRequired( "-test", "The input filename." );
  parser->MarkArgumentAsRequired( "-base", "The baseline image filename." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
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
  baselineReader->Update();

  // Read the file to test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( testImageFileName );
  testReader->Update();

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
  comparisonFilter->Update();

  itk::SizeValueType numberOfDifferentPixels = comparisonFilter->GetNumberOfPixelsWithDifferences();

  if(numberOfDifferentPixels > 0)
    {
    std::cerr << "There are " << numberOfDifferentPixels << " different pixels!" << std::endl;
    return EXIT_FAILURE;
    }
    
  // If there are discrepencies, create a difference image
  typedef itk::DifferenceImageFilter<ImageType,ImageType>   DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput( baselineReader->GetOutput() );
  diff->SetTestInput( testReader->GetOutput() );
  diff->Update();

  typedef itk::ImageFileWriter<ImageType>                    WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( diff->GetOutput() );
  writer->Write();

  return EXIT_SUCCESS;

} // end main

std::string GetHelpText( void )
{
  std::string helpString = "Usage:\n\
  pximagecompare\n\
    -test      image filename to test against baseline\
    -base      baseline image filename";
  return helpString;
} // end PrintHelp

