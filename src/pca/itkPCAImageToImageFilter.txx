#ifndef _itkPCAImageToImageFilter_txx
#define _itkPCAImageToImageFilter_txx

#include "itkPCAImageToImageFilter.h"

#include "vnl/vnl_math.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_fastops.h>

namespace itk
{

  /**
   * ********************* Constructor ****************************
   */

  template< class TInputImage, class TOutputImage >
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::PCAImageToImageFilter( void )
  {
    this->m_MeanOfFeatureImages.set_size( 0 );
    this->m_CenteredFeatureImages.set_size( 0, 0 );

    this->m_CovarianceMatrix.set_size( 0, 0 );
    this->m_EigenVectors.set_size( 0, 0 );
    this->m_EigenValues.set_size( 0 );
    this->m_NormalisedEigenValues.set_size( 0 );
    this->m_PrincipalComponents.set_size( 0, 0 );

    this->m_NumberOfPixels = 0;
    this->m_NumberOfFeatureImages = 0;
    this->m_NumberOfPrincipalComponentsRequired = 0;

  } // end Constructor()


  /**
   * ********************* EnlargeOutputRequestedRegion ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
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
   * ********************* GenerateInputRequestedRegion ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::GenerateInputRequestedRegion( void )
  {
    Superclass::GenerateInputRequestedRegion();

    if ( this->GetInput( 0 ) )
    {
      /** Set the requested region of the first input to largest possible region. */
      InputImagePointer input = const_cast< TInputImage *>( this->GetInput( 0 ) );
      input->SetRequestedRegionToLargestPossibleRegion();
      //this->GetInput( 0 )->SetRequestedRegionToLargestPossibleRegion();

