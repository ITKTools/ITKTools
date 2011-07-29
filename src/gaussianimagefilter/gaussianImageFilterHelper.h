#ifndef __gaussianImageFilterHelper_h
#define __gaussianImageFilterHelper_h

#include "itkImageFileReader.h"
#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkGaussianInvariantsImageFilter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GaussianImageFilter *******************
 *
 * This function performs Gaussian smoothing of an input image.
 * Several so-called orders are supported:
 * 0: plain Gaussian smoothing
 * 1: first derivative
 * 2: second derivative
 * The input is scalar, and so is the output.
 */

template< class OutputImageType >
void GaussianImageFilter(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, OutputImageType >                     FilterType;
  typedef typename FilterType::OrderType                  OrderType;
  typedef typename FilterType::SigmaType                  SigmaType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup the order and sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma[ 0 ] );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    orderFA[ i ] = order[ i ];
    if ( sigma.size() == Dimension ) sigmaFA[ i ] = sigma[ i ];
  }

  /** Setup the smoothing filter. */
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNormalizeAcrossScale( false );
  filter->SetInput( reader->GetOutput() );
  filter->SetSigma( sigmaFA );
  filter->SetOrder( orderFA );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end GaussianImageFilter()


/**
 * ******************* GaussianImageFilterMagnitude *******************
 *
 * This function computes the magnitude of smoothed versions of an image,
 * where the input is processed per channel. First a vector image is
 * constructed as follows:
 *
 *   vecImage = [ G^i_x( image ) G^i_y( image ) G^i_z( image ) ]^T
 *
 * where i refers to the order (zero = smoothing, 1 = first derivative,
 * 2 = second derivative ], and where x,y,z refer to the image direction
 * in which smoothing is performed. After construction of the vector image
 * the magnitude is taken per voxel: || vecImage(x) ||.
 */

template< class OutputImageType >
void GaussianImageFilterMagnitude(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::vector<unsigned int> & order )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, InputImageType >                      SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;
  typedef typename SmoothingFilterType::OrderType         OrderType;
  typedef typename SmoothingFilterType::SigmaType         SigmaType;
  typedef itk::ImageToVectorImageFilter<
    InputImageType >                                      ImageToVectorImageFilterType;
  typedef typename ImageToVectorImageFilterType
    ::OutputImageType                                     VectorImageType;
  typedef itk::GradientToMagnitudeImageFilter<
    VectorImageType, OutputImageType >                    MagnitudeFilterType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup the order and sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma[ 0 ] );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    orderFA[ i ] = order[ i ];
    if ( sigma.size() == Dimension ) sigmaFA[ i ] = sigma[ i ];
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( Dimension );
  typename ImageToVectorImageFilterType::Pointer composeFilter
    = ImageToVectorImageFilterType::New();
  typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    /** Setup smoothing filter. */
    OrderType order2; order2.Fill( 0 ); order2[ i ] = orderFA[ i ];

    smoothingFilter[ i ] = SmoothingFilterType::New();
    smoothingFilter[ i ]->SetInput( reader->GetOutput() );
    smoothingFilter[ i ]->SetNormalizeAcrossScale( false );
    smoothingFilter[ i ]->SetSigma( sigmaFA );
    smoothingFilter[ i ]->SetOrder( order2 );
    smoothingFilter[ i ]->Update();

    /** Setup composition filter. */
    composeFilter->SetNthInput( i, smoothingFilter[ i ]->GetOutput() );
  }

  /** Compose vector image and compute magnitude. */
  magnitudeFilter->SetInput( composeFilter->GetOutput() );
  magnitudeFilter->Update();

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( magnitudeFilter->GetOutput() );
  writer->Update();

} // end GaussianImageFilterMagnitude()


/**
 * ******************* GaussianImageFilterLaplacian *******************
 *
 * This function computes the Laplacian of an image using Gaussian derivatives.
 *
 *   L( image I ) = sum_i d^2 I / d x_i^2
 *
 * i being the direction.
 * This is actually quite similar to the function GaussianImageFilterMagnitude,
 * defined above, with order = [ 2 2 2 ]. But instead of using a magnitude filter,
 * a square magnitude should be used: the Laplacian computes the sum of squares,
 * while the vector magnitude computes the squareroot of the sum of squares.
 */

