
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkParabolicOpenImageFilter.h"


/**
 * ******************* openingGrayscale *******************
 */

template< class ImageType >
void openingGrayscale(
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
  typedef itk::GrayscaleMorphologicalOpeningImageFilter<
    ImageType, ImageType, StructuringElementType >    OpeningFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename OpeningFilterType::Pointer opening = OpeningFilterType::New();

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

  /** Setup the opening filter. */
  opening->SetKernel( S_ball );
  opening->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( opening->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end openingGrayscale()


  /**
   * ******************* openingBinary *******************
   */

template< class ImageType >
void openingBinary(
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
  typedef itk::BinaryMorphologicalOpeningImageFilter<
    ImageType, ImageType, StructuringElementType >    OpeningFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename OpeningFilterType::Pointer opening = OpeningFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get foreground, background and opening values. */
  std::vector<PixelType> values( 2 );
  values[ 0 ] = itk::NumericTraits<PixelType>::One;
  values[ 1 ] = itk::NumericTraits<PixelType>::Zero;
  if ( bin.size() == 2 )
  {
    for ( unsigned int i = 0; i < 2; ++i )
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

  /** Setup the opening filter. */
  opening->SetForegroundValue( values[ 0 ] );
  opening->SetBackgroundValue( values[ 1 ] );
  opening->SetKernel( S_ball );
  opening->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( opening->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end openingBinary()


/**
 * ******************* openingParabolic *******************
 */

template< class ImageType >
void openingParabolic(
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
  typedef itk::ParabolicOpenImageFilter<
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

} // end openingParabolic()

