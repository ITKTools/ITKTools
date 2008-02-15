#ifndef _itkTextureImageToImageFilter_txx
#define _itkTextureImageToImageFilter_txx

#include "itkTextureImageToImageFilter.h"

#include "../statisticsonimage/itkStatisticsImageFilterWithMask.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"


namespace itk
{

  /**
   * ********************* Constructor ****************************
   */
  
  template< class TInputImage, class TOutputImage >
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::TextureImageToImageFilter()
  {
    this->m_NumberOfRequestedOutputs = 8;
    this->m_NeighborhoodRadius = 3;

    this->m_OffsetsSetManually = false;
    this->m_OffsetScales.resize( 1, 1 );
    this->m_Offsets = OffsetVector::New();
    this->ComputeDefaultOffsets( this->m_OffsetScales );

    this->m_NumberOfHistogramBins = 64;
    this->m_NormalizeHistogram = false;
    this->m_HistogramMinimum = NumericTraits< InputImagePixelType >::min();
    this->m_HistogramMaximum = NumericTraits< InputImagePixelType >::max();
    this->m_HistogramMinimumSetManually = false;
    this->m_HistogramMaximumSetManually = false;

  } // end Constructor()


  /**
   * ********************* SetHistogramMinimum ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::SetHistogramMinimum( InputImagePixelType min )
  {
    if ( this->m_HistogramMinimum != min )
    {
      this->m_HistogramMinimum = min;
      this->m_HistogramMinimumSetManually = true;
      this->Modified();
    }

  } // end SetHistogramMinimum()


  /**
   * ********************* SetHistogramMaximum ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::SetHistogramMaximum( InputImagePixelType min )
  {
    if ( this->m_HistogramMaximum != min )
    {
      this->m_HistogramMaximum = min;
      this->m_HistogramMaximumSetManually = true;
      this->Modified();
    }

  } // end SetHistogramMaximum()


  /**
   * ********************* SetOffsets ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::SetOffsets( OffsetVector * vec )
  {
    if ( this->m_Offsets != vec )
    {
      this->Modified();
      this->m_Offsets = vec;
      this->m_OffsetsSetManually = true;
    }
  
  } // end SetOffsets()


  /**
   * ********************* SetOffsetScales ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::SetOffsetScales( const std::vector<unsigned int> & offsetScales )
  {
    if ( offsetScales.size() != this->m_OffsetScales.size() )
    {
      this->Modified();
      this->m_OffsetScales = offsetScales;
      return;
    }
    else
    {
      for ( unsigned int i = 0; i < offsetScales.size(); ++i )
      {
        if ( offsetScales[ i ] != this->m_OffsetScales[ i ] )
        {
          this->Modified();
          this->m_OffsetScales = offsetScales;
          return;
        }
      }
    }
      
  } // end SetOffsetScales()


  /**
   * ********************* GetOffsetScales ****************************
   */

  template < class TInputImage, class TOutputImage >
    const std::vector<unsigned int> &
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::GetOffsetScales( void ) const
  {
    return this->m_OffsetScales;
  } // end GetOffsetScales()


