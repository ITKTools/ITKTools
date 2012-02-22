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
#ifndef __itkLabelVoting2ImageFilter_h_
#define __itkLabelVoting2ImageFilter_h_

#include "itkImage.h"
#include "itkImageToImageFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

#include <vector>
#include "itkArray.h"
#include "itkArray2D.h"

namespace itk
{
  /** \class LabelVoting2ImageFilter
  *
  * \brief This filter performs pixelwise voting among an arbitrary number
  * of input images, where each of them represents a segmentation of the same
  * scene (i.e., image).
  *
  * Label voting is a simple method of classifier combination applied to
  * image segmentation. Typically, the accuracy of the combined segmentation
  * exceeds the accuracy of any of the input segmentations. Voting is therefore
  * commonly used as a way of boosting segmentation performance.
  *
  * The use of label voting for combination of multiple segmentations is
  * described in
  *
  * T. Rohlfing and C. R. Maurer, Jr., "Multi-classifier framework for
  * atlas-based image segmentation," Pattern Recognition Letters, 2005.
  *
  * \par INPUTS
  * All input volumes to this filter must be segmentations of an image,
  * that is, they must have discrete pixel values where each value represents
  * a different segmented object.
  *
  * Input volumes must all contain the same size RequestedRegions. Not all
  * input images must contain all possible labels, but all label values must
  * have the same meaning in all images.
  *
  * \par OUTPUTS
  * The voting filter produces a single output volume. Each output pixel
  * contains the label that occured most often among the labels assigned to
  * this pixel in all the input volumes, that is, the label that received the
  * maximum number of "votes" from the input pixels.. If the maximum number of
  * votes is not unique, i.e., if more than one label have a maximum number of
  * votes, the prior preferences are used to select a winning label. On request,
  * the probabilistic segmentation can also be produced.
  *
  * \par PARAMETERS
  *
  *
  * \author Torsten Rohlfing, SRI International, Neuroscience Program
  * Modified by Stefan Klein.
  *
  */

  template <typename TInputImage, typename TOutputImage = TInputImage, typename TWeights = float>
  class LabelVoting2ImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
  {
  public:
    /** Standard class typedefs. */
    typedef LabelVoting2ImageFilter Self;
    typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods) */
    itkTypeMacro(LabelVoting2ImageFilter, ImageToImageFilter);

    /** Extract some information from the image types.  Dimensionality
    * of the two images is assumed to be the same. */
    typedef typename TOutputImage::PixelType OutputPixelType;
    typedef typename TInputImage::PixelType InputPixelType;

    /** Extract some information from the image types.  Dimensionality
    * of the two images is assumed to be the same. */
    itkStaticConstMacro(InputImageDimension, unsigned int,
      TInputImage::ImageDimension );
    itkStaticConstMacro(ImageDimension, unsigned int,
      TOutputImage::ImageDimension);

    /** Image typedef support */
    typedef TInputImage  InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::ConstPointer InputImagePointer;
    typedef typename OutputImageType::Pointer     OutputImagePointer;

    /** Superclass typedefs. */
    typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

    /** Various typedefs */
    typedef TWeights WeightsType;
    typedef Array2D<WeightsType>                    ConfusionMatrixType;
    typedef Image<
      WeightsType,
      ::itk::GetImageDimension<
      InputImageType>::ImageDimension>              ProbabilityImageType;
    typedef typename ProbabilityImageType::Pointer  ProbabilityImagePointer;

    typedef Array<WeightsType>                      ObserverTrustType;
    typedef std::vector<ProbabilityImagePointer>    ProbabilisticSegmentationArrayType;
    typedef Array<OutputPixelType>                  PriorPreferenceType;

    /** Typedefs for mask support */
    typedef InputImageType                          MaskImageType;
    typedef typename MaskImageType::Pointer         MaskImagePointer;
    typedef typename MaskImageType::PixelType       MaskPixelType;

    /** Iterator types. */
    typedef ImageRegionConstIterator< InputImageType >  InputConstIteratorType;
    typedef ImageRegionIterator< OutputImageType >      OutputIteratorType;
    typedef ImageRegionIterator<
      ProbabilityImageType >                            ProbIteratorType;
    typedef ImageRegionConstIterator< MaskImageType >   MaskConstIteratorType;

    /** Set/get/unset prior preference; a scalar for each class indicating
    * the preference in case of undecided pixels. The lower the number,
    * the more preference. If not provided, the class numbers are assumed
    * as preferences. Make sure no duplicate values exist, and no
    * numbers higher than the numberOfClasses-1. */
    virtual void SetPriorPreference( const PriorPreferenceType& ppa )
    {
      this->m_PriorPreference = ppa;
      this->m_HasPriorPreference = true;
      this->Modified();
    }

    itkGetConstReferenceMacro( PriorPreference, PriorPreferenceType );

    virtual void UnsetPriorPreference( void )
    {
      if( this->m_HasPriorPreference )
      {
        this->m_HasPriorPreference = false;
        this->Modified();
      }
    }

    /** Set/get/unset observer trust factors.  */
    virtual void SetObserverTrust( const ObserverTrustType& ot )
    {
      this->m_ObserverTrust = ot;
      this->m_HasObserverTrust = true;
      this->Modified();
    }

    itkGetConstReferenceMacro( ObserverTrust, ObserverTrustType);

    virtual void UnsetObserverTrust( void )
    {
      if( this->m_HasObserverTrust )
      {
        this->m_HasObserverTrust = false;
        this->Modified();
      }
    }

