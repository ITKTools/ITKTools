
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkParabolicCloseImageFilter.h"


/**
 * ******************* closingGrayscale *******************
 */

template< class ImageType >
void closingGrayscale(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition,
	const bool useCompression)
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::GrayscaleMorphologicalClosingImageFilter<
    ImageType, ImageType, StructuringElementType >    OpeningFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename OpeningFilterType::Pointer closing = OpeningFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the closing filter. */
  closing->SetKernel( S_ball );
  closing->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( closing->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end closingGrayscale()


  /**
   * ******************* closingBinary *******************
   */

template< class ImageType >
void closingBinary(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
	const bool useCompression)
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::BinaryMorphologicalClosingImageFilter<
    ImageType, ImageType, StructuringElementType >    OpeningFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename OpeningFilterType::Pointer closing = OpeningFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get foreground, background and erosion values. */
  std::vector<PixelType> values( 1 );
  values[ 0 ] = itk::NumericTraits<PixelType>::One;
  if ( bin.size() == 1 )
  {
    for ( unsigned int i = 0; i < 1; ++i )
    {
      if ( itk::NumericTraits<PixelType>::is_integer )
      {
        values[ i ] = static_cast<PixelType>( atoi( bin[ i ].c_str() ) );
      }
      else
      {
        values[ i ] = static_cast<PixelType>( atof( bin[ i ].c_str() ) );
      }
    }
  }

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the closing filter. */
  closing->SetForegroundValue( values[ 0 ] );
  closing->SetKernel( S_ball );
  closing->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( closing->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end closingBinary()


/**
 * ******************* closingParabolic *******************
 */

template< class ImageType >
void closingParabolic(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
	const bool useCompression)
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::ParabolicCloseImageFilter<
    ImageType, ImageType >                            FilterType;
  typedef typename FilterType::RadiusType             RadiusType;
  typedef typename FilterType::ScalarRealType         ScalarRealType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename FilterType::Pointer filter = FilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get the correct radius. */
  RadiusType      radiusArray;
  ScalarRealType  radius1D = 0.0;
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    // Very specific computation for the parabolic filter:
    radius1D = static_cast<ScalarRealType>( radius[ i ] ) ;//+ 1.0;
    radius1D = radius1D * radius1D / 2.0 + 1.0;
    radiusArray.SetElement( i, radius1D );
  }

  /** Setup the filter. */
  filter->SetUseImageSpacing( false );
  filter->SetScale( radiusArray );
  filter->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( filter->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end closingParabolic()