  /**
   * ********************* EnlargeOutputRequestedRegion ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::EnlargeOutputRequestedRegion( DataObject * itkNotUsed(output) )
  {
    /** This filter requires the all of the output images to be in the buffer. */
    for ( unsigned int i = 0; i < this->GetNumberOfOutputs(); ++i )
    {
      if ( this->GetOutput( i ) )
      {
        this->GetOutput( i )->SetRequestedRegionToLargestPossibleRegion();
      }
    }
  
  } // end EnlargeOutputRequestedRegion()


  /**
   * ********************* GenerateData ****************************
   */

  template< class TInputImage, class TOutputImage >
    void 
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::GenerateData( void )
  {
    /** Make sure that the input image is completely up-to-date. */
    InputImagePointer inputPtr = const_cast< InputImageType * > ( this->GetInput() );
    if ( !inputPtr ) return;
    inputPtr->Update();

    /** Create outputs. */
    this->SetAndCreateOutputs( this->m_NumberOfRequestedOutputs );

    /** Compute the minimum and maximum histogram entries. */
    this->ComputeHistogramMinimumAndMaximum();

    /** Compute the offsets. */
    this->ComputeDefaultOffsets( this->m_OffsetScales );

    /** Setup local co-occurrence matrix generator. */
    typename CooccurrenceMatrixGeneratorType::Pointer cmGenerator
      = CooccurrenceMatrixGeneratorType::New();
    cmGenerator->SetInput( inputPtr );
    cmGenerator->SetOffsets( this->m_Offsets );
    cmGenerator->SetNumberOfBinsPerAxis( this->m_NumberOfHistogramBins );
    cmGenerator->SetPixelValueMinMax( this->m_HistogramMinimum, this->m_HistogramMaximum );
    cmGenerator->SetNormalize( this->m_NormalizeHistogram );

    /** Setup local texture feature calculator. */
    typename TextureCalculatorType::Pointer cmCalculator
      = TextureCalculatorType::New();
    
    /** Typedefs. */
    typedef ConstNeighborhoodIterator< InputImageType >         ConstNeighborhoodIteratorType;
    typedef typename ConstNeighborhoodIteratorType::RadiusType  RadiusType;
    typedef ImageRegionIterator< OutputImageType >              OutputIteratorType;

    /** Setup a neigborhood iterator over the input image. */
    RadiusType radius;
    radius.Fill( this->m_NeighborhoodRadius );
    ConstNeighborhoodIteratorType nit(
      radius,
      this->GetInput(),
      this->GetInput()->GetRequestedRegion() );
    nit.GoToBegin();

    /** Setup iterators over the output images. */
    const unsigned int noo = this->GetNumberOfOutputs();
    std::vector< OutputIteratorType > outputIterators( noo );
    for ( unsigned int i = 0; i < noo; ++i )
    {
      outputIterators[ i ] = OutputIteratorType( this->GetOutput( i ),
        this->GetOutput()->GetLargestPossibleRegion() );
      outputIterators[ i ].GoToBegin();
    }

    /** Printing progress. */
    const unsigned long totalNumberOfVoxels = inputPtr->GetBufferedRegion().GetNumberOfPixels();
    const unsigned long numberOfUpdates = 100;
    const unsigned long frac =
      static_cast<unsigned long>( totalNumberOfVoxels / numberOfUpdates );
    unsigned long currentVoxel = 0;
    std::cout << "Progress: 0%";

    /** Loop over the input image. */
    while ( !nit.IsAtEnd() )
    {
      /** Construct a local neighborhood. */
      InputImageRegionType localRegion = nit.GetBoundingBoxAsImageRegion();
      localRegion.Crop( inputPtr->GetBufferedRegion() );

      /** Generate the co-occurrence over a local region only. */
      inputPtr->SetRequestedRegion( localRegion );
      cmGenerator->Compute();

      /** Compute texture features from this co-occurrence matrix. */
      cmCalculator->SetHistogram( cmGenerator->GetOutput() );
      cmCalculator->Compute();

      /** Copy the requested texture features to the outputs. And update iterators. */
      for ( unsigned int ii = 0; ii < noo; ++ii )
      {
        outputIterators[ ii ].Set( cmCalculator->GetFeature( ii ) );
        ++outputIterators[ ii ];
      }
      ++nit;

      /** Print progress. */
      ++currentVoxel;
      if ( currentVoxel % frac == 0 )
      {
        unsigned int progress = static_cast<unsigned int>(
          100.0 * static_cast<float>( currentVoxel ) /
          static_cast<float>( totalNumberOfVoxels ) );
        std::cout << "\rProgress: " << progress << "%";
        std::cout << std::flush;
      }

    } // end while
    std::cout << "\rProgress: 100%" << std::endl;

  } // end GenerateData()


   /**
   * ********************* SetAndCreateOutputs ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::SetAndCreateOutputs( unsigned int n )
  {
    /** Modify the required number of outputs. */
    this->SetNumberOfRequiredOutputs( n );

    /** Add or remove outputs. */
    unsigned int noo = this->GetNumberOfOutputs();
    if ( noo < n )
    {
      for ( unsigned int i = noo; i < n; ++i )
      {
        typename DataObject::Pointer output = this->MakeOutput( i );
        this->SetNthOutput( i, output.GetPointer() );
      }
    }
    else if ( noo > n )
    {
      for ( unsigned int i = noo - 1; i >= n; --i )
      {
        typename DataObject::Pointer output = this->GetOutputs()[ i ];
        this->RemoveOutput( output );
      }
    }

    /** Allocate memory for each output. */
    unsigned int numberOfOutputs = 
      static_cast<unsigned int>( this->GetNumberOfOutputs() );
    for ( unsigned int i = 0; i < numberOfOutputs; ++i )
    {
      OutputImagePointer output = this->GetOutput( i );
      output->SetRegions( this->GetOutput( 0 )->GetRequestedRegion() );
      output->Allocate();
    }

  } // end SetAndCreateOutputs()


  /**
   * ********************* ComputeHistogramMinimumAndMaximum ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::ComputeHistogramMinimumAndMaximum( void )
  {
    /** Typedefs needed for computing the minimum and maximum of the input image. */
    typedef StatisticsImageFilter< InputImageType >  StatisticsFilterType;
    typename StatisticsFilterType::Pointer stats = StatisticsFilterType::New();
    InputImagePixelType min, max;

    /** Compute, but only if necessary. */
    if ( !this->m_HistogramMinimumSetManually || !this->m_HistogramMinimumSetManually )
    {
      stats->SetInput( this->GetInput() );
      stats->Update();
      min = stats->GetMinimum();
      max = stats->GetMaximum();
    }

    /** Update the minimum. */
    if ( !this->m_HistogramMinimumSetManually )
    {
      this->m_HistogramMinimum = min;
    }

    /** Update the maximum. */
    if ( !this->m_HistogramMaximumSetManually )
    {
      this->m_HistogramMaximum = max;
    }

  } // end ComputeHistogramMinimumAndMaximum()


  /**
   * ********************* ComputeDefaultOffsets ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::ComputeDefaultOffsets( std::vector<unsigned int> scales )
  {
    /** Compute, but only if necessary.
     * This loop creates the offsets (in 2D):
     * - scales = (1)    : (1,0); (0,1)
     * - scales = (1,2)  : (1,0); (0,1); (2,0); (0,2)
     * - scales = (1,2,4): (1,0); (0,1); (2,0); (0,2); (4,0); (0,4)
     * etcetera. The scales are by default just 1.
     */
    if ( !this->m_OffsetsSetManually )
    {
      this->m_Offsets->Reserve( scales.size() * InputImageDimension );
      unsigned int k = 0;
      OffsetType offset;
      for ( unsigned int i = 0; i < scales.size(); ++i )
      {
        for ( unsigned int j = 0; j < InputImageDimension; ++j )
        {
          offset.Fill( 0 );
          offset[ j ] = scales[ i ];
          this->m_Offsets->SetElement( k, offset );
          ++k;
        }
      }
    }

  } // end ComputeDefaultOffsets()
  

  /**
   * ********************* PrintSelf ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    TextureImageToImageFilter< TInputImage, TOutputImage >
    ::PrintSelf( std::ostream& os, Indent indent ) const
  {
    /** Call the superclass implementation. */
    Superclass::PrintSelf( os, indent );

    /** Print the member variables. */
    os << indent << "NeighborhoodRadius: "
      << this->m_NeighborhoodRadius << std::endl;
    os << indent << "NumberOfRequestedOutputs: "
      << this->m_NumberOfRequestedOutputs << std::endl;

    os << indent << "OffsetsSetManually: "
      << this->m_OffsetsSetManually << std::endl;
    os << indent << "OffsetScales: " << std::endl;
    for( unsigned int i = 0; i < this->m_OffsetScales.size(); i++ )
    {
      os << this->m_OffsetScales[ i ] << " ";
    }
    os << std::endl;
    os << indent << "Offsets: ";
    for ( unsigned int i = 0; i < this->m_Offsets->Size(); ++i )
    {
      os << this->m_Offsets->GetElement( i ) << " ";
    }
    os << std::endl;

    os << indent << "NumberOfHistogramBins: "
      << this->m_NumberOfHistogramBins << std::endl;
    os << indent << "HistogramMinimum: "
      << this->m_HistogramMinimum << std::endl;
    os << indent << "HistogramMaximum: "
      << this->m_HistogramMaximum << std::endl;

    os << indent << "HistogramMinimumSetManually: "
      << this->m_HistogramMinimumSetManually << std::endl;
    os << indent << "HistogramMaximumSetManually: "
      << this->m_HistogramMaximumSetManually << std::endl;
    os << indent << "NormalizeHistogram: "
      << this->m_NormalizeHistogram << std::endl;

  } // end PrintSelf()


} // namespace itk

#endif // end #ifndef _itkTextureImageToImageFilter_txx