template< class OutputImageType >
void GaussianImageFilterLaplacian(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef typename OutputImageType::PixelType             OutputPixelType;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, InputImageType >                      SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;
  typedef typename SmoothingFilterType::OrderType         OrderType;
  typedef typename SmoothingFilterType::SigmaType         SigmaType;
  typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< OutputImageType >     IteratorType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup sigma. */
  SigmaType sigmaFA; sigmaFA.Fill( sigma[ 0 ] );
  if ( sigma.size() == Dimension )
  {
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      sigmaFA[ i ] = sigma[ i ];
    }
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( Dimension );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    /** Setup smoothing filter. */
    OrderType order; order.Fill( 0 ); order[ i ] = 2;
    smoothingFilter[ i ] = SmoothingFilterType::New();
    smoothingFilter[ i ]->SetInput( reader->GetOutput() );
    smoothingFilter[ i ]->SetNormalizeAcrossScale( false );
    smoothingFilter[ i ]->SetSigma( sigmaFA );
    smoothingFilter[ i ]->SetOrder( order );
    smoothingFilter[ i ]->Update();
  }

  /** Create output image. */
  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->CopyInformation( reader->GetOutput() );
  outputImage->SetRegions( reader->GetOutput()->GetLargestPossibleRegion() );
  outputImage->Allocate();

  /** Setup iterators. */
  std::vector< ConstIteratorType > itIn( Dimension );
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    itIn[ i ] = ConstIteratorType( smoothingFilter[ i ]->GetOutput(),
      smoothingFilter[ i ]->GetOutput()->GetLargestPossibleRegion() );
    itIn[ i ].GoToBegin();
  }
  IteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );
  itOut.GoToBegin();

  /** Fill the output image by adding the second order derivatives. */
  while ( !itOut.IsAtEnd() )
  {
    InputPixelType value = itk::NumericTraits<InputPixelType>::Zero;
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      value += itIn[ i ].Get();
      ++itIn[ i ];
    }
    itOut.Set( static_cast<OutputPixelType>( value ) );
    ++itOut;
  }

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( outputImage );
  writer->Update();

} // end GaussianImageFilterLaplacian()


/**
 * ******************* GaussianImageFilterInvariants *******************
 *
 * This function computes some invariants based on Gaussian derivatives.
 *
 * It can compute one of the following invariants:
 *   {LiLi, LiLijLj, LiLijLjkLk, Lii, LijLji, LijLjkLki }
 * where L is the input image, and using Einstein notation.
 * Together they form the irreducible set of second order Cartesian structure
 * invariants. They can also be written in matrix notation:
 *
 * Einstein notation     matrix notation
 * ----------------------------------------
 * L                     L
 * L_iL_i                g^T g
 * L_iL_{ij}L_j          g^T H g
 * L_iL_{ij}L_{jk}L_k    g^T H H g
 * L_{ii}                trace(H)
 * L_{ij}L_{ji}          trace(H H)
 * L_{ij}L_{jk}L_{ki}    trace(H H H)
 *
 * where g is the gradient and H the Hessian, both computed using Gaussian
 * derivatives at scale sigma.
 */

template< class OutputImageType >
void GaussianImageFilterInvariants(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<float> & sigma,
  const std::string & invariant )
{
  /** Typedef's. */
  const unsigned int Dimension = OutputImageType::ImageDimension;
  typedef typename OutputImageType::PixelType             OutputPixelType;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::GaussianInvariantsImageFilter<
    InputImageType, OutputImageType >                     InvariantFilterType;
  typedef typename InvariantFilterType::Pointer           InvariantFilterPointer;
  typedef typename InvariantFilterType::SigmaType         SigmaType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  /** Setup sigma. */
  SigmaType sigmaFA; sigmaFA.Fill( sigma[ 0 ] );
  if ( sigma.size() == Dimension )
  {
    for ( unsigned int i = 0; i < Dimension; ++i )
    {
      sigmaFA[ i ] = sigma[ i ];
    }
  }

  /** Setup invariant filter. */
  InvariantFilterPointer invariantFilter = InvariantFilterType::New();
  invariantFilter->SetSigma( sigmaFA );
  invariantFilter->SetInvariant( invariant );
  invariantFilter->SetInput( reader->GetOutput() );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( invariantFilter->GetOutput() );
  writer->Update();

} // end GaussianImageFilterInvariants()

#endif // end #ifndef __gaussianImageFilterHelper_h

