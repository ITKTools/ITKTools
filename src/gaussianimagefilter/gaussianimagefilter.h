/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __gaussianimagefilter_h_
#define __gaussianimagefilter_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkSmoothingRecursiveGaussianImageFilter2.h"
#include "itkGaussianInvariantsImageFilter.h"
#include "itkComposeImageFilter.h"
#include "itkVectorMagnitudeImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsContrastEnhanceImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsGaussianBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsGaussianBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_WhichOperation = "Gaussian";
    this->m_Invariant = "LiLi";
  };
  /** Destructor. */
  ~ITKToolsGaussianBase(){};

  /** Input member parameters. */
  std::string                 m_InputFileName;
  std::string                 m_OutputFileName;
  std::string                 m_WhichOperation;
  std::vector<float>          m_Sigma;
  std::vector<unsigned int>   m_Order;
  std::string                 m_Invariant;

}; // end class ITKToolsGaussianBase


/** \class ITKToolsGaussian
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsGaussian : public ITKToolsGaussianBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsGaussian Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsGaussian(){};
  ~ITKToolsGaussian(){};

  typedef itk::Image< TComponentType, VDimension >  OutputImageType;

  /** Run function. */
  void Run( void )
  {
    if ( this->m_WhichOperation == "Gaussian" )
    {
      this->GaussianImageFilter();
    }
    else if ( this->m_WhichOperation == "Magnitude" )
    {
      this->GaussianImageFilterMagnitude();
    }
    else if ( this->m_WhichOperation == "Laplacian" )
    {
      this->GaussianImageFilterLaplacian();
    }
    else if ( this->m_WhichOperation == "Invariants" )
    {
      this->GaussianImageFilterInvariants();
    }
  } // end Run();

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
  void GaussianImageFilter( void );

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
  void GaussianImageFilterMagnitude( void );

  /**
   * ******************* GaussianImageFilterLaplacian *******************
   *
   * This function computes the Laplacian of an image using Gaussian derivatives.
   *
   *   L( image I ) = sum_i d^2 I / d x_i^2
   *
   * i being the direction.
   * This is actually quite similar to the function GaussianImageFilterMagnitude,
   * defined above, with this->m_Order = [ 2 2 2 ]. But instead of using a magnitude filter,
   * a square magnitude should be used: the Laplacian computes the sum of squares,
   * while the vector magnitude computes the squareroot of the sum of squares.
   */
  void GaussianImageFilterLaplacian( void );

  /**
   * ******************* GaussianImageFilterInvariants *******************
   *
   * This function computes some invariants based on Gaussian derivatives.
   *
   * It can compute one of the following invariants:
   *   {LiLi, LiLijLj, LiLijLjkLk, Lii, LijLji, LijLjkLki }
   * where L is the input image, and using Einstein notation.
   * Together they form the irreducible set of second order Cartesian structure
   * invariants. They can also be written in matrix notation: *
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
  void GaussianImageFilterInvariants( void );

}; // end class ITKToolsGaussian

// \todo: move to hxx

/**
 * ******************* GaussianImageFilter *******************
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsGaussian< VDimension, TComponentType >
::GaussianImageFilter( void )
{
  /** Typedef's. */
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, VDimension >        InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, OutputImageType >                     FilterType;
  typedef typename FilterType::OrderType                  OrderType;
  typedef typename FilterType::SigmaType                  SigmaType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( this->m_InputFileName );

  /** Setup the this->m_Order and this->m_Sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( this->m_Sigma[ 0 ] );
  for( unsigned int i = 0; i < VDimension; ++i )
  {
    orderFA[ i ] = this->m_Order[ i ];
    if( this->m_Sigma.size() == VDimension ) sigmaFA[ i ] = this->m_Sigma[ i ];
  }

  /** Setup the smoothing filter. */
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNormalizeAcrossScale( false );
  filter->SetInput( reader->GetOutput() );
  filter->SetSigma( sigmaFA );
  filter->SetOrder( orderFA );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( this->m_OutputFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end GaussianImageFilter()


/**
 * ******************* GaussianImageFilterMagnitude *******************
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsGaussian< VDimension, TComponentType >
::GaussianImageFilterMagnitude( void )
{
  /** Typedef's. */
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, VDimension >        InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::SmoothingRecursiveGaussianImageFilter2<
    InputImageType, InputImageType >                      SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;
  typedef typename SmoothingFilterType::OrderType         OrderType;
  typedef typename SmoothingFilterType::SigmaType         SigmaType;
  typedef itk::ComposeImageFilter<
    InputImageType >                                      ImageToVectorImageFilterType;
  typedef typename ImageToVectorImageFilterType
    ::OutputImageType                                     VectorImageType;
  typedef itk::VectorMagnitudeImageFilter<
    VectorImageType, OutputImageType >                    MagnitudeFilterType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( this->m_InputFileName );

  /** Setup the this->m_Order and this->m_Sigma. */
  OrderType orderFA;
  SigmaType sigmaFA;
  sigmaFA.Fill( this->m_Sigma[ 0 ] );
  for( unsigned int i = 0; i < VDimension; ++i )
  {
    orderFA[ i ] = this->m_Order[ i ];
    if( this->m_Sigma.size() == VDimension ) sigmaFA[ i ] = this->m_Sigma[ i ];
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( VDimension );
  typename ImageToVectorImageFilterType::Pointer composeFilter
    = ImageToVectorImageFilterType::New();
  typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
  for( unsigned int i = 0; i < VDimension; ++i )
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
    composeFilter->SetInput( i, smoothingFilter[ i ]->GetOutput() );
  }

  /** Compose vector image and compute magnitude. */
  magnitudeFilter->SetInput( composeFilter->GetOutput() );
  magnitudeFilter->Update();

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( this->m_OutputFileName );
  writer->SetInput( magnitudeFilter->GetOutput() );
  writer->Update();

} // end GaussianImageFilterMagnitude()


