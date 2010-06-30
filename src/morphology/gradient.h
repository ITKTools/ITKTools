
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

/*#include "itkBinaryBallStructuringElement.h"*/
#include "itkNeighborhood.h"
#include "itkMorphologicalGradientImageFilter.h"


/**
 * ******************* erosionGrayscale *******************
 */

template< class ImageType >
void gradient( 
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const int & algorithm )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;

  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
//   typedef itk::BinaryBallStructuringElement<
//     PixelType, Dimension >                            StructuringElementType;
  //typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::Neighborhood< PixelType, Dimension >   KernelType;
  typedef itk::MorphologicalGradientImageFilter<
    ImageType, ImageType, KernelType >    FilterType;
  typedef typename FilterType::RadiusType             RadiusType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename FilterType::Pointer filter = FilterType::New();
  
  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
//   StructuringElementType  S_ball;
//   S_ball.SetRadius( radiusarray );
//   S_ball.CreateStructuringElement();

  /** Setup the gradient filter. BASIC = 0, HISTO = 1, ANCHOR = 2, VHGW = 3. */
  //filter->SetKernel( S_ball );
  filter->SetRadius( radiusarray );
  filter->SetAlgorithm( algorithm );
  filter->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end gradient()
