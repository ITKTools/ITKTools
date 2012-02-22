
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkErodeObjectMorphologyImageFilter.h"
#include "itkParabolicErodeImageFilter.h"


/**
 * ******************* erosionGrayscale *******************
 */

template< class ImageType >
void erosionGrayscale(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::GrayscaleErodeImageFilter<
    ImageType, ImageType, StructuringElementType >    ErodeFilterType;
  typedef typename
    ErodeFilterType::DefaultBoundaryConditionType     BoundaryConditionType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename ErodeFilterType::Pointer erosion = ErodeFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Set a boundary condition value.
   * This is the value outside the image.
   * By default it is set to max(PixelType).
   */
  
  PixelType boundaryValue = itk::NumericTraits<PixelType>::max();
  if( boundaryCondition != "" )
  {
    if( itk::NumericTraits<PixelType>::is_integer )
    {
      boundaryValue = static_cast<PixelType>( atoi( boundaryCondition.c_str() ) );
    }
    else
    {
      boundaryValue = static_cast<PixelType>( atof( boundaryCondition.c_str() ) );
    }
    
    erosion->SetBoundary( boundaryValue );
  }

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the erosion filter. */
  erosion->SetKernel( S_ball );
  erosion->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( erosion->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end erosionGrayscale()


/**
 * ******************* erosionBinary *******************
 */

template< class ImageType >
void erosionBinary(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::BinaryErodeImageFilter<
    ImageType, ImageType, StructuringElementType >    ErodeFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename ErodeFilterType::Pointer erosion = ErodeFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get foreground, background and erosion values. */
  std::vector<PixelType> values( 3 );
  values[ 0 ] = itk::NumericTraits<PixelType>::One;
  values[ 1 ] = itk::NumericTraits<PixelType>::Zero;
  values[ 2 ] = itk::NumericTraits<PixelType>::One;
  if( bin.size() == 3 )
  {
    for( unsigned int i = 0; i < 3; ++i )
    {
      if( itk::NumericTraits<PixelType>::is_integer )
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
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the erosion filter. */
  erosion->SetForegroundValue( values[ 0 ] );
  erosion->SetBackgroundValue( values[ 1 ] );
  erosion->SetErodeValue( values[ 2 ] );
  erosion->SetBoundaryToForeground( false );
  erosion->SetKernel( S_ball );
  erosion->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( erosion->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end erosionBinary()


/**
 * ******************* erosionBinaryObject *******************
 * Don't use for now, because it does not give output, consistent
 * with the grayscale and binary.
 */

template< class ImageType >
void erosionBinaryObject(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::ErodeObjectMorphologyImageFilter<
    ImageType, ImageType, StructuringElementType >    ErodeFilterType;
  typedef typename
    ErodeFilterType::DefaultBoundaryConditionType     BoundaryConditionType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename ErodeFilterType::Pointer erosion = ErodeFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Create and fill the radius. */
  RadiusType  radiusarray;
  //radiusarray.Fill( 1 );
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }

  /** Create the structuring element and set it into the erosion filter. */
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();
  erosion->SetKernel( S_ball );

  /** Set a boundary condition value.
   * This is the value outside the image.
   * By default it is set to max(PixelType).
   */
  BoundaryConditionType bc;
  PixelType bcValue = itk::NumericTraits<PixelType>::max();
  if( boundaryCondition != "" )
  {
    if( itk::NumericTraits<PixelType>::is_integer )
    {
      bcValue = static_cast<PixelType>( atoi( boundaryCondition.c_str() ) );
    }
    else
    {
      bcValue = static_cast<PixelType>( atof( boundaryCondition.c_str() ) );
    }
    bc.SetConstant( bcValue );
    erosion->OverrideBoundaryCondition( &bc );
  }

  /** Connect the pipeline. */
  erosion->SetBackgroundValue( 0 );
  erosion->SetObjectValue( 1 );
  //erosion->SetErodeValue( 1 );
  erosion->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( erosion->GetOutput() );
  writer->Update();

} // end erosionBinaryObject



/**
 * ******************* erosionParabolic *******************
 */

template< class ImageType >
void erosionParabolic(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::ParabolicErodeImageFilter<
    ImageType, ImageType >                            ErodeFilterType;
  typedef typename ErodeFilterType::RadiusType        RadiusType;
  typedef typename ErodeFilterType::ScalarRealType    ScalarRealType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename ErodeFilterType::Pointer erosion = ErodeFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get the correct radius. */
  RadiusType      radiusArray;
  ScalarRealType  radius1D = 0.0;
  for( unsigned int i = 0; i < Dimension; ++i )
  {
    // Very specific computation for the parabolic filter:
    radius1D = static_cast<ScalarRealType>( radius[ i ] ) ;//+ 1.0;
    radius1D = radius1D * radius1D / 2.0 + 1.0;
    radiusArray.SetElement( i, radius1D );
  }

  /** Setup the filter. */
  erosion->SetUseImageSpacing( false );
  erosion->SetScale( radiusArray );
  erosion->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( erosion->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end erosionParabolic()

