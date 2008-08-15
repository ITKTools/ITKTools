

/** Include the right headers. */

#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkAndImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkThresholdLabelerImageFilter.h"
#include "itkNumericTraits.h"
#include <map>
#include <iostream>
#include <string>

/** \todo explain rationale.
 *
 *
 */
int main( int argc, char ** argv )
{
  /** Warning. */
  std::cout << "Warning: this program assumes 3d images with a pixeltype convertible to shorts!\n" << std::endl;

  /** Define image type. */
  const unsigned int Dimension = 3;
  typedef short PixelType;

  /** Some typedef's. */
  typedef itk::Image<PixelType, Dimension>            ImageType;
  typedef ImageType::Pointer                          ImagePointer;
  typedef itk::ImageFileReader<ImageType>             ImageReaderType;
  typedef ImageReaderType::Pointer                    ImageReaderPointer;
  typedef itk::AndImageFilter<
    ImageType, ImageType, ImageType>                  AndFilterType;
  typedef AndFilterType::Pointer                      AndFilterPointer;
  typedef itk::ImageRegionConstIterator<ImageType>    IteratorType;
  typedef itk::ThresholdLabelerImageFilter<
    ImageType, ImageType>                             ThresholdFilterType;
  typedef ThresholdFilterType::Pointer                ThresholdFilterPointer;
  typedef ThresholdFilterType::ThresholdVector        ThresholdVectorType;
  typedef std::map<std::string, std::string>          ArgMapType;

  /** Store the command line arguments. */
  ArgMapType argmap;

  /** Get and print the command line arguments. */
  std::cout << "ComputeOverlap called with the following arguments:" << std::endl;
  for ( unsigned int i = 1; i < argc; i += 2 )
  {
    std::string key = argv[ i ];
    std::string value = "";
    if ( ( i + 1 ) < argc )
    {
      value = argv[ i + 1 ];
    }
    std::cout << "  " << key << "\t" << value << std::endl;
    argmap[ key ] = value;
  }
  std::cout << std::endl;

  /** Print HELP if needed. */
  if (argmap.count("-help") | 
      argmap.count("--help") |
      argmap.count("-h") |
      argmap.count("--h") |
      argmap.count("-?") |
      !argmap.count("-im1") |
      !argmap.count("-im2")   )
  {
    std::cerr << "Compute the overlap of two binary images. Masks of the valid region\n"
              << "in the images (for example an US beam) are also taken into account.\n"
              << "If the image are not binary, you must specify a threshold value.\n";
    std::cerr << "\nUsage:\n";
    std::cerr << "pxcomputeoverlap\n"
              << "\t-im1 <image> -im2 <image>\n"
              << "\t[ -mask1 <mask-image> ] [ -mask2 <mask-image> ]\n"
              << "\t[ -t1 <threshold value> ] [ -t2 <threshold value> ]\n";
    std::cerr << "\nThe results is computed as:\n";
    std::cerr << " 2 * L1( (im1 AND mask2) AND (im2 AND mask1) )\n";
    std::cerr << "----------------------------------------------\n";
    std::cerr << "     L1(im1 AND mask2) + L1(im2 AND mask1)    \n" << std::endl;
    return 1;
  }

  /**
   * Setup pipeline 
   */

  /** Create readers and an AND filter. */
  ImageReaderPointer imreader1 = ImageReaderType::New();
  imreader1->SetFileName( argmap["-im1"].c_str() );
  ImageReaderPointer imreader2 = ImageReaderType::New();
  imreader2->SetFileName( argmap["-im2"].c_str() );
  AndFilterPointer finalANDFilter = AndFilterType::New();

  /** Create images, threshold filters, and threshold vectors. */
  ImagePointer im1 = 0;
  ImagePointer im2 = 0;
  ThresholdFilterPointer im1Thresholder = 0;
  ThresholdFilterPointer im2Thresholder = 0;
  ThresholdVectorType im1thresholdvector( 2 );
  ThresholdVectorType im2thresholdvector( 2 );

  /** If there is a threshold given for image1, use it. */
  if ( argmap.count("-t1") )
  {
    im1Thresholder = ThresholdFilterType::New();
    im1thresholdvector[ 0 ] = atoi( argmap[ "-t1" ].c_str() );
    im1thresholdvector[ 1 ] = itk::NumericTraits<PixelType>::max();
    im1Thresholder->SetThresholds( im1thresholdvector );
    im1Thresholder->SetInput( imreader1->GetOutput() );
    im1 = im1Thresholder->GetOutput();
  }
  /** Otherwise, just take the input image1. */
  else
  {
    im1 = imreader1->GetOutput();
  }

  /** If there is a threshold given for image2, use it. */
  if ( argmap.count("-t2") )
  {
    im2Thresholder = ThresholdFilterType::New();
    im2thresholdvector[ 0 ] = atoi( argmap[ "-t2" ].c_str() );
    im2thresholdvector[ 1 ] = itk::NumericTraits<PixelType>::max();
    im2Thresholder->SetThresholds( im2thresholdvector );
    im2Thresholder->SetInput( imreader2->GetOutput() );
    im2 = im2Thresholder->GetOutput();
  }
  /** Otherwise, just take the input image2. */
  else
  {
    im2 = imreader2->GetOutput();
  }

  /** Create readers for the masks and AND filters. */
  ImageReaderPointer maskreader1 = 0;
  ImageReaderPointer maskreader2 = 0;
  AndFilterPointer im2ANDmask1Filter = 0;
  AndFilterPointer im1ANDmask2Filter = 0;

  /** If there is a mask given for image1, use it on image2. */
  if ( argmap.count("-mask1") )
  {
    maskreader1 = ImageReaderType::New();
    maskreader1->SetFileName( argmap["-mask1"].c_str() );
    im2ANDmask1Filter = AndFilterType::New();
    im2ANDmask1Filter->SetInput1( im2 );
    im2ANDmask1Filter->SetInput2( maskreader1->GetOutput() );
    finalANDFilter->SetInput1( im2ANDmask1Filter->GetOutput() );
  }
  /** Otherwise, just use image2. */
  else
  {
    finalANDFilter->SetInput1( im2 );
  }

  /** If there is a mask given for image2, use it on image1. */
  if ( argmap.count("-mask2") )
  {
    maskreader2 = ImageReaderType::New();
    maskreader2->SetFileName( argmap["-mask2"].c_str() );
    im1ANDmask2Filter = AndFilterType::New();
    im1ANDmask2Filter->SetInput1( im1 );
    im1ANDmask2Filter->SetInput2( maskreader2->GetOutput() );
    finalANDFilter->SetInput2( im1ANDmask2Filter->GetOutput() );
  }
  /** Otherwise, just use image1. */
  else
  {
    finalANDFilter->SetInput2( im1 );
  }

  /** UPDATE!
   *
   * Call the update of the final filter, in order to execute the whole pipeline.
   */

  try
  {
    finalANDFilter->Update();
  }
  catch (itk::ExceptionObject & err)
  {
    std::cerr << err << std::endl;
    throw err;
  }

  /**
   * Now calculate the L1-norm.
   */

  /** Create iterators. */
  IteratorType iteratorA( finalANDFilter->GetInput(1),
    finalANDFilter->GetInput(1)->GetLargestPossibleRegion() );
  IteratorType iteratorB( finalANDFilter->GetInput(0),
    finalANDFilter->GetInput(0)->GetLargestPossibleRegion() );
  IteratorType iteratorC( finalANDFilter->GetOutput(),
    finalANDFilter->GetOutput()->GetLargestPossibleRegion() );

  /** Determine size of first object. */
  long long sumA = 0;
  for ( iteratorA.GoToBegin(); !iteratorA.IsAtEnd(); ++iteratorA )
  {
    if ( iteratorA.Value() )
    {
      ++sumA;
    }
  }
  std::cout << "Size of first object: " << sumA << std::endl;

  /** Determine size of second object. */
  long long sumB = 0;
  for ( iteratorB.GoToBegin(); !iteratorB.IsAtEnd(); ++iteratorB )
  {
    if ( iteratorB.Value() )
    {
      ++sumB;
    }
  }
  std::cout << "Size of second object: " << sumB << std::endl;

  /** Determine size of cross-section. */
  long long sumC = 0;
  for ( iteratorC.GoToBegin(); !iteratorC.IsAtEnd(); ++iteratorC )
  {
    if ( iteratorC.Value() )
    {
      ++sumC;
    }
  }
  std::cout << "Size of cross-section of both objects: " << sumC << std::endl;

  /** Calculate the overlap. */
  double overlap;
  if ( ( sumA + sumB ) == 0 )
  {
    overlap = 0;
  }
  else
  {
    overlap = static_cast<double>( 2 * sumC ) / static_cast<double>( sumA + sumB );
  }

  /** Format the output and show overlap. */
  std::cout << std::fixed << std::showpoint;
  std::cout << "Overlap: " << overlap << std::endl;

  /** Return a value. */
  return 0;

} // end main


