#ifndef _itkGaussianInvariantsImageFilter_txx
#define _itkGaussianInvariantsImageFilter_txx

#include "itkGaussianInvariantsImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_trace.h"
#include "itkProgressAccumulator.h"

namespace itk
{

/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::GaussianInvariantsImageFilter()
{
  /** Initialize variables. */
  this->m_NormalizeAcrossScale = false;
  this->m_Invariant = "";

  /** Setup the derivative filters. */
  this->m_DerivativeFilters.resize( ImageDimension );
  for ( unsigned int i = 0; i < ImageDimension; ++i )
  {
    OrderType order; order.Fill( 0 ); order[ i ] = 1;
    this->m_DerivativeFilters[ i ] = DerivativeFilterType::New();
    this->m_DerivativeFilters[ i ]->SetOrder( order );
    this->m_DerivativeFilters[ i ]->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );
    //this->m_DerivativeFilters[ i ]->ReleaseDataFlagOn();
  }

  /** Setup the Hessian filter. */
  this->m_HessianFilter = HessianFilterType::New();
  this->m_HessianFilter->SetNormalizeAcrossScale( this->m_NormalizeAcrossScale );
  //this->m_HessianFilter->ReleaseDataFlagOn();

  /** Initialize variables. */
  this->SetSigma( 1.0 );

} // end Constructor


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void 
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::SetSigma( const ScalarRealType sigma )
{
  SigmaType sigmaFA;
  sigmaFA.Fill( sigma );
  this->SetSigma( sigmaFA );
} // end SetSigma()


/**
 * Set value of Sigma
 */
template <typename TInputImage, typename TOutputImage>
void 
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::SetSigma( const SigmaType sigma )
{
  if ( this->m_Sigma != sigma )
  {
    this->m_Sigma = sigma;
    this->Modified();

    /** Pass on the sigma. */
    for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
      this->m_DerivativeFilters[ i ]->SetSigma( sigma );
    }
    this->m_HessianFilter->SetSigma( sigma );
  } // end if

} // end SetSigma()


/**
 * Set Normalize Across Scale Space
 */
template <typename TInputImage, typename TOutputImage>
void 
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::SetNormalizeAcrossScale( const bool arg )
{
  if ( this->m_NormalizeAcrossScale != arg )
  {
    this->m_NormalizeAcrossScale = arg;
    this->Modified();

    /** Pass on the argument. */
    for( unsigned int i = 0; i < ImageDimension; i++ )
    {
      this->m_DerivativeFilters[ i ]->SetNormalizeAcrossScale( arg );
    }
    this->m_HessianFilter->SetNormalizeAcrossScale( arg );
  } // end if

} // end SetNormalizeAcrossScale()


/**
 * Set invariant
 */

template <typename TInputImage, typename TOutputImage>
void 
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::SetInvariant( std::string arg )
{
  if ( this->m_Invariant != arg )
  {
    this->m_Invariant = arg;
    this->Modified();
  }

} // end SetInvariant()


//
//
//
template <typename TInputImage, typename TOutputImage>
void
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename GaussianInvariantsImageFilter<TInputImage,TOutputImage>
    ::InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  if( image )
    {
    image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
    }
}


//
//
//
template <typename TInputImage, typename TOutputImage>
void
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if (out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}

/**
 * Compute filter for Gaussian kernel
 */
