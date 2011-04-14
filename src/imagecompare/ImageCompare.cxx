#include "itkWin32Header.h"
#include <iostream>
#include <fstream>
#include "itkNumericTraits.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkDifferenceImageFilter.h"

/** Define maximum dimension of an image. */
#define ITK_TEST_DIMENSION_MAX 6

/** Declare function to compare two images. */
int RegressionTestImage (const char *, const char *, int, double, int );


int main( int argc, char **argv )
{
  /** Check arguments. */
  if( argc < 2 )
  {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "pximagecompare [-t threshold] [-r radius] testImage baselineImage1 [baselineImage2 baselineImage3 ...]" << std::endl;
    std::cerr << "Note that if you supply more than one baselineImage, this test will pass if any" << std::endl;
    std::cerr << "of them match the testImage." << std::endl;
    return -1;
  }

  /** Defaults for the difference filter. */
  double differenceThreshold = 2.0;
  int toleranceRadius = 0;

  /** Get the command line arguments. */
  bool testImageFound = false;
  char * testImage;
  bool baselineImageStartIndexFound = false;
  unsigned int baselineImageStartIndex = 2;
  unsigned int baselineImageEndIndex = 2;
  for ( unsigned int i = 1; i < static_cast<unsigned int>(argc); i++ )
  {
    /** Check for the difference threshold. */
    if ( static_cast<std::string>( argv[ i ] ) == "-t" )
    {
      differenceThreshold = atof( argv[ i + 1 ] );
      i++;
    }
    /** Check for the difference tolerance radius. */
    else if ( static_cast<std::string>( argv[ i ] ) == "-r" )
    {
      toleranceRadius = atoi( argv[ i + 1 ] );
      i++;
      if ( toleranceRadius > 0 )
      {
        std::cout << "WARNING: setting the tolerance radius to values "
          << "larger than 0, will result in very (!) large computation time, "
          << "since a 6D neighborhood will have to be traversed." << std::endl;
      }
    }
    else
    {
      /** Get the testImage. */
      if ( !testImageFound )
      {
        testImage = argv[ i ];
        testImageFound = true;
      }
      /** Get the starting baselineImage. */
      else if ( !baselineImageStartIndexFound )
      {
        baselineImageStartIndex = i;
        baselineImageEndIndex = i;
        baselineImageStartIndexFound = true;
      }
      /** Get the ending baselineImage. */
      else if ( baselineImageStartIndexFound )
      {
        baselineImageEndIndex = i;
      }
    }
  }

  /** Print arguments. */
  std::cout << "pximagecompare called with the following arguments:" << std::endl;
  std::cout << "differenceThreshold:\t" << differenceThreshold << std::endl;
  std::cout << "toleranceRadius:\t" << toleranceRadius << std::endl;
  std::cout << "testImage:\t\t" << testImage << std::endl;
  if ( baselineImageStartIndex == baselineImageEndIndex )
  {
    std::cout << "baselineImage:\t\t" << argv[ baselineImageStartIndex ] << std::endl;
  }
  else if ( baselineImageEndIndex - baselineImageStartIndex == 1 )
  {
    std::cout << "baselineImages:\t\t{"
      << argv[ baselineImageStartIndex ] << ", "
      << argv[ baselineImageStartIndex + 1 ] << "}" << std::endl;
  }
  else
  {
    std::cout << "baselineImages:\t\t{"
      << argv[ baselineImageStartIndex ] << ", ..., "
      << argv[ baselineImageEndIndex ] << "}" << std::endl;
  }

  /** Run the comparison(s). */
  int bestBaselineStatus = 2001;
  int bestBaseline = 2;
  try
  {
    if( baselineImageStartIndex == baselineImageEndIndex )
    {
      bestBaselineStatus = RegressionTestImage(
        testImage, argv[ baselineImageStartIndex ], 0, differenceThreshold, toleranceRadius );
    }
    else
    {
      int currentStatus = 2001;
      for ( unsigned int i = baselineImageStartIndex; i < baselineImageEndIndex; i++ )
      {
        currentStatus = RegressionTestImage(
          testImage, argv[ i ], 0, differenceThreshold, toleranceRadius );
        if( currentStatus < bestBaselineStatus )
        {
          bestBaselineStatus = currentStatus;
          bestBaseline = i;
        }
        if( bestBaselineStatus == 0 )
        {
          break;
        }
      }
    }
    // if the best we can do still has errors, generate the error images
    if ( bestBaselineStatus )
    {
      RegressionTestImage( testImage, argv[ bestBaseline ], 1,
        differenceThreshold, toleranceRadius );
    }
  } // end try
  catch( const itk::ExceptionObject& e )
  {
    std::cerr << "ITK test driver caught an ITK exception:\n";
    std::cerr << e.GetFile() << ":" << e.GetLine() << ":\n"
              << e.GetDescription() << "\n";
    bestBaselineStatus = -1;
  }
  catch( const std::exception& e )
  {
    std::cerr << "ITK test driver caught an exception:\n";
    std::cerr << e.what() << "\n";
    bestBaselineStatus = -1;
   }
  catch(...)
  {
    std::cerr << "ITK test driver caught an unknown exception!!!\n";
    bestBaselineStatus = -1;
  }

  /** Return a value. */
  std::cout << bestBaselineStatus << std::endl;
  return bestBaselineStatus;

} // end main


// Regression Testing Code

