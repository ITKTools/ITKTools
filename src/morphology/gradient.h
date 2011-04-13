
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkFlatStructuringElement.h"
#include "itkMorphologicalGradientImageFilter.h"


/**
 * ******************* gradient *******************
 */

template< class ImageType >
void gradient(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const int & algorithm,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;

  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::FlatStructuringElement< Dimension >    KernelType;
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

  KernelType structuringElement;
  structuringElement = structuringElement.Box( radiusarray );

  /** Setup the gradient filter. BASIC = 0, HISTO = 1, ANCHOR = 2, VHGW = 3. */
  filter->SetKernel( structuringElement );
  filter->SetAlgorithm( algorithm );
  filter->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( filter->GetOutput() );
	writer->SetUseCompression( useCompression );
  writer->Update();

} // end gradient()
