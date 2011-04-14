#ifndef _itkMultiLabelSTAPLE2ImageFilter_txx
#define _itkMultiLabelSTAPLE2ImageFilter_txx

#include "itkMultiLabelSTAPLE2ImageFilter.h"
#include "itkLabelVoting2ImageFilter.h"

#include "vnl/vnl_math.h"

namespace itk
{

  template <typename TInputImage, typename TOutputImage, typename TWeights>
    MultiLabelSTAPLE2ImageFilter<TInputImage, TOutputImage, TWeights>
    ::MultiLabelSTAPLE2ImageFilter()
  {
    this->m_HasPriorPreference = false;
    this->m_HasPriorProbabilities = false;
    this->m_HasPriorProbabilityImageArray = false;
    this->m_HasNumberOfClasses = false;
    this->m_HasMaximumNumberOfIterations = false;
    this->m_HasObserverTrust = false;

    this->m_TerminationUpdateThreshold = 1e-5;
    this->m_ElapsedIterations = 0;
    this->m_MaximumConfusionMatrixElementUpdate = 0.0;
    this->m_GenerateProbabilisticSegmentations = false;
    this->m_NumberOfClasses = 2;
    this->m_MaskImage = 0;
    this->m_InitializeWithMajorityVoting = false;
  } // end constructor