    /** Set/unset/get the number of classes. If you don't set it, it is
    * automatically determined from the input segmentations */
    virtual void SetNumberOfClasses(InputPixelType arg)
    {
      this->m_NumberOfClasses = arg;
      this->m_HasNumberOfClasses = true;
      this->Modified();
    }

    virtual void UnsetNumberOfClasses( void )
    {
      if( this->m_HasNumberOfClasses )
      {
        this->m_HasNumberOfClasses = false;
        this->Modified();
      }
    }

    itkGetConstMacro( NumberOfClasses, InputPixelType );


    /** Setting: turn on/off to whether a probabilistic segmentation
    * is generated; default: false */
    itkSetMacro(GenerateProbabilisticSegmentations, bool);
    itkGetConstMacro(GenerateProbabilisticSegmentations, bool);

    /** Get the probabilistic segmentations. Only valid when
    * SetGenerateProbabilisticSegmentations(true) has been
    * invoked before updating this filter. */
    virtual const ProbabilisticSegmentationArrayType &
      GetProbabilisticSegmentationArray( void ) const
    {
      return this->m_ProbabilisticSegmentationArray;
    }

    /** If you have inspected the probabilistic segmentations and want to get rid
    * of those float images sitting in your memory, call this function */
    virtual void CleanProbabilisticSegmentations( void )
    {
      if( this->m_ProbabilisticSegmentationArray.size() > 0 )
      {
        this->m_ProbabilisticSegmentationArray =
          ProbabilisticSegmentationArrayType(0);
        this->Modified();
      }
    }

    /** Set/Get a mask image; If a mask is supplied, only pixels that are
     * within the mask are used in the staple procedure. The output
     * at pixels outside the mask will be equal to that of the first
     * observer */
    itkSetObjectMacro( MaskImage, MaskImageType );
    itkGetObjectMacro( MaskImage, MaskImageType );

    /** Setting: turn on/off to whether a confusion matrix
     * is generated; default: false */
    itkSetMacro(GenerateConfusionMatrix, bool);
    itkGetConstMacro(GenerateConfusionMatrix, bool);

    /** Get confusion matrix for the i-th input segmentation. */
    virtual const ConfusionMatrixType & GetConfusionMatrix( const unsigned int i ) const
    {
      return this->m_ConfusionMatrixArray[ i ];
    }

#ifdef ITK_USE_CONCEPT_CHECKING
    /** Begin concept checking */
    itkConceptMacro(InputConvertibleToOutputCheck,
      (Concept::Convertible<InputPixelType, OutputPixelType>));
    itkConceptMacro(IntConvertibleToInputCheck,
      (Concept::Convertible<int, InputPixelType>));
    itkConceptMacro(SameDimensionCheck,
      (Concept::SameDimension<InputImageDimension, ImageDimension>));
    itkConceptMacro(InputConvertibleToUnsignedIntCheck,
      (Concept::Convertible<InputPixelType, unsigned int>));
    itkConceptMacro(IntConvertibleToOutputPixelType,
      (Concept::Convertible<int, OutputPixelType>));
    itkConceptMacro(InputPlusIntCheck,
      (Concept::AdditiveOperators<InputPixelType, int>));
    itkConceptMacro(InputIncrementDecrementOperatorsCheck,
      (Concept::IncrementDecrementOperators<InputPixelType>));
    itkConceptMacro(OutputOStreamWritableCheck,
      (Concept::OStreamWritable<OutputPixelType>));
    /** End concept checking */
#endif

  protected:
    LabelVoting2ImageFilter();
    virtual ~LabelVoting2ImageFilter() {}

    /** Determine maximum label value in all input images and initialize global data.*/
    void BeforeThreadedGenerateData ();
    void AfterThreadedGenerateData ();
    void ThreadedGenerateData
      ( const OutputImageRegionType &outputRegionForThread, ThreadIdType threadId);

    void PrintSelf(std::ostream&, Indent) const;

    /** The number of different labels found in the input segmentations */
    InputPixelType m_NumberOfClasses;

    /** The label with the highest priorPreference number */
    OutputPixelType m_LeastPreferredLabel;

    /** Variables that store whether the a specific parameter has been
    * set by the user */
    bool m_HasObserverTrust;
    bool m_HasNumberOfClasses;
    bool m_HasPriorPreference;

    typedef std::vector<ConfusionMatrixType> ConfusionMatrixArrayType;
    typedef std::vector<ConfusionMatrixArrayType> ConfusionMatrixArrayArrayType;

    /** These variables could in principle be accessed via the member functions,
    * but for inheriting classes this would be annoying. So, make them protected. */
    ObserverTrustType                  m_ObserverTrust;
    ProbabilisticSegmentationArrayType m_ProbabilisticSegmentationArray;
    PriorPreferenceType                m_PriorPreference;
    ConfusionMatrixArrayType           m_ConfusionMatrixArray;

    /** For multithreading: */
    ConfusionMatrixArrayArrayType      m_ConfusionMatrixArrays;

    /** Determine maximum value among all input images' pixels */
    virtual InputPixelType ComputeMaximumInputValue();

    /** Allocate confusion matrix array(s) */
    virtual void AllocateConfusionMatrixArray();

  private:
    LabelVoting2ImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    /** Settings that can be accessed via the set/get member functions */
    bool m_GenerateProbabilisticSegmentations;
    bool m_GenerateConfusionMatrix;
    MaskImagePointer m_MaskImage;


  };

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelVoting2ImageFilter.txx"
#endif

#endif // end #ifndef __itkLabelVoting2ImageFilter_h_
