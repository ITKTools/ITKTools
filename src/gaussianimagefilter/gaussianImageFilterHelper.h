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


  /**
   * ******************* GetHelpString *******************
   */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxgaussianimagefilter" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + BLURRED.mhd" << std::endl
  << "  [-std]   sigma, for each dimension, default 1.0" << std::endl
  << "  [-ord]   order, for each dimension, default zero" << std::endl
  << "             0: zero order = blurring" << std::endl
  << "             1: first order = gradient" << std::endl
  << "             2: second order derivative" << std::endl
  << "  [-mag]   compute the magnitude of the separate blurrings, default false" << std::endl
  << "  [-lap]   compute the laplacian, default false" << std::endl
  << "  [-inv]   compute invariants, choose one of" << std::endl
  << "           {LiLi, LiLijLj, LiLijLjkLk, Lii, LijLji, LijLjkLki}" << std::endl
  << "  [-opct]  output pixel type, default equal to input" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()

#endif // end #ifndef __gaussianImageFilterHelper_h
