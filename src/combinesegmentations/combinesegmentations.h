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
#ifndef __combinesegmentations_h_
#define __combinesegmentations_h_

#include "ITKToolsBase.h"

#include <string>
#include <vector>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSTAPLEImageFilter.h"
#include "itkLabelVoting2ImageFilter.h"
#include "itkMultiLabelSTAPLEImageFilter.h"
#include "itkMultiLabelSTAPLE2ImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNaryUnequalityTestImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkChangeLabelImageFilter.h"
#include "itkMultiThreader.h"


/** \class ITKToolsCombineSegmentationsBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCombineSegmentationsBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCombineSegmentationsBase()
  {
    this->m_HardOutputFileName = "";
    this->m_ConfusionOutputFileName = "";
    this->m_NumberOfClasses = 2;
    this->m_TerminationThreshold = 1e-5;
    this->m_CombinationMethod = "MULTISTAPLE2";
    this->m_UseMask = false;
    this->m_MaskDilationRadius = 1;
    this->m_UseCompression = false;
  };
  /** Destructor. */
  ~ITKToolsCombineSegmentationsBase(){};

  /** Input member parameters. */
  std::vector< std::string >  m_InputSegmentationFileNames;
  std::vector< std::string >  m_PriorProbImageFileNames;
  std::vector< std::string >  m_SoftOutputFileNames;
  std::string                 m_HardOutputFileName;
  std::string                 m_ConfusionOutputFileName;
  unsigned char               m_NumberOfClasses;
  std::vector< float >        m_PriorProbs;
  std::vector< float >        m_Trust;
  float                       m_TerminationThreshold;
  std::string                 m_CombinationMethod;
  bool                        m_UseMask;
  unsigned int                m_MaskDilationRadius;
  std::vector< unsigned int > m_PrefOrder;
  std::vector< unsigned int > m_InValues;
  std::vector< unsigned int > m_OutValues;
  bool                        m_UseCompression;

}; // end class ITKToolsCombineSegmentationsBase