      /** Set the requested region of the remaining input to the
       * largest possible region of the first input.
       */
      for ( unsigned int i = 1; i < this->GetNumberOfInputs(); ++i )
      {
        if ( this->GetInput( i ) )
        {
          typename TInputImage::RegionType requestedRegion =
            this->GetInput( 0 )->GetLargestPossibleRegion();

          typename TInputImage::RegionType largestRegion =
            this->GetInput( i )->GetLargestPossibleRegion();

          if ( !largestRegion.IsInside( requestedRegion ) )
          {
            itkExceptionMacro( << "LargestPossibleRegion of input " << i
              << " is not a superset of the LargestPossibleRegion of input 0" );
          }

          InputImagePointer ptr = const_cast<TInputImage *>( this->GetInput( i ) );
          ptr->SetRequestedRegion( requestedRegion );
          //this->GetInput( i )->SetRequestedRegion( requestedRegion );

        }
      }
    }

  } // end GenerateInputRequestedRegion()


  /**
   * ********************* SetNumberOfPrincipalComponentsRequired ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::SetNumberOfPrincipalComponentsRequired( unsigned int n )
  {
    if ( m_NumberOfPrincipalComponentsRequired != n )
    {
      this->m_NumberOfPrincipalComponentsRequired = n;
      this->Modified();

      /** Create enough outputs. */
      this->SetAndCreateOutputs( n );
    }

  } // end SetNumberOfPrincipalComponentsRequired()


  /**
   * ********************* SetAndCreateOutputs ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
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

  } // end SetAndCreateOutputs()


  /**
   * ********************* SetNumberOfFeatureImages ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::SetNumberOfFeatureImages( unsigned int n )
  {
    if ( this->m_NumberOfFeatureImages != n )
    {
      this->m_NumberOfFeatureImages = n;
      this->Modified();

      /** Modify the required number of inputs. */
      this->SetNumberOfRequiredInputs( this->m_NumberOfFeatureImages );
    }

  } // end SetNumberOfFeatureImages()


  /**
   * ********************* GenerateData ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::GenerateData( void )
  {
    /** Do the principal component analysis. */
    this->PerformPCA();

    /** Allocate memory for each output. ??Why here? */
    unsigned int numberOfOutputs =
      static_cast<unsigned int>( this->GetNumberOfOutputs() );

    for ( unsigned int i = 0; i < numberOfOutputs; ++i )
    {
      OutputImagePointer output = this->GetOutput( i );
      output->SetBufferedRegion( output->GetRequestedRegion() );
      output->Allocate();
    }

    /** Create the output images. This is a matter of converting
     * the vnl_matrix of principal components to images.
     */
    for ( unsigned int i = 0; i < numberOfOutputs; ++i )
    {
      /** Extract one column vector at a time. This is faster
       * than extracting the column every time, but comes at
       * the cost of some additional memory use.
       */

      /** Create iterator. */
      OutputImageIterator iter( this->GetOutput( i ), this->GetOutput( i )->GetRequestedRegion() );
      iter.GoToBegin();
      unsigned int pix = 0;

      /** Fill this output with a principal component. */
      while ( !iter.IsAtEnd() )
      {
        iter.Set( static_cast< OutputImagePixelType >( this->m_PrincipalComponents[ pix ][ i ] ) );
        ++iter;
        ++pix;
      }
    }

    /** Maybe remove principal components? */
    //this->m_PrincipalComponents.set_size( 0, 0 );

  } // end GenerateData()


  /**
   * ********************* PerformPCA ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::PerformPCA( void )
  {
    /** Get the number of pixels. */
    this->m_NumberOfPixels = this->GetInput( 0 )->GetBufferedRegion().GetNumberOfPixels();

    this->CheckNumberOfOutputs();
    this->CalculateMeanOfFeatureImages();
    this->CenterFeatureImages();
    this->CalculateCovarianceMatrix();
    this->PerformEigenAnalysis();

  } // end PerformPCA()


  /**
   * ********************* CheckNumberOfOutputs ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::CheckNumberOfOutputs( void )
  {
    if ( this->m_NumberOfPrincipalComponentsRequired == 0 )
    {
      /** In this case the number of required PC's have not been
       * specified (or illegally specified to 0). We let this
       * filter output all principal components.
       */
      this->SetAndCreateOutputs( this->m_NumberOfFeatureImages );
    }
    else
    {
      /** In this case the number of required PC's have been
       * specified.
       */
      unsigned int numberOfValidOutputs = vnl_math_min(
        this->m_NumberOfFeatureImages,
        this->m_NumberOfPrincipalComponentsRequired );
      this->SetAndCreateOutputs( numberOfValidOutputs );
    }

  } // end CheckNumberOfOutputs()


  /**
   * ********************* CalculateMeanOfFeatureImages ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::CalculateMeanOfFeatureImages( void )
  {
    /** Get pointers to the input and setup iterators. */
    std::vector< InputImageConstPointer >   inputImages( this->m_NumberOfFeatureImages );
    std::vector< InputImageConstIterator >  iterators( this->m_NumberOfFeatureImages );

    for ( unsigned int i = 0; i < this->m_NumberOfFeatureImages; i++ )
    {
      inputImages[ i ] = this->GetInput( i );

      iterators[ i ] = InputImageConstIterator(
        inputImages[ i ], inputImages[ i ]->GetBufferedRegion() );
      iterators[ i ].GoToBegin();
    }

    /** Calculate for each training image its mean. */
    this->m_MeanOfFeatureImages.set_size( this->m_NumberOfFeatureImages );
    this->m_MeanOfFeatureImages.fill( 0.0 );

    for ( unsigned int im = 0; im < this->m_NumberOfFeatureImages; ++im )
    {
      for ( unsigned int pix = 0; pix < this->m_NumberOfPixels; pix++ )
      {
        this->m_MeanOfFeatureImages[ im ] += iterators[ im ].Get();
        ++iterators[ im ];
      }
    }
    this->m_MeanOfFeatureImages /= this->m_NumberOfPixels;

  } // end CalculateMeanOfFeatureImages()


  /**
   * ********************* CenterFeatureImages ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::CenterFeatureImages( void )
  {
    /** Get pointers to the input and setup iterators. */
    std::vector< InputImageConstIterator >  iterators( this->m_NumberOfFeatureImages );

    for ( unsigned int i = 0; i < this->m_NumberOfFeatureImages; i++ )
    {
      iterators[ i ] = InputImageConstIterator(
        this->GetInput( i ), this->GetInput( i )->GetBufferedRegion() );
      iterators[ i ].GoToBegin();
    }

    /** Set the proper size of the centered training images. */
    this->m_CenteredFeatureImages.set_size(
      this->m_NumberOfPixels, this->m_NumberOfFeatureImages );

    /** Center the training images by subtracting the mean. */
    for ( unsigned int i = 0; i < m_NumberOfFeatureImages; ++i )
    {
      for ( unsigned int pix = 0; pix < this->m_NumberOfPixels; pix++ )
      {

        this->m_CenteredFeatureImages[ pix ][ i ] =
          (iterators[ i ]).Get() - this->m_MeanOfFeatureImages[ i ];
        ++iterators[ i ];
      }
    }

    /** TODO: Try to remove the input images, for memory efficiency.
     * However, this also depends on other references to the input.
     * We could try an InPlaceImageFilter.
     */

  } // end CenterFeatureImages()


  /**
   * ********************* CalculateCovarianceMatrix ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::CalculateCovarianceMatrix( void )
  {
    /** Calculate the inner product of the centered training data.
     * The slow and memory consuming alternative is:
     * this->m_CovarianceMatrix =
     *   this->m_CenteredTrainingImages.transpose() *
     *   this->m_CenteredTrainingImages;
     * The vnl function AtA is fast and memory efficient.
     */
    vnl_fastops::AtA( this->m_CovarianceMatrix, this->m_CenteredFeatureImages );

    /** Divide. */
    if ( this->m_NumberOfPixels != 1 )
    {
      this->m_CovarianceMatrix /= ( this->m_NumberOfPixels - 1 );
    }
    else
    {
      this->m_CovarianceMatrix.fill( 0.0 );
    }

  } // end CalculateCovarianceMatrix()


  /**
   * ********************* PerformEigenAnalysis ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::PerformEigenAnalysis( void )
  {
    /** Perform the eigen analysis. */
    vnl_symmetric_eigensystem< double > eigenSystem( this->m_CovarianceMatrix );

    /** Get the eigen vectors. */
    this->m_EigenVectors = eigenSystem.V;
    this->m_EigenVectors.fliplr();

    /** Get the eigen values. */
    this->m_EigenValues = (eigenSystem.D).diagonal();
    this->m_EigenValues.flip();

    /** Also get a normalised version. */
    this->m_NormalisedEigenValues = this->m_EigenValues;
    this->m_NormalisedEigenValues.normalize();

    /** Calculate the principal components.
     * This is done by multiplying the training data with the eigen vectors.
     */
    this->m_PrincipalComponents =
      this->m_CenteredFeatureImages * this->m_EigenVectors;

    /** The CenteredFeatureImages can now be removed for memory efficiency. */
    this->m_CenteredFeatureImages.set_size( 0, 0 );

  } // end PerformEigenAnalysis()


  /**
   * ********************* PrintSelf ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    PCAImageToImageFilter< TInputImage, TOutputImage >
    ::PrintSelf( std::ostream& os, Indent indent ) const
  {
    /** Call the superclass implementation. */
    Superclass::PrintSelf( os, indent );

    /** Print the member variables. */
    os << indent << "NumberOfPrincipalComponentsRequired: "
      << this->m_NumberOfPrincipalComponentsRequired << std::endl;
    os << indent << "NumberOfFeatureImages: "
      << this->m_NumberOfFeatureImages << std::endl;
    os << indent << "NumberOfPixels: "
      << this->m_NumberOfPixels << std::endl;

    os << indent << "CovarianceMatrix: " << std::endl;
    for( unsigned int i = 0; i < this->m_CovarianceMatrix.size(); i++ )
    {
      os << indent << this->m_CovarianceMatrix.get_row( i ) << std::endl;
    }

    os << indent << "EigenValues: "
      << this->m_EigenValues << std::endl;
    os << indent << "NormalisedEigenValues: "
      << this->m_NormalisedEigenValues << std::endl;

    os << indent << "Eigenvectors: " << std::endl;
    for( unsigned int i = 0; i < this->m_EigenValues.size(); i++ )
    {
      os << indent << this->m_EigenVectors.get_row( i ) << std::endl;
    }

  } // end PrintSelf()


} // namespace itk

#endif // end #ifndef _itkPCAImageToImageFilter_txx