/**
 * ******************* GaussianImageFilterLaplacian *******************
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsGaussian< VDimension, TComponentType >
::GaussianImageFilterLaplacian( void )
{
  /** Typedef's. */
  typedef typename OutputImageType::PixelType             OutputPixelType;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, VDimension >         InputImageType;
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
  reader->SetFileName( this->m_InputFileName );

  /** Setup this->m_Sigma. */
  SigmaType sigmaFA; sigmaFA.Fill( this->m_Sigma[ 0 ] );
  if( this->m_Sigma.size() == VDimension )
  {
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      sigmaFA[ i ] = this->m_Sigma[ i ];
    }
  }

  /** Setup filters. */
  std::vector< SmoothingFilterPointer > smoothingFilter( VDimension );
  for( unsigned int i = 0; i < VDimension; ++i )
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
  std::vector< ConstIteratorType > itIn( VDimension );
  for( unsigned int i = 0; i < VDimension; ++i )
  {
    itIn[ i ] = ConstIteratorType( smoothingFilter[ i ]->GetOutput(),
      smoothingFilter[ i ]->GetOutput()->GetLargestPossibleRegion() );
    itIn[ i ].GoToBegin();
  }
  IteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );
  itOut.GoToBegin();

  /** Fill the output image by adding the second this->m_Order derivatives. */
  while ( !itOut.IsAtEnd() )
  {
    InputPixelType value = itk::NumericTraits<InputPixelType>::Zero;
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      value += itIn[ i ].Get();
      ++itIn[ i ];
    }
    itOut.Set( static_cast<OutputPixelType>( value ) );
    ++itOut;
  }

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( this->m_OutputFileName );
  writer->SetInput( outputImage );
  writer->Update();

} // end GaussianImageFilterLaplacian()


/**
 * ******************* GaussianImageFilterInvariants *******************
 */

template< unsigned int VDimension, class TComponentType >
void
ITKToolsGaussian< VDimension, TComponentType >
::GaussianImageFilterInvariants( void )
{
  /** Typedef's. */
  typedef typename OutputImageType::PixelType             OutputPixelType;
  typedef float                                           InputPixelType;
  typedef itk::Image< InputPixelType, VDimension >         InputImageType;
  typedef itk::ImageFileReader< InputImageType >          ReaderType;
  typedef itk::GaussianInvariantsImageFilter<
    InputImageType, OutputImageType >                     InvariantFilterType;
  typedef typename InvariantFilterType::Pointer           InvariantFilterPointer;
  typedef typename InvariantFilterType::SigmaType         SigmaType;
  typedef itk::ImageFileWriter< OutputImageType >         WriterType;

  /** Read in the input image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( this->m_InputFileName );

  /** Setup this->m_Sigma. */
  SigmaType sigmaFA; sigmaFA.Fill( this->m_Sigma[ 0 ] );
  if( this->m_Sigma.size() == VDimension )
  {
    for( unsigned int i = 0; i < VDimension; ++i )
    {
      sigmaFA[ i ] = this->m_Sigma[ i ];
    }
  }

  /** Setup this->m_Invariant filter. */
  InvariantFilterPointer invariantFilter = InvariantFilterType::New();
  invariantFilter->SetSigma( sigmaFA );
  invariantFilter->SetInvariant( this->m_Invariant );
  invariantFilter->SetInput( reader->GetOutput() );

  /** Write image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( this->m_OutputFileName );
  writer->SetInput( invariantFilter->GetOutput() );
  writer->Update();

} // end GaussianImageFilterInvariants()


#endif // end #ifndef __gaussianimagefilter_h_