  template <typename TInputImage, typename TOutputImage, typename TWeights>
    void
    MultiLabelSTAPLE2ImageFilter<TInputImage, TOutputImage, TWeights>
    ::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
     //etc.
  } // end PrintSelf


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    typename TInputImage::PixelType
    MultiLabelSTAPLE2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::ComputeMaximumInputValue()
  {
    /** compute the maximum class label from the input data */
    InputPixelType maxLabel = 0;
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    for ( unsigned int k = 0; k < numberOfInputs; ++k )
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
    MultiLabelSTAPLE2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::AllocateConfusionMatrixArray()
  {
    /**  we need one confusion matrix for every input */
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    this->m_ConfusionMatrixArray.clear();
    this->m_UpdatedConfusionMatrixArray.clear();

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
      this->m_ConfusionMatrixArray.push_back
        ( ConfusionMatrixType( this->m_NumberOfClasses, this->m_NumberOfClasses ) );
      this->m_UpdatedConfusionMatrixArray.push_back
        ( ConfusionMatrixType( this->m_NumberOfClasses, this->m_NumberOfClasses ) );
    }
  } // end AllocateConfusionMatrixArray


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    MultiLabelSTAPLE2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::InitializeConfusionMatrixArray()
  {
    const unsigned int numberOfInputs = this->GetNumberOfInputs();

    if ( this->GetInitializeWithMajorityVoting() )
    {
      typedef itk::LabelVoting2ImageFilter<
        InputImageType, OutputImageType, WeightsType>  VotingFilterType;
      typename VotingFilterType::Pointer voting = VotingFilterType::New();
      for (unsigned int i = 0; i < numberOfInputs; ++i)
      {
        voting->SetInput( i, this->GetInput(i) );
      }
      voting->SetNumberOfClasses( this->GetNumberOfClasses() );
      voting->SetObserverTrust( this->GetObserverTrust() );
      voting->SetMaskImage( this->GetMaskImage() );
      voting->SetPriorPreference( this->GetPriorPreference() );
      voting->SetGenerateConfusionMatrix( true );
      voting->SetGenerateProbabilisticSegmentations( false );
      voting->Update();
      for (unsigned int i = 0; i < numberOfInputs; ++i)
      {
        this->m_ConfusionMatrixArray[i] = voting->GetConfusionMatrix(i);
      }

    }
    else
    {
      /** Set the trust factor on the diagonal.
      * All off-diagonal elements get an equal value,
      * such that the columns are normalized */
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        this->m_ConfusionMatrixArray[k].Fill( 0.0 );
        const WeightsType trust = this->m_ObserverTrust[k];
        const WeightsType invtrust = ( 1.0 - trust ) / static_cast<WeightsType>(this->m_NumberOfClasses-1);
        for ( InputPixelType inLabel = 0; inLabel < this->m_NumberOfClasses; ++inLabel )
        {
          for ( OutputPixelType outLabel = 0; outLabel < this->m_NumberOfClasses; ++outLabel )
          {
            if ( outLabel == inLabel )
            {
              this->m_ConfusionMatrixArray[k][inLabel][outLabel] = trust;
            }
            else
            {
              this->m_ConfusionMatrixArray[k][inLabel][outLabel] = invtrust;
            }
          } // end for outLabel
        } // end for inLabel
      } // end for k
    }
  } // end InitializeConfusionMatrixArray


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    MultiLabelSTAPLE2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::InitializePriorProbabilities()
  {
    /** test for user-defined prior probabilities and create an estimated one if
     * none exists */
    if ( this->m_HasPriorProbabilityImageArray )
    {
      if ( this->m_PriorProbabilityImageArray.size() < this->m_NumberOfClasses )
      {
        itkExceptionMacro("m_PriorProbabilityImageArray has wrong size: " << m_PriorProbabilityImageArray.size() << "; should have at least " << this->m_NumberOfClasses << "elements!");
      }
    }
    else if ( this->m_HasPriorProbabilities )
    {
      if ( this->m_PriorProbabilities.GetSize() < this->m_NumberOfClasses )
      {
        itkExceptionMacro ("m_PriorProbabilities array has wrong size " << m_PriorProbabilities << "; should have at least " << this->m_NumberOfClasses << "elements!" );
      }
    }
    else
    {
      this->m_PriorProbabilities.SetSize( this->m_NumberOfClasses );
      this->m_PriorProbabilities.Fill( 0.0 );

      /** Set up the mask iterator */
      MaskConstIteratorType mit;
      const bool useMask = this->m_MaskImage.IsNotNull();
      const MaskPixelType zeroMaskPixel = itk::NumericTraits<MaskPixelType>::Zero;
      if ( useMask )
      {
        mit = MaskConstIteratorType(
          this->m_MaskImage, this->GetOutput()->GetRequestedRegion() );
      }

      /** Loop over all input images to estimate the prior probabilities */
      const unsigned int numberOfInputs = this->GetNumberOfInputs();
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        InputConstIteratorType in = InputConstIteratorType
          ( this->GetInput( k ), this->GetOutput()->GetRequestedRegion() );

        const WeightsType trust = this->GetObserverTrust()[k];

        if ( useMask )
        {
          mit.GoToBegin();
          for ( in.GoToBegin(); ! in.IsAtEnd(); ++in )
          {
            if ( mit.Get() != zeroMaskPixel  )
            {
              this->m_PriorProbabilities[ in.Get() ] += trust;
            }
            ++mit;
          }
        } // end use mask
        else
        {
          for ( in.GoToBegin(); ! in.IsAtEnd(); ++in )
          {
            this->m_PriorProbabilities[ in.Get() ] += trust;
          }
        } // end no mask
      }

      /** Normalize */
      WeightsType sumP = this->m_PriorProbabilities.sum();
      if ( sumP )
      {
        this->m_PriorProbabilities /= sumP;
      }
    } // end else: if no prior probs are supplied by the user

  } // end InitializePriorProbabilities


  template< typename TInputImage, typename TOutputImage, typename TWeights >
    void
    MultiLabelSTAPLE2ImageFilter< TInputImage, TOutputImage, TWeights >
    ::GenerateData()
  {
    typedef Array<WeightsType>                  WType;
    typedef std::vector<InputConstIteratorType> InputConstIteratorArrayType;
    typedef std::vector<ProbIteratorType>       ProbIteratorArrayType;
    typedef std::vector<ProbConstIteratorType>  ProbConstIteratorArrayType;

    /** Initialize some variables */
    this->m_MaximumConfusionMatrixElementUpdate = 0.0;
    this->m_ElapsedIterations = 0;
    const bool generateProbSeg =
      this->GetGenerateProbabilisticSegmentations();
    const bool useMask = this->m_MaskImage.IsNotNull();
    const unsigned int numberOfInputs = this->GetNumberOfInputs();
    WType W( this->m_NumberOfClasses );
    OutputImagePointer output = this->GetOutput();
    this->AllocateOutputs();

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
      this->m_ObserverTrust.Fill(0.99999);
    }

    /** Determine the least preferred label */
    OutputPixelType leastPreferredLabel = 0;
    for (unsigned int i= 0; i< this->m_NumberOfClasses; ++i)
    {
      if ( this->m_PriorPreference[i] == (this->m_NumberOfClasses-1) )
      {
        leastPreferredLabel = i;
      }
    }

    /** Initialize prior probabilities and confusion matrices */
    this->InitializePriorProbabilities();
    this->AllocateConfusionMatrixArray();
    this->InitializeConfusionMatrixArray();

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

    /** create and initialize all input image iterators */
    InputConstIteratorArrayType it(numberOfInputs);
    for ( unsigned int k = 0; k < numberOfInputs; ++k )
    {
      it[k] = InputConstIteratorType
        ( this->GetInput( k ), output->GetRequestedRegion() );
    }

    /** Create and initialize the prior prob image iterators */
    ProbConstIteratorArrayType pit;
    if ( this->m_HasPriorProbabilityImageArray )
    {
      pit = ProbConstIteratorArrayType( this->m_NumberOfClasses );
      for ( unsigned int k = 0; k < this->m_NumberOfClasses; ++k )
      {
        pit[k] = ProbConstIteratorType(
          this->m_PriorProbabilityImageArray[k], output->GetRequestedRegion() );
      }
    }

    /** Create and initialize the output probabilistic segmentation image iterators */
    ProbIteratorArrayType psit;
    if ( generateProbSeg )
    {
      psit = ProbIteratorArrayType( this->m_NumberOfClasses );
      for ( unsigned int k = 0; k < this->m_NumberOfClasses; ++k )
      {
        psit[k] = ProbIteratorType(
          this->m_ProbabilisticSegmentationArray[k], output->GetRequestedRegion() );
      }
    }

    /** Create and initialize the mask iterator */
    MaskConstIteratorType mit;
    const MaskPixelType zeroMaskPixel = itk::NumericTraits<MaskPixelType>::Zero;
    if ( useMask )
    {
      mit = MaskConstIteratorType(
        this->m_MaskImage, this->GetOutput()->GetRequestedRegion() );
    }

    /** Create and initialize the output iterator */
    OutputIteratorType out = OutputIteratorType( output, output->GetRequestedRegion() );

    /** Start iterating! */
    while (  ( !this->m_HasMaximumNumberOfIterations ) ||
             ( this->m_ElapsedIterations < this->m_MaximumNumberOfIterations )   )
    {
      /** reset updated confusion matrix */
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        this->m_UpdatedConfusionMatrixArray[k].Fill( 0.0 );
      }

      /** Loop over voxels and do the E and M step
       * use it[0] as indicator for image pixel count */
      while ( ! it[0].IsAtEnd() )
      {
        if (useMask)
        {
          if ( mit.Get() == zeroMaskPixel )
          {
            /** Move all iterators to the next pixel and go to the
             * next cycle of the while loop */
            ++mit;
            for ( unsigned int k = 0; k < numberOfInputs; ++k )
            {
              ++(it[k]);
            }
            if ( this->m_HasPriorProbabilityImageArray )
            {
              for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
              {
                ++(pit[ci]);
              }
            }
            continue;
          } // end if mit==zero
          /** Move the iterator to the next pixel */
          ++mit;
        } // end if useMask

        /** the following is the E step for one pixel, only performed when this
         * pixel is inside the mask */
        if ( this->m_HasPriorProbabilityImageArray )
        {
          for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
          {
            W[ci] = pit[ci].Get();
            /** move already to next pixel */
            ++(pit[ci]);
          }
        }
        else
        {
          W = this->m_PriorProbabilities;
        }

        for ( unsigned int k = 0; k < numberOfInputs; ++k )
        {
          const InputPixelType j = it[k].Get();
          for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
          {
            W[ci] *= this->m_ConfusionMatrixArray[k][j][ci];
          }
        }

        // the following is the M step
        /** normalize: */
        WeightsType sumW = W.sum();
        if ( sumW )
        {
          W /= sumW;
        }

        for ( unsigned int k = 0; k < numberOfInputs; ++k )
        {
          const InputPixelType j = it[k].Get();
          for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
          {
            this->m_UpdatedConfusionMatrixArray[k][j][ci] += W[ci];
          }

          // we're now done with this input pixel, so update.
          ++(it[k]);
        }

      } // end loop over voxels

      /** Normalize matrix elements of each of the updated confusion matrices
       * with sum over all expert decisions. */
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        // compute sum over all output classifications
        for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
        {
          WeightsType sumW = this->m_UpdatedConfusionMatrixArray[k][0][ci];
          for ( InputPixelType j = 1; j < this->m_NumberOfClasses; ++j )
          {
            sumW += this->m_UpdatedConfusionMatrixArray[k][j][ci];
          }

          // normalize with sumW for each class ci
          if ( sumW )
          {
            this->m_UpdatedConfusionMatrixArray[k].scale_column(ci, 1.0/sumW);
          }
        }
      } // end for k: end normalization of updated confusion matrix

      // now we're applying the update to the confusion matrices and compute the
      // maximum parameter change in the process, to check for convergence.
      WeightsType maximumUpdate = 0;
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        const WeightsType maximumUpdate_k = static_cast<WeightsType>(
          (this->m_UpdatedConfusionMatrixArray[k]-this->m_ConfusionMatrixArray[k]).array_inf_norm() );
        maximumUpdate = vnl_math_max( maximumUpdate, maximumUpdate_k );

        this->m_ConfusionMatrixArray[k] = this->m_UpdatedConfusionMatrixArray[k];
      }
      this->m_MaximumConfusionMatrixElementUpdate = maximumUpdate;

      /** We have finished this iteration */
      ++(this->m_ElapsedIterations);

      /** reset all input iterators to start, to be prepared for next iteration
       * or for generating the final segmentation, if the current iteration
       * happened to be the last one */
      if (useMask)
      {
        mit.GoToBegin();
      }
      for ( unsigned int k = 0; k < numberOfInputs; ++k )
      {
        it[k].GoToBegin();
      }
      if ( this->m_HasPriorProbabilityImageArray )
      {
        for ( unsigned int k = 0; k < this->m_NumberOfClasses; ++k )
        {
          pit[k].GoToBegin();
        }
      }

      /** Allow user to do something */
      this->InvokeEvent( IterationEvent() );
      if( this->GetAbortGenerateData() )
      {
        this->ResetPipeline();
        // fake this to cause termination; we could really just break
        maximumUpdate = 0;
      }

      // if all confusion matrix parameters changes by less than the defined
      // threshold, we're done.
      if ( maximumUpdate < this->m_TerminationUpdateThreshold )
        break;

    } // end for ( iteration )


    /** now we'll build the combined output image based on the estimated
     * confusion matrices */
    for ( out.GoToBegin(); !out.IsAtEnd(); ++out )
    {
      OutputPixelType winningLabel = leastPreferredLabel;

      bool insideMask = true;
      if (useMask)
      {
        /** For pixels outside the mask use the decision
         * of th first observer */
        if ( mit.Get() == zeroMaskPixel )
        {
          insideMask = false;
          W.Fill( 0.0 );
          winningLabel = it[0].Get();
          W[ winningLabel ] = 1.0;
          /** Set the winning label to the output pixel */
          out.Set( winningLabel );
          /** move the pit and it iterators */
          if ( this->m_HasPriorProbabilityImageArray )
          {
            for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
            {
              ++(pit[ci]);
            }
          }
          for ( unsigned int k = 0; k < numberOfInputs; ++k )
          {
            ++(it[k]);
          }
        } // if mit==zero
        ++mit;
      } // end if useMask

      if ( insideMask )
      {
        // basically, we'll repeat the E step from above
        if ( this->m_HasPriorProbabilityImageArray )
        {
          for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
          {
            W[ci] = pit[ci].Get();
            ++(pit[ci]);
          }
        }
        else
        {
          W = this->m_PriorProbabilities;
        }

        for ( unsigned int k = 0; k < numberOfInputs; ++k )
        {
          const InputPixelType j = it[k].Get();
          for ( OutputPixelType ci = 0; ci < this->m_NumberOfClasses; ++ci )
          {
            W[ci] *= this->m_ConfusionMatrixArray[k][j][ci];
          }
          ++(it[k]);
        }

        /** normalize: */
        WeightsType sumW = W.sum();
        if ( sumW )
        {
          W /= sumW;
        }

        // now determine the label with the maximum W
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
      }

    } // end loop over output pixels

  } // end GenerateData

} // end namespace itk

#endif
