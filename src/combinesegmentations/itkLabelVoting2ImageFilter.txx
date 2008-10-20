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
    this->m_MaskImage = 0;
    this->m_GenerateConfusionMatrix = false;
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
    ::AllocateConfusionMatrixArray()
  {
    /**  we need one confusion matrix for every input */
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    this->m_ConfusionMatrixArray.clear();
    
    /** create the confusion matrix and space for updated confusion matrix for
     * each of the input images */
    for ( unsigned int k = 0; k < numberOfInputs; ++k )
    {
      /** the confusion matrix has as many row/columns as there are input labels,
       * The column nrs correspond to the 'real' class, as estimated by the 
       * STAPLE algorithm. The rows correspond to the classes assigned by
       * the observers.
       * We skip the extra column to accomodate "reject" classifications,
       * which was implemented by Rohlfing. */
      this->m_ConfusionMatrixArray.push_back(
        ConfusionMatrixType( this->m_NumberOfClasses, this->m_NumberOfClasses ) );
      this->m_ConfusionMatrixArray[k].Fill(0.0);
    }

    /** For multithreading: */
    unsigned int numberOfThreads = this->GetNumberOfThreads();
    this->m_ConfusionMatrixArrays.resize( numberOfThreads );
    for ( unsigned int t = 0; t < numberOfThreads; ++t)
    {
      this->m_ConfusionMatrixArrays[t].clear();
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        this->m_ConfusionMatrixArrays[t].push_back( 
          ConfusionMatrixType( this->m_NumberOfClasses, this->m_NumberOfClasses ) );
        this->m_ConfusionMatrixArrays[t][k].Fill(0.0);
      }
    }

  } // end AllocateConfusionMatrixArray
   


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::BeforeThreadedGenerateData ()
  {
    this->Superclass::BeforeThreadedGenerateData();

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
        this->m_ProbabilisticSegmentationArray[k]->CopyInformation( output );
        this->m_ProbabilisticSegmentationArray[k]->Allocate();
      }
    }

    /** Allocate the confusion matrix arrays */
    if ( this->GetGenerateConfusionMatrix() )
    {
      this->AllocateConfusionMatrixArray();
    }

  } // end BeforeThreadedGenerateData


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::ThreadedGenerateData( const OutputImageRegionType &outputRegionForThread,
    int threadId)
  {
    typedef Array<WeightsType>                  WType;
    typedef std::vector<InputConstIteratorType> InputConstIteratorArrayType;
    typedef std::vector<ProbIteratorType>       ProbIteratorArrayType;

    typename TOutputImage::Pointer output = this->GetOutput();
    const bool generateProbSeg = 
      this->GetGenerateProbabilisticSegmentations();
    const bool generateConfusionMatrix = this->GetGenerateConfusionMatrix();
    const unsigned int numberOfInputs = this->GetNumberOfInputs();
    const bool useMask = this->m_MaskImage.IsNotNull();
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

    /** Create and initialize the mask iterator */
    MaskConstIteratorType mit;
    const MaskPixelType zeroMaskPixel = itk::NumericTraits<MaskPixelType>::Zero;
    if ( useMask )
    {
      mit = MaskConstIteratorType(
        this->m_MaskImage, outputRegionForThread );
    }

    /** Create and intialize the output iterator */
    OutputIteratorType out = OutputIteratorType( output, outputRegionForThread );

    /** Loop over the output pixels */
    for ( out.GoToBegin(); !out.IsAtEnd(); ++out )
    {
      // reset number of votes per label for all labels
      W.Fill(0.0);
      OutputPixelType winningLabel = this->m_LeastPreferredLabel;

      bool insideMask = true;
      if (useMask)
      {
        /** For pixels outside the mask use the decision
         * of the first observer */
        if ( mit.Get() == zeroMaskPixel )
        {
          insideMask = false;
          winningLabel = it[0].Get();
          W[ winningLabel ] = 1.0;
          /** Set the winning label to the output pixel */
          out.Set( winningLabel );
          /** move the it iterators */
          for ( unsigned int k = 0; k < numberOfInputs; ++k )
          {
            ++(it[k]);
          }
        } // if mit==zero
        ++mit;
      } // end if useMask

      if (insideMask)
      {

        // count number of votes for the labels
        for( unsigned int i = 0; i < numberOfInputs; ++i)
        {
          const InputPixelType label = it[i].Get();
          W[label] += this->m_ObserverTrust( i );       
        }

        /** normalize: */
        WeightsType sumW = W.sum();
        if ( sumW )
        {
          W /= sumW;
        }

        /** now determine the label with the maximum W, i.e., 
        * determine the label with the most votes for this pixel */
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

        /** Update the confusion matrix */
        if ( generateConfusionMatrix )
        {
          for ( unsigned int i = 0; i < numberOfInputs; ++i )
          {
            const InputPixelType label = it[i].Get();
            for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
            {
              this->m_ConfusionMatrixArrays[threadId][i][label][ci] += W[ci];
            }
          }
        } // end if generateConfusionMatrix
  
        /** Next input pixel */
        for( unsigned int i = 0; i < numberOfInputs; ++i)
        {
          ++(it[i]);
        }
      } // end if insideMask

      /** copy the W values into the probabilistic segmentation images
       * and move the psit iterators */
      if ( generateProbSeg )
      {
        for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
        {
          psit[ci].Set( W[ci] );
          ++(psit[ci]);
        }
      } // end if generateProbSeg

    } // end loop over output pixels

  } // end ThreadedGenerateData


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    LabelVoting2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::AfterThreadedGenerateData ()
  {
    this->Superclass::AfterThreadedGenerateData();

    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    if ( this->GetGenerateConfusionMatrix() )
    {

      /** Add the confusion matrix arrays of all threads */
      for ( unsigned int t = 0; t < this->GetNumberOfThreads(); ++t )
      {
        for ( unsigned int i = 0; i < numberOfInputs; ++i )
        {
          this->m_ConfusionMatrixArray[i] +=
            this->m_ConfusionMatrixArrays[t][i];            
        }
      } // end for t

      /** Normalize each column of each confusion matrix */
      for ( unsigned int i = 0; i < numberOfInputs; ++i)
      {
        // compute sum over all output classifications
        for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci ) 
        {
          WeightsType sumW = this->m_ConfusionMatrixArray[i][0][ci]; 
          for ( InputPixelType j = 1; j < this->m_NumberOfClasses; ++j )
          {
            sumW += this->m_ConfusionMatrixArray[i][j][ci];
          }

          // normalize with sumW for each class ci
          if ( sumW )
          {
            this->m_ConfusionMatrixArray[i].scale_column(ci, 1.0/sumW);
          }
        } // end for ci
      } // end for i
                  
    } // end if GetGenerateConfusionMatrix

  } // end AfterThreadedGenerateData

} // end namespace itk

#endif
