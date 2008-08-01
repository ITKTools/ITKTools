
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"


	/**
	 * ******************* openingGrayscale *******************
	 */

template< class ImageType >
void openingGrayscale(
  const std::string & inputFileName,
  const std::string & outputFileName,
	const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition )
{
	/** Typedefs. */
  typedef typename ImageType::PixelType					      PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::ImageFileReader< ImageType >			      ReaderType;
	typedef itk::ImageFileWriter< ImageType >			      WriterType;
	typedef itk::BinaryBallStructuringElement<
		PixelType, Dimension >														StructuringElementType;
	typedef typename StructuringElementType::RadiusType	RadiusType;
	typedef itk::GrayscaleMorphologicalOpeningImageFilter<
		ImageType, ImageType, StructuringElementType >		OpeningFilterType;

	/** Declarations. */
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();
	typename OpeningFilterType::Pointer opening = OpeningFilterType::New();
	
	/** Setup the reader. */
	reader->SetFileName( inputFileName.c_str() );

	/** Create the structuring element. */
  RadiusType	radiusarray;
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		radiusarray.SetElement( i, radius[ i ] );
	}
  StructuringElementType	S_ball;
	S_ball.SetRadius( radiusarray );
	S_ball.CreateStructuringElement();
	
	/** Setup the opening filter. */
  opening->SetKernel( S_ball );
	opening->SetInput( reader->GetOutput() );

	/** Write the output image. */
	writer->SetFileName( outputFileName.c_str() );
	writer->SetInput( opening->GetOutput() );
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
  const std::vector<std::string> & bin )
{
	/** Typedefs. */
  typedef typename ImageType::PixelType					      PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
	typedef itk::ImageFileReader< ImageType >			      ReaderType;
	typedef itk::ImageFileWriter< ImageType >			      WriterType;
	typedef itk::BinaryBallStructuringElement<
		PixelType, Dimension >														StructuringElementType;
	typedef typename StructuringElementType::RadiusType	RadiusType;
	typedef itk::BinaryMorphologicalOpeningImageFilter<
		ImageType, ImageType, StructuringElementType >		OpeningFilterType;

	/** Declarations. */
	typename ReaderType::Pointer reader = ReaderType::New();
	typename WriterType::Pointer writer = WriterType::New();
	typename OpeningFilterType::Pointer opening = OpeningFilterType::New();
	
	/** Setup the reader. */
	reader->SetFileName( inputFileName.c_str() );

  /** Get foreground, background and erosion values. */
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
  RadiusType	radiusarray;
	for ( unsigned int i = 0; i < Dimension; i++ )
	{
		radiusarray.SetElement( i, radius[ i ] );
	}
  StructuringElementType	S_ball;
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
	writer->Update();

} // end openingBinary()