/** \class ITKToolsCombineSegmentations
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCombineSegmentations : public ITKToolsCombineSegmentationsBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCombineSegmentations Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCombineSegmentations(){};
  ~ITKToolsCombineSegmentations(){};

  /** Run function. */
  void Run( void )
  {
    //typedef unsigned char   LabelPixelType;
    typedef TComponentType  LabelPixelType;
    typedef float           ProbPixelType;
    typedef float           ConfusionMatrixPixelType;

    typedef itk::Image< LabelPixelType, VDimension >    LabelImageType;
    typedef itk::Image< ProbPixelType, VDimension >     ProbImageType;
    typedef itk::Image< ConfusionMatrixPixelType, 3 >   ConfusionMatrixImageType;

    typedef typename LabelImageType::RegionType         RegionType;

    typedef typename LabelImageType::Pointer            LabelImagePointer;
    typedef typename ProbImageType::Pointer             ProbImagePointer;
    typedef typename ConfusionMatrixImageType::Pointer  ConfusionMatrixImagePointer;

    typedef itk::ImageFileReader< LabelImageType >    LabelImageReaderType;
    typedef itk::ImageFileReader< ProbImageType >     ProbImageReaderType;
    typedef itk::ImageFileWriter< LabelImageType >    LabelImageWriterType;
    typedef itk::ImageFileWriter< ProbImageType >     ProbImageWriterType;
    typedef itk::ImageFileWriter<
      ConfusionMatrixImageType >                      ConfusionMatrixImageWriterType;

    typedef itk::ChangeLabelImageFilter<
      LabelImageType, LabelImageType >                RelabelFilterType;

    typedef itk::ProcessObject                        SegmentationCombinerType;
    typedef itk::STAPLEImageFilter<
      LabelImageType, ProbImageType >                 STAPLEType;
    typedef itk::LabelVoting2ImageFilter<
      LabelImageType, LabelImageType, ProbPixelType > LabelVotingType;
    typedef itk::MultiLabelSTAPLEImageFilter<
      LabelImageType, LabelImageType, ProbPixelType > MultiLabelSTAPLEType;
    typedef itk::MultiLabelSTAPLE2ImageFilter<
      LabelImageType, LabelImageType, ProbPixelType > MultiLabelSTAPLE2Type;

    typedef itk::InvertIntensityImageFilter<
      ProbImageType, ProbImageType >                  InverterType;
    typedef itk::BinaryThresholdImageFilter<
      ProbImageType, LabelImageType >                 ThresholderType;

    typedef itk::ImageRegionIteratorWithIndex<
      ConfusionMatrixImageType >                      ConfusionMatrixImageIteratorType;
    typedef typename
      ConfusionMatrixImageType::IndexType             ConfusionIndexType;
    typedef typename
      ConfusionMatrixImageType::SizeType              ConfusionSizeType;

    typedef typename
      MultiLabelSTAPLEType::PriorProbabilitiesType   MultiSTAPLEPriorProbsType;
    typedef typename
      MultiLabelSTAPLEType::WeightsType              MultiSTAPLEWeightsType;
    typedef typename
      MultiLabelSTAPLEType::ConfusionMatrixType      MultiSTAPLEConfusionMatrixType;
    typedef typename
      MultiLabelSTAPLE2Type::ObserverTrustType       MultiSTAPLE2ObserverTrustType;
    typedef typename MultiLabelSTAPLE2Type::
      PriorProbabilityImageArrayType                 MultiSTAPLE2PriorProbImageArrayType;
    typedef typename MultiLabelSTAPLE2Type::
      MaskImageType                                  MaskImageType;
    typedef typename MaskImageType::PixelType        MaskPixelType;
    typedef typename
      MultiLabelSTAPLE2Type::PriorPreferenceType     PriorPreferenceType;

    typedef itk::NaryUnequalityTestImageFilter<
      LabelImageType, MaskImageType>                 MaskGeneratorType;

    typedef itk::BinaryBallStructuringElement<
      MaskPixelType, VDimension >                    StructuringElementType;
    typedef typename
      StructuringElementType::RadiusType             KernelRadiusType;
    typedef itk::BinaryDilateImageFilter<
      MaskImageType,
      MaskImageType,
      StructuringElementType >                       DilateFilterType;

    typedef std::vector< LabelImagePointer >         LabelImageArrayType;
    typedef std::vector< ProbImagePointer >          ProbImageArrayType;

    /** Declare some variables */
    unsigned int numberOfObservers = 0;
    SegmentationCombinerType::Pointer segmentationCombiner = nullptr;
    LabelImageArrayType labelImageArray;
    ProbImageArrayType priorProbImageArray;
    ProbImageArrayType softSegmentationArray;
    LabelImagePointer hardSegmentation = nullptr;
    ConfusionMatrixImagePointer confusionMatrixImage = nullptr;

    /** Initialize some variables */
    numberOfObservers = this->m_InputSegmentationFileNames.size();
    labelImageArray.resize( numberOfObservers );
    softSegmentationArray.resize( this->m_NumberOfClasses );
    priorProbImageArray.resize( this->m_NumberOfClasses );

    /** Read the input label images */
    RegionType lastRegion;
    bool relabel = ( this->m_InValues.size() > 0 );
    std::cout << "Reading (and possibly relabeling) input segmentations..." << std::endl;
    for( unsigned int i = 0; i < numberOfObservers; ++i )
    {
      typename LabelImageReaderType::Pointer labelImageReader =
        LabelImageReaderType::New();
      labelImageReader->SetFileName( this->m_InputSegmentationFileNames[ i ].c_str() );
      labelImageReader->Update();

      /** Check size. */
      RegionType region = labelImageReader->GetOutput()->GetLargestPossibleRegion();
      if( i > 0 && region != lastRegion )
      {
        std::cerr << "\nERROR: input label images are not of the same size!" << std::endl;
        return;
      }
      lastRegion = region;

      /** Relabel? */
      if( relabel )
      {
        typename RelabelFilterType::Pointer relabeler = RelabelFilterType::New();
        relabeler->SetInput( labelImageReader->GetOutput() );
        for( unsigned int lab = 0; lab < this->m_InValues.size(); ++lab )
        {
          LabelPixelType labin = static_cast<LabelPixelType>( this->m_InValues[lab] );
          LabelPixelType labout = static_cast<LabelPixelType>( this->m_OutValues[lab] );
          relabeler->SetChange( labin, labout );
        }
        relabeler->Update();
        labelImageArray[ i ] = relabeler->GetOutput();
      } // end relabel
      else
      {
        labelImageArray[ i ] = labelImageReader->GetOutput();
      }
    }
    std::cout << "Done reading input segmentations." << std::endl;

    /** Read the prior probability images, if supplied */
    if( this->m_PriorProbImageFileNames.size() == this->m_NumberOfClasses )
    {
      std::cout << "Reading prior probability images..." << std::endl;
      for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
      {
        typename ProbImageReaderType::Pointer probImageReader =
          ProbImageReaderType::New();
        probImageReader->SetFileName( this->m_PriorProbImageFileNames[ i ].c_str() );
        probImageReader->Update();
        priorProbImageArray[ i ] = probImageReader->GetOutput();
      }
      std::cout << "Done reading prior probability images." << std::endl;
    }

    /** Prepare the confusion matrix */
    if( this->m_ConfusionOutputFileName != "" )
    {
      ConfusionSizeType csize;
      csize[0] = this->m_NumberOfClasses;
      csize[1] = this->m_NumberOfClasses;
      csize[2] = numberOfObservers;
      confusionMatrixImage = ConfusionMatrixImageType::New();
      confusionMatrixImage->SetRegions( csize );
      confusionMatrixImage->Allocate();
    }

    /** Combine the label images */
    if( this->m_CombinationMethod == "STAPLE" )
    {
      /** run STAPLE */
      typename STAPLEType::Pointer staple = STAPLEType::New();
      segmentationCombiner = staple;
      staple->SetForegroundValue( 1 );
      for( unsigned int i = 0; i < numberOfObservers; ++i )
      {
        staple->SetInput(i, labelImageArray[ i ]);
      }
      if( this->m_PriorProbs.size() == 2 )
      {
        staple->SetConfidenceWeight( this->m_PriorProbs[1] );
      }
      std::cout << "Performing STAPLE algorithm..." << std::endl;
      staple->Update();
      std::cout << "Done performing STAPLE algorithm." << std::endl;
      std::cout << "NumberOfIterations = " << staple->GetElapsedIterations() << std::endl;

      /** Save the result in the softSegmentationArray[1] */
      softSegmentationArray[0] = 0;
      softSegmentationArray[1] = staple->GetOutput();

      /** From this result also generate softSegmentationArray[0] */
      typename InverterType::Pointer inverter = InverterType::New();
      inverter->SetMaximum( itk::NumericTraits<ProbPixelType>::One );
      inverter->SetInput( softSegmentationArray[1] );
      std::cout << "Generating soft segmentation for class 0..." << std::endl;
      inverter->Update();
      std::cout << "Done generating soft segmentation for class 0..." << std::endl;
      softSegmentationArray[0] = inverter->GetOutput();

      /** Generate the hard segmentation from the softSegmentation[0] */
      if( this->m_HardOutputFileName != "" )
      {
        /** use the soft segmentation of class 0, since the threshold filter
        * applies a > operator, and we would like to apply >= on the soft
        * segmentation of class 1  */
        typename ThresholderType::Pointer thresholder = ThresholderType::New();
        thresholder->SetLowerThreshold( itk::NumericTraits<ProbPixelType>::NonpositiveMin() );
        thresholder->SetUpperThreshold( static_cast<ProbPixelType>( 0.5 )  );
        thresholder->SetInsideValue( itk::NumericTraits<LabelPixelType>::One );
        thresholder->SetOutsideValue( itk::NumericTraits<LabelPixelType>::Zero );
        thresholder->SetInput( softSegmentationArray[0] );
        std::cout << "Generating hard segmentation..." << std::endl;
        thresholder->Update();
        std::cout << "Done generating hard segmentation." << std::endl;
        hardSegmentation = thresholder->GetOutput();
      }

      /** Generate the confusion matrix */
      if( this->m_ConfusionOutputFileName != "" )
      {
        ConfusionIndexType cindex;
        ConfusionMatrixImageIteratorType cit( confusionMatrixImage,
          confusionMatrixImage->GetLargestPossibleRegion() );
        cit.GoToBegin();
        while ( !cit.IsAtEnd() )
        {
          cindex = cit.GetIndex();
          const ConfusionMatrixPixelType sens = static_cast<ConfusionMatrixPixelType>(
            staple->GetSensitivity( cindex[2] ) );
          const ConfusionMatrixPixelType spec = static_cast<ConfusionMatrixPixelType>(
            staple->GetSpecificity( cindex[2] ) );
          cit.Value() = spec; // class = 0, label = 0.
          ++cit;
          cit.Value() = 1.0 - sens; // class = 1, label = 0
          ++cit;
          cit.Value() = 1.0 - spec; // class = 0, label = 1
          ++cit;
          cit.Value() = sens; // class = 1, label = 1
          ++cit; // next observer
        } // end while
      } // end if confusion
    }  // end if STAPLE
    else if( this->m_CombinationMethod == "MULTISTAPLE" )
    {
      /** Run the MultiLabelSTAPLE algorithm */
      typename MultiLabelSTAPLEType::Pointer multistaple = MultiLabelSTAPLEType::New();
      segmentationCombiner = multistaple;
      for( unsigned int i = 0; i < numberOfObservers; ++i )
      {
        multistaple->SetInput(i, labelImageArray[ i ]);
      }
      if( this->m_PriorProbs.size() == this->m_NumberOfClasses )
      {
        MultiSTAPLEPriorProbsType priorProbsCast( this->m_PriorProbs.size() );
        for( unsigned int i = 0; i < this->m_PriorProbs.size(); ++i )
        {
          priorProbsCast[ i ] = static_cast<MultiSTAPLEWeightsType>( this->m_PriorProbs[ i ] );
        }
        multistaple->SetPriorProbabilities( priorProbsCast );
      }
      std::cout << "TerminationUpdateThreshold = " << this->m_TerminationThreshold << std::endl;
      multistaple->SetTerminationUpdateThreshold( this->m_TerminationThreshold );
      std::cout << "Performing MULTISTAPLE algorithm..." << std::endl;
      multistaple->Update();
      std::cout << "Done performing MULTISTAPLE algorithm." << std::endl;
      std::cout
        << "Estimated/supplied priorProbabilities were: "
        << multistaple->GetPriorProbabilities()
        << std::endl;
      hardSegmentation = multistaple->GetOutput();

      /** Generate the confusion matrix */
      if( this->m_ConfusionOutputFileName != "" )
      {
        ConfusionIndexType cindex;
        ConfusionMatrixImageIteratorType cit( confusionMatrixImage,
          confusionMatrixImage->GetLargestPossibleRegion() );
        cit.GoToBegin();
        while ( !cit.IsAtEnd() )
        {
          cindex = cit.GetIndex();
          MultiSTAPLEConfusionMatrixType confmat =
            multistaple->GetConfusionMatrix( cindex[2] );
          /** In multiSTAPLE the first index corresponds to applied class, and the
          * second index to the real class, so just different than in our definition. */
          for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
          {
            for( unsigned int j = 0; j < this->m_NumberOfClasses; ++j )
            {
              cit.Value() = confmat[ i ][j];
              ++cit;
            }
          }
        } // end while
      } // end if confusion
    } // end if MULTISTAPLE
    else if( ( this->m_CombinationMethod == "MULTISTAPLE2" ) || ( this->m_CombinationMethod == "VOTE_MULTISTAPLE2" ) )
    {
      /** Run the MultiLabelSTAPLE algorithm */
      typename MultiLabelSTAPLE2Type::Pointer multistaple2 = MultiLabelSTAPLE2Type::New();
      segmentationCombiner = multistaple2;

      typename MaskGeneratorType::Pointer maskGenerator = MaskGeneratorType::New();
      typename DilateFilterType::Pointer dilater = DilateFilterType::New();

      /** Set the number of classes */
      multistaple2->SetNumberOfClasses( this->m_NumberOfClasses );

      /** Set the inputs */
      for( unsigned int i = 0; i < numberOfObservers; ++i )
      {
        multistaple2->SetInput(i, labelImageArray[ i ]);
        maskGenerator->SetInput(i, labelImageArray[ i ]);
      }

      /** Set the mask */
      if( this->m_UseMask && ( numberOfObservers > 1 ) )
      {
        StructuringElementType kernel;
        KernelRadiusType radius;
        radius.Fill(this->m_MaskDilationRadius);
        kernel.SetRadius(radius);
        kernel.CreateStructuringElement();
        dilater->SetKernel(kernel);
        dilater->SetForegroundValue( itk::NumericTraits<MaskPixelType>::One );
        dilater->SetBackgroundValue( itk::NumericTraits<MaskPixelType>::Zero );
        dilater->SetInput( maskGenerator->GetOutput() );
        std::cout << "Creating mask (this->m_MaskDilationRadius = " << this->m_MaskDilationRadius << ")..." << std::endl;
        dilater->Update();
        multistaple2->SetMaskImage( dilater->GetOutput() );
        std::cout << "Done creating mask." << std::endl;
      }

      /** Set the prior preferences. They are given as a list of labels in this->m_PrefOrder.
      * The staple class expects a 'preference'-number for each class;
      * the lower the more preference. The code below does the conversion. */
      PriorPreferenceType priorPref(this->m_NumberOfClasses);
      for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
      {
        priorPref[ this->m_PrefOrder[ i ] ] = i;
      }
      multistaple2->SetPriorPreference( priorPref );

      /** Set the prior probabilities */
      if( this->m_PriorProbImageFileNames.size() == this->m_NumberOfClasses )
      {
        MultiSTAPLE2PriorProbImageArrayType priorProbImageArrayCast( this->m_NumberOfClasses );
        for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
        {
          priorProbImageArrayCast[ i ] = priorProbImageArray[ i ];
        }
        multistaple2->SetPriorProbabilityImageArray( priorProbImageArrayCast );
      }
      else if( this->m_PriorProbs.size() == this->m_NumberOfClasses )
      {
        MultiSTAPLEPriorProbsType priorProbsCast( this->m_NumberOfClasses );
        for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
        {
          priorProbsCast[ i ] = static_cast<MultiSTAPLEWeightsType>( this->m_PriorProbs[ i ] );
        }
        multistaple2->SetPriorProbabilities( priorProbsCast );
      }

      /** Set the this->m_Trust in the observers */
      if( this->m_Trust.size() == numberOfObservers )
      {
        MultiSTAPLE2ObserverTrustType observerTrustCast( this->m_Trust.size() );
        for( unsigned int i = 0; i < this->m_Trust.size(); ++i )
        {
          observerTrustCast[ i ] = static_cast<MultiSTAPLEWeightsType>( this->m_Trust[ i ] );
        }
        multistaple2->SetObserverTrust( observerTrustCast );
      }

      multistaple2->SetInitializeWithMajorityVoting( ( this->m_CombinationMethod == "VOTE_MULTISTAPLE2" ) );

      /** Set whether soft segmentations are required */
      if( this->m_SoftOutputFileNames.size() > 0 )
      {
        multistaple2->SetGenerateProbabilisticSegmentations(true);
      }

      /** Set the termination threshold */
      std::cout << "TerminationUpdateThreshold = " << this->m_TerminationThreshold << std::endl;
      multistaple2->SetTerminationUpdateThreshold( this->m_TerminationThreshold );

      /** Run!! */
      std::cout << "Performing " << this->m_CombinationMethod << " algorithm..." << std::endl;
      multistaple2->Update();
      std::cout << "Done performing " << this->m_CombinationMethod << " algorithm." << std::endl;
      if( this->m_PriorProbImageFileNames.size() != this->m_NumberOfClasses )
      {
        std::cout
          << "Estimated/supplied priorProbabilities were: "
          << multistaple2->GetPriorProbabilities()
          << std::endl;
      }
      std::cout
        << "Estimated/supplied initial observer this->m_Trust was: "
        << multistaple2->GetObserverTrust()
        << std::endl;
      std::cout << "NumberOfIterations = " << multistaple2->GetElapsedIterations() << std::endl;
      std::cout << "Last maximum confusion matrix element update = "
        << multistaple2->GetMaximumConfusionMatrixElementUpdate() << std::endl;

      /** Get the hard segmentation */
      hardSegmentation = multistaple2->GetOutput();

      /** Get the soft segmentations */
      if( this->m_SoftOutputFileNames.size() > 0 )
      {
        for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
        {
          softSegmentationArray[ i ] = multistaple2->GetProbabilisticSegmentationArray()[ i ];
        }
      }

      /** Generate the confusion matrix */
      if( this->m_ConfusionOutputFileName != "" )
      {
        ConfusionIndexType cindex;
        ConfusionMatrixImageIteratorType cit( confusionMatrixImage,
          confusionMatrixImage->GetLargestPossibleRegion() );
        cit.GoToBegin();
        while ( !cit.IsAtEnd() )
        {
          cindex = cit.GetIndex();
          MultiSTAPLEConfusionMatrixType confmat =
            multistaple2->GetConfusionMatrix( cindex[2] );
          /** In multiSTAPLE2 the first index corresponds to applied class, and the
          * second index to the real class, so just different than in our definition. */
          for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
          {
            for( unsigned int j = 0; j < this->m_NumberOfClasses; ++j )
            {
              cit.Value() = confmat[ i ][j];
              ++cit;
            }
          }
        } // end while
      } // end if confusion

    } // end if MULTISTAPLE2
    else if( this->m_CombinationMethod == "VOTE" )
    {
      /** Run the LabelVoting2 algorithm */
      typename LabelVotingType::Pointer voting = LabelVotingType::New();
      typename MaskGeneratorType::Pointer maskGenerator = MaskGeneratorType::New();
      typename DilateFilterType::Pointer dilater = DilateFilterType::New();
      segmentationCombiner = voting;

      /** Set the number of classes */
      voting->SetNumberOfClasses( this->m_NumberOfClasses );

      /** Set the inputs */
      for( unsigned int i = 0; i < numberOfObservers; ++i )
      {
        voting->SetInput(i, labelImageArray[ i ]);
        maskGenerator->SetInput(i, labelImageArray[ i ]);
      }

      /** Set the mask */
      if( this->m_UseMask  && ( numberOfObservers > 1) )
      {
        StructuringElementType kernel;
        KernelRadiusType radius;
        radius.Fill(this->m_MaskDilationRadius);
        kernel.SetRadius(radius);
        kernel.CreateStructuringElement();
        dilater->SetKernel(kernel);
        dilater->SetForegroundValue( itk::NumericTraits<MaskPixelType>::One );
        dilater->SetBackgroundValue( itk::NumericTraits<MaskPixelType>::Zero );
        dilater->SetInput( maskGenerator->GetOutput() );
        std::cout << "Creating mask (this->m_MaskDilationRadius = "
          << this->m_MaskDilationRadius << ")..." << std::endl;
        dilater->Update();
        voting->SetMaskImage( dilater->GetOutput() );
        std::cout << "Done creating mask." << std::endl;
      }

      voting->SetGenerateConfusionMatrix( this->m_ConfusionOutputFileName != "" );

      /** Set the prior preferences. They are given as a list of labels in this->m_PrefOrder.
      * The staple class expects a 'preference'-number for each class;
      * the lower the more preference. The code below does the conversion. */
      PriorPreferenceType priorPref(this->m_NumberOfClasses);
      for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
      {
        priorPref[ this->m_PrefOrder[ i ] ] = i;
      }
      voting->SetPriorPreference( priorPref );

      /** Set the this->m_Trust in the observers */
      if( this->m_Trust.size() == numberOfObservers )
      {
        MultiSTAPLE2ObserverTrustType observerTrustCast( this->m_Trust.size() );
        for( unsigned int i = 0; i < this->m_Trust.size(); ++i )
        {
          observerTrustCast[ i ] = static_cast<MultiSTAPLEWeightsType>( this->m_Trust[ i ] );
        }
        voting->SetObserverTrust( observerTrustCast );
      }

      /** Set whether soft segmentations are required */
      if( this->m_SoftOutputFileNames.size() > 0 )
      {
        voting->SetGenerateProbabilisticSegmentations(true);
      }

      /** Run!! */
      std::cout << "Performing VOTE algorithm..." << std::endl;
      voting->Update();
      std::cout << "Done performing VOTE algorithm." << std::endl;

      std::cout
        << "Estimated/supplied initial observer this->m_Trust was: "
        << voting->GetObserverTrust()
        << std::endl;

      /** Get the hard segmentation */
      hardSegmentation = voting->GetOutput();

      /** Get the soft segmentations */
      if( this->m_SoftOutputFileNames.size() > 0 )
      {
        for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
        {
          softSegmentationArray[ i ] = voting->GetProbabilisticSegmentationArray()[ i ];
        }
      }

      /** Generate the confusion matrix */
      if( this->m_ConfusionOutputFileName != "" )
      {
        ConfusionIndexType cindex;
        ConfusionMatrixImageIteratorType cit( confusionMatrixImage,
          confusionMatrixImage->GetLargestPossibleRegion() );
        cit.GoToBegin();
        while ( !cit.IsAtEnd() )
        {
          cindex = cit.GetIndex();
          MultiSTAPLEConfusionMatrixType confmat =
            voting->GetConfusionMatrix( cindex[2] );
          /** In multiSTAPLE2 the first index corresponds to applied class, and the
          * second index to the real class, so just different than in our definition. */
          for( unsigned int i = 0; i < this->m_NumberOfClasses; ++i )
          {
            for( unsigned int j = 0; j < this->m_NumberOfClasses; ++j )
            {
              cit.Value() = confmat[ i ][j];
              ++cit;
            }
          }
        } // end while
      } // end if confusion

    } // end if VOTE
    else
    {
      std::cout << "ERROR: The desired combination method "
        << this->m_CombinationMethod
        << " is not yet supported!" << std::endl;
      itkGenericExceptionMacro( << "Specify a different this->m_CombinationMethod via the \"-m\" option" );
    }

    /** Write soft segmentations */
    if( this->m_SoftOutputFileNames.size() > 0 )
    {
      std::cout << "Writing soft segmentations..." << std::endl;
      for( unsigned int i = 0; i < this->m_SoftOutputFileNames.size(); ++i )
      {
        typename ProbImageWriterType::Pointer softWriter =
          ProbImageWriterType::New();
        softWriter->SetFileName( this->m_SoftOutputFileNames[ i ].c_str() );
        /** Check if the soft segmentation is available. MULTISTAPLE does not
        * generate soft segmentations */
        if( softSegmentationArray[ i ].IsNotNull() )
        {
          softWriter->SetInput( softSegmentationArray[ i ] );
          softWriter->SetUseCompression( this->m_UseCompression );
          softWriter->Update();
        }
      }
      std::cout << "Done writing soft segmentations." << std::endl;
    }

    /** Write hard segmentations */
    if( this->m_HardOutputFileName != "" )
    {
      typename LabelImageWriterType::Pointer hardWriter =
        LabelImageWriterType::New();
      hardWriter->SetFileName( this->m_HardOutputFileName.c_str() );
      if( hardSegmentation.IsNotNull() )
      {
        hardWriter->SetInput( hardSegmentation );
        hardWriter->SetUseCompression( this->m_UseCompression );
        std::cout << "Writing hard segmentation..." << std::endl;
        hardWriter->Update();
        std::cout << "Done writing hard segmentation." << std::endl;
      }
    }

    /** Write confusion image */
    if( this->m_ConfusionOutputFileName != "" )
    {
      typename ConfusionMatrixImageWriterType::Pointer confusionWriter =
        ConfusionMatrixImageWriterType::New();
      confusionWriter->SetFileName( this->m_ConfusionOutputFileName.c_str() );
      if( confusionMatrixImage.IsNotNull() )
      {
        confusionWriter->SetInput( confusionMatrixImage );
        confusionWriter->SetUseCompression( this->m_UseCompression );
        std::cout << "Writing confusion matrix image..." << std::endl;
        confusionWriter->Update();
        std::cout << "Done writing confusion matrix image..." << std::endl;
      }
    }
  } // end Run()

}; // end class ITKToolsCombineSegmentations


#endif // end #ifndef __combinesegmentations_h_
