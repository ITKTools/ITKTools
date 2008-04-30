#ifndef _itkReshapeImageToImageFilter_txx
#define _itkReshapeImageToImageFilter_txx

#include "itkReshapeImageToImageFilter.h"

namespace itk
{

  /**
   * ********************* Constructor ****************************
   */
  
  template< class TInputImage, class TOutputImage >
    ReshapeImageToImageFilter< TInputImage, TOutputImage >
    ::ReshapeImageToImageFilter( void )
  {

  } // end Constructor()


  /**
   * ********************* EnlargeOutputRequestedRegion ****************************
   */

  template< class TInputImage, class TOutputImage >
    void
    ReshapeImageToImageFilter< TInputImage, TOutputImage >
    ::EnlargeOutputRequestedRegion( DataObject * itkNotUsed(output) )
  {
    /** This filter requires the all of the output images to be in the buffer. */
    if ( this->GetOutput( 0 ) )
    {
      this->GetOutput( 0 )->SetRequestedRegionToLargestPossibleRegion();
    }
  
  } // end EnlargeOutputRequestedRegion()


  /**
   * ********************* GenerateData ****************************
   */

  template< class TInputImage, class TOutputImage >
    void 
    ReshapeImageToImageFilter< TInputImage, TOutputImage >
    ::GenerateData( void )
  {
    /** Get handles to the input and output. */
    OutputImagePointer input = this->GetInput();
    OutputImagePointer output = this->GetOutput();

    /** Allocate memory. */
    output->SetRegions( this->m_OutputRegion );
    output->Allocate();

    /** Get the number of pixels. */
    unsigned long numPixelsInput  = input->GetLargestPossibleRegion().GetNumberOfPixels();
    unsigned long numPixelsOutput = output->GetLargestPossibleRegion().GetNumberOfPixels();
    unsigned long min = numPixelsInput < numPixelsOutput ? numPixelsInput : numPixelsOutput;

    /** Copy pixels. */
    for ( unsigned long i = 0; i < min; ++i )
    {
    }

    
  } // end GenerateData()


  /**
   * ********************* PrintSelf ****************************
   */

  template < class TInputImage, class TOutputImage >
    void
    ReshapeImageToImageFilter< TInputImage, TOutputImage >
    ::PrintSelf( std::ostream& os, Indent indent ) const
  {
    /** Call the superclass implementation. */
    Superclass::PrintSelf( os, indent );

    /** Print the member variables. */
    os << indent << ": "
      << this-> << std::endl;

  } // end PrintSelf()


} // namespace itk

#endif // end #ifndef _itkReshapeImageToImageFilter_txx