template <typename TInputImage, typename TOutputImage >
void
GaussianInvariantsImageFilter<TInputImage,TOutputImage >
::GenerateData(void)
{
  /** Typedefs. */
  typedef ImageRegionIterator<
    OutputImageType >                     OutputIteratorType;
  typedef ImageRegionConstIterator<
    RealImageType >                       DerivativeIteratorType;
  typedef ImageRegionConstIterator<
    HessianOutputImageType >              HessianIteratorType;

  /** Get a pointer to the input. */
  InputImageConstPointer input( this->GetInput() );
  //if ( input )

  /** Allocate output image. */
  OutputImagePointer output = this->GetOutput();
  output->SetRegions( input->GetRequestedRegion() );
  output->Allocate();

  /** Create a process accumulator for tracking the progress of this minipipeline. */
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter( this );

  /** Register the filters. */
  for ( unsigned int i = 0; i < ImageDimension; i++ )
  {
    progress->RegisterInternalFilter(
      this->m_DerivativeFilters[ i ], 1.0 / ( ImageDimension + 1.0 ) );
  }
  progress->RegisterInternalFilter(
    this->m_HessianFilter, 1.0 / ( ImageDimension + 1.0 ) );

  /** Compute derivatives and Hessian. */
  for ( unsigned int i = 0; i < ImageDimension; i++ )
  {
    this->m_DerivativeFilters[ i ]->SetInput( input );
    this->m_DerivativeFilters[ i ]->Update();
  }
  this->m_HessianFilter->SetInput( input );
  this->m_HessianFilter->Update();

  /** Setup iterators. */
  std::vector< DerivativeIteratorType > derIt( ImageDimension );
  for ( unsigned int i = 0; i < ImageDimension; i++ )
  {
    derIt[ i ] = DerivativeIteratorType(
      this->m_DerivativeFilters[ i ]->GetOutput(),
      this->m_DerivativeFilters[ i ]->GetOutput()->GetLargestPossibleRegion() );
    derIt[ i ].GoToBegin();
  }
  HessianIteratorType hesIt(
    this->m_HessianFilter->GetOutput(),
    this->m_HessianFilter->GetOutput()->GetLargestPossibleRegion() );
  hesIt.GoToBegin();
  OutputIteratorType outIt( output, output->GetLargestPossibleRegion() );
  outIt.GoToBegin();

  /** Initialize temporary variables. */
  vnl_vector< ScalarRealType > gradient( ImageDimension, 0.0 );
  vnl_matrix< ScalarRealType > H( ImageDimension, ImageDimension, 0.0 );

  /** Loop over the output image. */
  while ( !outIt.IsAtEnd() )
  {
    /** Construct gradient. */
    for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
      gradient[ i ] = derIt[ i ].Value();
      ++derIt[ i ];
    }
    
    /** Construct Hessian. */
    HessianPixelType hes = hesIt.Value();
    for ( unsigned int row = 0; row < ImageDimension; row++ )
    {
      for ( unsigned int col = 0; col < ImageDimension; col++ )
      {
        H[ row ][ col ] = hes( row, col );
      }
    }

    /** Compute the invariant. */
    ScalarRealType outValue = 0.0;
    if ( this->m_Invariant == "LiLi" )
    {
      /** LiLi = gradient magnitude */
      outValue = gradient.magnitude();
    }
    else if ( this->m_Invariant == "LiLijLj" )
    {
      /** LiLijLj = g^T H g */
      outValue = dot_product( gradient * H, gradient );
    }
    else if ( this->m_Invariant == "LiLijLjkLk" )
    {
      /** LiLijLjkLk = g^T H H g */
      outValue = dot_product( gradient * ( H * H ), gradient );
    }
    else if ( this->m_Invariant == "Lii" )
    {
      /** Lii = trace( H ) = Laplacian */
      outValue = vnl_trace( H );
    }
    else if ( this->m_Invariant == "LijLji" )
    {
      /** LijLji = trace( H H ) */
      outValue = vnl_trace( H * H );
    }
    else if ( this->m_Invariant == "LijLjkLki" )
    {
      /** LijLjkLki = trace( H H H ) */
      outValue = vnl_trace( H * H * H );
    }
    else
    {
      itkExceptionMacro( << "ERROR: the invariant \"" << this->m_Invariant << "\" is not implemented" );
    }

    /** Set the output value. */
    outIt.Set( static_cast< OutputPixelType >( outValue ) );

    /** Update iterator. */
    ++hesIt;
    ++outIt;
  }

} // end GenerateData()


template <typename TInputImage, typename TOutputImage>
void
GaussianInvariantsImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "NormalizeAcrossScale: " << m_NormalizeAcrossScale << std::endl;
}


} // end namespace itk

#endif