int RegressionTestImage( const char *testImageFilename,
  const char *baselineImageFilename, int reportErrors,
  double differenceThreshold, int toleranceRadius )
{
  // Use the factory mechanism to read the test and baseline files and convert them to double
  typedef itk::Image<double,ITK_TEST_DIMENSION_MAX>           ImageType;
  typedef itk::Image<unsigned char,ITK_TEST_DIMENSION_MAX>    OutputType;
  typedef itk::Image<unsigned char,2>                         DiffOutputType;
  typedef itk::ImageFileReader<ImageType>                     ReaderType;

  // Read the baseline file
  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName( baselineImageFilename );
  try
  {
    baselineReader->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject& e)
  {
    std::cerr << "Exception detected while reading "
      << baselineImageFilename
      << " : "  << e.GetDescription() << std::endl;
    return 1000;
  }

  // Read the file generated by the test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( testImageFilename );
  try
  {
    testReader->UpdateLargestPossibleRegion();
  }
  catch ( itk::ExceptionObject& e )
  {
    std::cerr << "Exception detected while reading "
      << testImageFilename
      << " : "  << e.GetDescription() << std::endl;
    return 1000;
  }

  // The sizes of the baseline and test image must match
  ImageType::SizeType baselineSize;
  baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  ImageType::SizeType testSize;
  testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( baselineSize != testSize )
  {
    std::cerr << "The size of the Baseline image and Test image do not match!" << std::endl;
    std::cerr << "Baseline image: " << baselineImageFilename
      << " has size " << baselineSize << std::endl;
    std::cerr << "Test image:     " << testImageFilename
      << " has size " << testSize << std::endl;
    return 1;
  }

  // Now compare the two images
  typedef itk::DifferenceImageFilter<ImageType,ImageType>   DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput( baselineReader->GetOutput() );
  diff->SetTestInput( testReader->GetOutput() );
  diff->SetDifferenceThreshold( differenceThreshold );
  diff->SetToleranceRadius( toleranceRadius );
  diff->UpdateLargestPossibleRegion();

  double status = diff->GetTotalDifference();

  // if there are discrepencies, create a diff image
  if ( status && reportErrors )
  {
    typedef itk::RescaleIntensityImageFilter<ImageType,OutputType>  RescaleType;
    typedef itk::ExtractImageFilter<OutputType,DiffOutputType>      ExtractType;
    typedef itk::ImageFileWriter<DiffOutputType>                    WriterType;
    typedef itk::ImageRegion<ITK_TEST_DIMENSION_MAX>                RegionType;
    OutputType::IndexType index; index.Fill(0);
    OutputType::SizeType size; size.Fill(0);

    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum( itk::NumericTraits<unsigned char>::NonpositiveMin() );
    rescale->SetOutputMaximum( itk::NumericTraits<unsigned char>::max() );
    rescale->SetInput( diff->GetOutput() );
    rescale->UpdateLargestPossibleRegion();

    RegionType region;
    region.SetIndex( index );

    size = rescale->GetOutput()->GetLargestPossibleRegion().GetSize();
    for ( unsigned int i = 2; i < ITK_TEST_DIMENSION_MAX; i++ )
    {
      size[ i ] = 0;
    }
    region.SetSize( size );

    ExtractType::Pointer extract = ExtractType::New();
    extract->SetInput( rescale->GetOutput() );
    extract->SetExtractionRegion(region);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( extract->GetOutput() );

    std::cout << "<DartMeasurement name=\"ImageError\" type=\"numeric/double\">";
    std::cout << status;
    std::cout <<  "</DartMeasurement>" << std::endl;

    std::ostringstream diffName;
    diffName << testImageFilename << ".diff.png";

    /** Rescale difference image. */
    try
    {
      rescale->SetInput( diff->GetOutput() );
      rescale->Update();
    }
    catch (...)
    {
      std::cerr << "Error during rescale of " << diffName.str() << std::endl;
    }

    /** Write extracted difference image slice. */
    writer->SetFileName( diffName.str().c_str() );
    try
    {
      writer->Update();
    }
    catch (...)
    {
      std::cerr << "Error during write of " << diffName.str() << std::endl;
    }

    /** Dart output. */
    std::cout << "<DartMeasurementFile name=\"DifferenceImage\" type=\"image/png\">";
    std::cout << diffName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    /** Rescale baseline image. */
    std::ostringstream baseName;
    baseName << testImageFilename << ".base.png";
    try
    {
      rescale->SetInput(baselineReader->GetOutput());
      rescale->Update();
    }
    catch (...)
    {
      std::cerr << "Error during rescale of " << baseName.str() << std::endl;
    }

    /** Write extracted baseline image slice. */
    try
    {
      writer->SetFileName(baseName.str().c_str());
      writer->Update();
    }
    catch (...)
    {
      std::cerr << "Error during write of " << baseName.str() << std::endl;
    }

    /** Dart output. */
    std::cout << "<DartMeasurementFile name=\"BaselineImage\" type=\"image/png\">";
    std::cout << baseName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    /** Rescale test image. */
    std::ostringstream testName;
    testName << testImageFilename << ".test.png";
    try
    {
      rescale->SetInput(testReader->GetOutput());
      rescale->Update();
    }
    catch (...)
    {
      std::cerr << "Error during rescale of " << testName.str()
        << std::endl;
    }

    /** Write extracted test image slice. */
    try
    {
      writer->SetFileName(testName.str().c_str());
      writer->Update();
    }
    catch (...)
    {
      std::cerr << "Error during write of " << testName.str() << std::endl;
    }

    /** Dart output. */
    std::cout << "<DartMeasurementFile name=\"TestImage\" type=\"image/png\">";
    std::cout << testName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

  } // end if

  /** Return a value. */
  return ( status != 0 ) ? 1 : 0;

} // end RegressionTestImage

