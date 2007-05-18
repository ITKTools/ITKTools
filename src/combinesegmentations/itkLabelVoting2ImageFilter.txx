#ifndef _itkLabelVoting2ImageFilter_txx
#define _itkLabelVoting2ImageFilter_txx

#include "itkLabelVoting2ImageFilter.h"

#include "vnl/vnl_math.h"

namespace itk
{

  template <typename TInputImage, typename TOutputImage, typename TWeights>
    LabelVoting2ImageFilter<TInputImage, TOutputImage, TWeights>
    ::LabelVoting2ImageFilter()
  {
    this->m_HasPriorPreference = false;
    this->m_HasObserverTrust = false;
    this->m_HasNumberOfClasses = false;

    this->m_GenerateProbabilisticSegmentations = false;
    this->m_NumberOfClasses = 2;
    this->m_LeastPreferredLabel = 1;
  } // end constructor


  template <typename TInputImage, typename TOutputImage, typename TWeights>
    void
    LabelVoting2ImageFilter<TInputImage, TOutputImage, TWeights>
    ::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    //etc  
  } // end PrintSelf


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    typename LabelVoting2ImageFilter<TInputImage,TOutputImage, TWeights>::InputPixelType
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::ComputeMaximumInputValue()
  {
    /** compute the maximum class label from the input data */
    InputPixelType maxLabel = 0;
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    for ( int k = 0; k < numberOfInputs; ++k )
    {
      InputConstIteratorType it
        ( this->GetInput( k ), this->GetInput( k )->GetBufferedRegion() );

      for ( it.GoToBegin(); !it.IsAtEnd(); ++it )
        maxLabel = vnl_math_max( maxLabel, it.Get() );
    }

    return maxLabel;
  } // end ComputeMaximumInputValue


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::BeforeThreadedGenerateData ()
  {
    Superclass::BeforeThreadedGenerateData();

    const bool generateProbSeg = 
      this->GetGenerateProbabilisticSegmentations();
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    /** Set some default values if necessary */
    if ( this->m_HasNumberOfClasses == false )
    {
      this->m_NumberOfClasses = this->ComputeMaximumInputValue() + 1;
    }
    if ( ! this->m_HasPriorPreference )
    {
      this->m_PriorPreference.SetSize( this->m_NumberOfClasses );
      for ( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
      {
        this->m_PriorPreference[i] = i;
      }
    }
    if ( !this->m_HasObserverTrust )
    {
      this->m_ObserverTrust.SetSize( numberOfInputs );
      this->m_ObserverTrust.Fill(1.0);
    }

    /** Determine the least preferred label */
    this->m_LeastPreferredLabel = 0;
    for (unsigned int i= 0; i< this->m_NumberOfClasses; ++i)
    {
      if ( this->m_PriorPreference[i] == (this->m_NumberOfClasses-1) )
      {
        this->m_LeastPreferredLabel = i;
      }
    }

    /**  Allocate the output image. */
    this->AllocateOutputs();
    typename TOutputImage::Pointer output = this->GetOutput();

    /** If probabilistic segmentations are desired, allocate them */   
    if ( generateProbSeg )
    {
      this->m_ProbabilisticSegmentationArray = 
        ProbabilisticSegmentationArrayType( this->m_NumberOfClasses );
      for ( unsigned int k = 0; k < this->m_NumberOfClasses; ++k )
      {
        this->m_ProbabilisticSegmentationArray[k] = 
          ProbabilityImageType::New();
        this->m_ProbabilisticSegmentationArray[k]->SetRegions(
          output->GetRequestedRegion() );
        this->m_ProbabilisticSegmentationArray[k]->Allocate();
      }
    }

  } // end BeforeThreadedGenerateData


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::ThreadedGenerateData( const OutputImageRegionType &outputRegionForThread,
    int itkNotUsed(threadId))
  {
    typedef Array<WeightsType>                  WType;
    typedef std::vector<InputConstIteratorType> InputConstIteratorArrayType;
    typedef std::vector<ProbIteratorType>       ProbIteratorArrayType;

    typename TOutputImage::Pointer output = this->GetOutput();
    const bool generateProbSeg = 
      this->GetGenerateProbabilisticSegmentations();
    const unsigned int numberOfInputs = this->GetNumberOfInputs();
    /** Votes by label, weighted by the observer trust */
    WType W( this->m_NumberOfClasses );

    /** create and initialize all input image iterators */
    InputConstIteratorArrayType it(numberOfInputs);
    for ( unsigned int k = 0; k < numberOfInputs; ++k )
    {
      it[k] = InputConstIteratorType
        ( this->GetInput( k ), outputRegionForThread );
    }

    /** Create and initialize the output probabilistic segmentation image iterators */
    ProbIteratorArrayType psit;
    if ( generateProbSeg )
    {
      psit = ProbIteratorArrayType( this->m_NumberOfClasses );
      for ( unsigned int k = 0; k < this->m_NumberOfClasses; ++k )
      {
        psit[k] = ProbIteratorType(
          this->m_ProbabilisticSegmentationArray[k], outputRegionForThread );
      }
    }

    /** Create and intialize the output iterator */
    OutputIteratorType out = OutputIteratorType( output, outputRegionForThread );

    /** Loop over the output pixels */
    for ( out.GoToBegin(); !out.IsAtEnd(); ++out )
    {
      // reset number of votes per label for all labels
      W.Fill(0.0);

      // count number of votes for the labels
      for( unsigned int i = 0; i < numberOfInputs; ++i)
      {
        const InputPixelType label = it[i].Get();
        W[label] += this->m_ObserverTrust( i );
        ++(it[i]);
      }

      /** normalize: */
      WeightsType sumW = W.sum();
      if ( sumW )
      {
        W /= sumW;
      }

      /** now determine the label with the maximum W, i.e., 
      * determine the label with the most votes for this pixel */
      OutputPixelType winningLabel = this->m_LeastPreferredLabel;
      WeightsType winningLabelW = 0.0;
      for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
      {
        if ( W[ci] > winningLabelW )
        {
          winningLabelW = W[ci];
          winningLabel = ci;
        }
        else
        {
          if ( ! (W[ci] < winningLabelW ) )
          {
            if ( this->m_PriorPreference[ci] < this->m_PriorPreference[winningLabel] )
            {
              winningLabel = ci;
            }
          }
        }
      } // next ci

      /** Set the winning label to the output pixel */
      out.Set( winningLabel );

      /** copy the W values into the probabilistic segmentation images
       * and move the psit iterators */
      if ( generateProbSeg )
      {
        for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
        {
          psit[ci].Set( W[ci] );
          ++(psit[ci]);
        }
      }

    } // end loop over output pixels

  } // end ThreadedGenerateData

} // end namespace itk

#endif
