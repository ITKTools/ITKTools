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
/** \file
 \brief This program combines multiple segmentations into one.
 
 \verbinclude combinesegmentations.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>
#include <vector>

#include "itkImage.h"
#include "itkExceptionObject.h"
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


/** Declare CombineSegmentations. */
template <unsigned int NDimensions>
void CombineSegmentations(
  const std::vector<std::string> & inputSegmentationFileNames,
  const std::vector<std::string> & priorProbImageFileNames,
  const std::vector<std::string> & softOutputFileNames,
  const std::string & hardOutputFileName,
  const std::string & confusionOutputFileName,
  unsigned char numberOfClasses,
  const std::vector<float> & priorProbs,
  const std::vector<float> & trust,
  float terminationThreshold,
  const std::string & combinationMethod,
  bool useMask,
  unsigned int maskDilationRadius,
  const std::vector<unsigned int> & prefOrder,
  const std::vector<unsigned int> & inValues,
  const std::vector<unsigned int> & outValues );

/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
    /** Get the combination method (mandatory) */
  std::string combinationMethod = "MULTISTAPLE2";
  parser->GetCommandLineArgument( "-m", combinationMethod );

  /** Get the input segmentation file names (mandatory). */
  std::vector< std::string >  inputSegmentationFileNames;
  parser->GetCommandLineArgument( "-in", inputSegmentationFileNames );

  /** Get the settings for the change label image filter (not mandatory) */
  std::vector<unsigned int>  inValues;
  std::vector<unsigned int>  outValues;
  parser->GetCommandLineArgument( "-iv", inValues );
  parser->GetCommandLineArgument( "-ov", outValues );
  if ( inValues.size() != outValues.size() )
  {
    std::cerr << "ERROR: Number of values following after \"-iv\" and \"-ov\" should be equal." << std::endl;
    return 1;
  }

  /** Get the number of classes to segment (not mandatory) */
  unsigned char numberOfClasses = 2;
  parser->GetCommandLineArgument( "-n", numberOfClasses );

  /** Get the prior probability images (not mandatory) */
  std::vector< std::string >  priorProbImageFileNames;
  bool retP = parser->GetCommandLineArgument( "-P", priorProbImageFileNames );
  if (retP)
  {
    if ( priorProbImageFileNames.size() != numberOfClasses )
    {
      std::cerr
        << "ERROR: Number of prior probability images should be equal "
        << "to the number of classes."
        << std::endl;
      std::cerr
        << "i.e., \"-P\" should be followed by "
        << numberOfClasses
        << " filenames or just totally omitted."
        << std::endl;
      return 1;
    }
  }

  /** Get the prior probabilities (not mandatory) */
  std::vector< float >  priorProbs(0);
  bool retp = parser->GetCommandLineArgument( "-p", priorProbs );
  if (retp && !retP)
  {
    if ( priorProbs.size() != numberOfClasses )
    {
      std::cerr
        << "ERROR: Number of prior probabilities should be equal "
        << "to the number of classes."
        << std::endl;
      std::cerr
        << "i.e., \"-p\" should be followed by "
        << numberOfClasses
        << " numbers or just totally omitted."
        << std::endl;
      return 1;
    }
  }
  if (retp && retP)
  {
    std::cout << "WARNING: \"-p\" is ignored when \"-P\" is given as well!" << std::endl;
  }

  /** Get the trust factor for each observer (not mandatory) */
  std::vector< float > trust(0);
  bool rett = parser->GetCommandLineArgument( "-t", trust );
  if (rett)
  {
    if ( trust.size() != inputSegmentationFileNames.size() )
    {
      std::cerr
        << "ERROR: Number of trust factors should be equal to the number of "
        << "input segmentations."
        << std::endl;
      std::cerr
        << "i.e., \"-t\" should be followed by "
        << inputSegmentationFileNames.size()
        << " numbers or just totally omitted." << std::endl;
      return 1;
    }
  }

  /** Get the number of classes to segment (not mandatory) */
  float terminationThreshold = 1e-5;
  parser->GetCommandLineArgument( "-e", terminationThreshold );

  /** Get the outputFileNames */
  std::vector< std::string > softOutputFileNames;
  std::string hardOutputFileName = "";
  std::string confusionOutputFileName = "";
  bool retouts = parser->GetCommandLineArgument( "-outs", softOutputFileNames );
  if ( retouts )
  {
    if ( softOutputFileNames.size() != numberOfClasses  &&
         softOutputFileNames.size() != 1 )
    {
      std::cerr
        << "ERROR: Number of soft output image file names should be equal "
        << "to the number of classes."
        << std::endl;
      std::cerr
        << "i.e., \"-outs\" should be followed by "
        << numberOfClasses
        << " filenames or just totally omitted."
        << std::endl;
      return 1;
    }
  }
  parser->GetCommandLineArgument( "-outh", hardOutputFileName );
  parser->GetCommandLineArgument( "-outc", confusionOutputFileName );

  /** Use mask or not? If yes, read the maskDilationRadius. */
  unsigned int maskDilationRadius = 1;
  bool useMask = parser->ArgumentExists( "-mask" );
  parser->GetCommandLineArgument( "-mask", maskDilationRadius );

  /** Read the preferred order of classes in case of undecided pixels */
  std::vector<unsigned int> prefOrder(numberOfClasses);
  for (unsigned int i = 0; i < numberOfClasses; ++i)
  {
    prefOrder[i] = i;
  }
  parser->GetCommandLineArgument( "-ord", prefOrder );

  /** Threads. */
  unsigned int maximumNumberOfThreads
    = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  parser->GetCommandLineArgument(
    "-threads", maximumNumberOfThreads );
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(
    maximumNumberOfThreads );

  /** Determine image properties. */
  std::string ComponentType = "unsigned char";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputSegmentationFileNames[0],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: vector images are not supported." << std::endl;
    return 1;
  }
  if ( (Dimension!=2) && (Dimension!=3) )
  {
    std::cerr
      << "ERROR: images of dimension "
      << Dimension
      << " are not supported!"
      << std::endl;
    return 1;
  }
  /** This is the easiest way to make sure that the input labels are in the
   * valid range 0 - 127. Leads to crashes in case of short input with high
   * values otherwise.
   */
  if ( ComponentType != "unsigned char" )
  {
    std::cerr << "ERROR: only \"unsigned char\" images are supported." << std::endl;
    return 1;

  }

  /** Run the program. */
  try
  {
    if (Dimension==2)
    {
      CombineSegmentations<2>(
        inputSegmentationFileNames,
        priorProbImageFileNames,
        softOutputFileNames,
        hardOutputFileName,
        confusionOutputFileName,
        numberOfClasses,
        priorProbs,
        trust,
        terminationThreshold,
        combinationMethod,
        useMask,
        maskDilationRadius,
        prefOrder,
        inValues,
        outValues);
    }
    if (Dimension==3)
    {
      CombineSegmentations<3>(
        inputSegmentationFileNames,
        priorProbImageFileNames,
        softOutputFileNames,
        hardOutputFileName,
        confusionOutputFileName,
        numberOfClasses,
        priorProbs,
        trust,
        terminationThreshold,
        combinationMethod,
        useMask,
        maskDilationRadius,
        prefOrder,
        inValues,
        outValues);
    }

  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  std::cout << "pxcombinesegmentations has finished!" << std::endl;

  /** End program. */
  return 0;

} // end main



/**
 * ******************* CombineSegmentations ****************
 *
 * The function that does the work, templated over the image dimension.
 */

template <unsigned int NDimensions>
void CombineSegmentations(
  const std::vector<std::string> & inputSegmentationFileNames,
  const std::vector<std::string> & priorProbImageFileNames,
  const std::vector<std::string> & softOutputFileNames,
  const std::string & hardOutputFileName,
  const std::string & confusionOutputFileName,
  unsigned char numberOfClasses,
  const std::vector<float> & priorProbs,
  const std::vector<float> & trust,
  float terminationThreshold,
  const std::string & combinationMethod,
  bool useMask,
  unsigned int maskDilationRadius,
  const std::vector<unsigned int> & prefOrder,
  const std::vector<unsigned int> & inValues,
  const std::vector<unsigned int> & outValues)
{
  const unsigned int Dimension = NDimensions;
  typedef unsigned char   LabelPixelType;
  typedef float           ProbPixelType;
  typedef float           ConfusionMatrixPixelType;

  typedef itk::Image< LabelPixelType, Dimension >   LabelImageType;
  typedef itk::Image< ProbPixelType, Dimension >    ProbImageType;
  typedef itk::Image<
    ConfusionMatrixPixelType, 3 >                   ConfusionMatrixImageType;

  typedef typename LabelImageType::RegionType       RegionType;

  typedef typename LabelImageType::Pointer          LabelImagePointer;
  typedef typename ProbImageType::Pointer           ProbImagePointer;
  typedef typename
    ConfusionMatrixImageType::Pointer               ConfusionMatrixImagePointer;

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
    MaskPixelType, Dimension >                     StructuringElementType;
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
  SegmentationCombinerType::Pointer segmentationCombiner = 0;
  LabelImageArrayType labelImageArray;
  ProbImageArrayType priorProbImageArray;
  ProbImageArrayType softSegmentationArray;
  LabelImagePointer hardSegmentation = 0;
  ConfusionMatrixImagePointer confusionMatrixImage = 0;

  /** Initialize some variables */
  numberOfObservers = inputSegmentationFileNames.size();
  labelImageArray.resize( numberOfObservers );
  softSegmentationArray.resize( numberOfClasses );
  priorProbImageArray.resize( numberOfClasses );

  /** Read the input label images */
  RegionType lastRegion;
  bool relabel = ( inValues.size() > 0 );
  std::cout << "Reading (and possibly relabeling) input segmentations..." << std::endl;
  for ( unsigned int i = 0; i < numberOfObservers; ++i )
  {
    typename LabelImageReaderType::Pointer labelImageReader =
      LabelImageReaderType::New();
    labelImageReader->SetFileName( inputSegmentationFileNames[i].c_str() );
    labelImageReader->Update();

    /** Check size. */
    RegionType region = labelImageReader->GetOutput()->GetLargestPossibleRegion();
    if ( i > 0 && region != lastRegion )
    {
      std::cerr << "\nERROR: input label images are not of the same size!" << std::endl;
      return;
    }
    lastRegion = region;

    /** Relabel? */
    if ( relabel )
    {
      typename RelabelFilterType::Pointer relabeler = RelabelFilterType::New();
      relabeler->SetInput( labelImageReader->GetOutput() );
      for ( unsigned int lab = 0; lab < inValues.size(); ++lab )
      {
        LabelPixelType labin = static_cast<LabelPixelType>( inValues[lab] );
        LabelPixelType labout = static_cast<LabelPixelType>( outValues[lab] );
        relabeler->SetChange( labin, labout );
      }
      relabeler->Update();
      labelImageArray[i] = relabeler->GetOutput();
    } // end relabel
    else
    {
      labelImageArray[i] = labelImageReader->GetOutput();
    }
  }
  std::cout << "Done reading input segmentations." << std::endl;

  /** Read the prior probability images, if supplied */
  if ( priorProbImageFileNames.size() == numberOfClasses )
  {
    std::cout << "Reading prior probability images..." << std::endl;
    for (unsigned int i = 0; i < numberOfClasses; ++i )
    {
      typename ProbImageReaderType::Pointer probImageReader =
        ProbImageReaderType::New();
      probImageReader->SetFileName( priorProbImageFileNames[i].c_str() );
      probImageReader->Update();
      priorProbImageArray[i] = probImageReader->GetOutput();
    }
    std::cout << "Done reading prior probability images." << std::endl;
  }

  /** Prepare the confusion matrix */
  if ( confusionOutputFileName != "" )
  {
    ConfusionSizeType csize;
    csize[0] = numberOfClasses;
    csize[1] = numberOfClasses;
    csize[2] = numberOfObservers;
    confusionMatrixImage = ConfusionMatrixImageType::New();
    confusionMatrixImage->SetRegions( csize );
    confusionMatrixImage->Allocate();
  }

  /** Combine the label images */
  if ( combinationMethod == "STAPLE" )
  {
    /** run STAPLE */
    typename STAPLEType::Pointer staple = STAPLEType::New();
    segmentationCombiner = staple;
    staple->SetForegroundValue( 1 );
    for (unsigned int i = 0; i < numberOfObservers; ++i )
    {
      staple->SetInput(i, labelImageArray[i]);
    }
    if ( priorProbs.size() == 2 )
    {
      staple->SetConfidenceWeight( priorProbs[1] );
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
    if ( hardOutputFileName != "" )
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
    if ( confusionOutputFileName != "" )
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
  else if ( combinationMethod == "MULTISTAPLE" )
  {
    /** Run the MultiLabelSTAPLE algorithm */
    typename MultiLabelSTAPLEType::Pointer multistaple = MultiLabelSTAPLEType::New();
    segmentationCombiner = multistaple;
    for (unsigned int i = 0; i < numberOfObservers; ++i )
    {
      multistaple->SetInput(i, labelImageArray[i]);
    }
    if ( priorProbs.size() == numberOfClasses )
    {
      MultiSTAPLEPriorProbsType priorProbsCast( priorProbs.size() );
      for (unsigned int i = 0; i < priorProbs.size(); ++i)
      {
        priorProbsCast[i] = static_cast<MultiSTAPLEWeightsType>( priorProbs[i] );
      }
      multistaple->SetPriorProbabilities( priorProbsCast );
    }
    std::cout << "TerminationUpdateThreshold = " << terminationThreshold << std::endl;
    multistaple->SetTerminationUpdateThreshold( terminationThreshold );
    std::cout << "Performing MULTISTAPLE algorithm..." << std::endl;
    multistaple->Update();
    std::cout << "Done performing MULTISTAPLE algorithm." << std::endl;
    std::cout
      << "Estimated/supplied priorProbabilities were: "
      << multistaple->GetPriorProbabilities()
      << std::endl;
    hardSegmentation = multistaple->GetOutput();

     /** Generate the confusion matrix */
    if ( confusionOutputFileName != "" )
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
        for (unsigned int i = 0; i < numberOfClasses; ++i)
        {
          for ( unsigned int j = 0; j < numberOfClasses; ++j)
          {
            cit.Value() = confmat[i][j];
            ++cit;
          }
        }
      } // end while
    } // end if confusion
  } // end if MULTISTAPLE
  else if ( ( combinationMethod == "MULTISTAPLE2" ) || ( combinationMethod == "VOTE_MULTISTAPLE2" ) )
  {
    /** Run the MultiLabelSTAPLE algorithm */
    typename MultiLabelSTAPLE2Type::Pointer multistaple2 = MultiLabelSTAPLE2Type::New();
    segmentationCombiner = multistaple2;

    typename MaskGeneratorType::Pointer maskGenerator = MaskGeneratorType::New();
    typename DilateFilterType::Pointer dilater = DilateFilterType::New();

    /** Set the number of classes */
    multistaple2->SetNumberOfClasses( numberOfClasses );

    /** Set the inputs */
    for (unsigned int i = 0; i < numberOfObservers; ++i )
    {
      multistaple2->SetInput(i, labelImageArray[i]);
      maskGenerator->SetInput(i, labelImageArray[i]);
    }

    /** Set the mask */
    if ( useMask && ( numberOfObservers > 1 ) )
    {
      StructuringElementType kernel;
      KernelRadiusType radius;
      radius.Fill(maskDilationRadius);
      kernel.SetRadius(radius);
      kernel.CreateStructuringElement();
      dilater->SetKernel(kernel);
      dilater->SetForegroundValue( itk::NumericTraits<MaskPixelType>::One );
      dilater->SetBackgroundValue( itk::NumericTraits<MaskPixelType>::Zero );
      dilater->SetInput( maskGenerator->GetOutput() );
      std::cout << "Creating mask (maskDilationRadius = " << maskDilationRadius << ")..." << std::endl;
      dilater->Update();
      multistaple2->SetMaskImage( dilater->GetOutput() );
      std::cout << "Done creating mask." << std::endl;
    }

    /** Set the prior preferences. They are given as a list of labels in prefOrder.
     * The staple class expects a 'preference'-number for each class;
     * the lower the more preference. The code below does the conversion. */
    PriorPreferenceType priorPref(numberOfClasses);
    for (unsigned int i = 0; i < numberOfClasses; ++i)
    {
      priorPref[ prefOrder[i] ] = i;
    }
    multistaple2->SetPriorPreference( priorPref );

    /** Set the prior probabilities */
    if ( priorProbImageFileNames.size() == numberOfClasses )
    {
      MultiSTAPLE2PriorProbImageArrayType priorProbImageArrayCast( numberOfClasses );
      for (unsigned int i = 0; i < numberOfClasses; ++i)
      {
        priorProbImageArrayCast[i] = priorProbImageArray[i];
      }
      multistaple2->SetPriorProbabilityImageArray( priorProbImageArrayCast );
    }
    else if ( priorProbs.size() == numberOfClasses )
    {
      MultiSTAPLEPriorProbsType priorProbsCast( numberOfClasses );
      for (unsigned int i = 0; i < numberOfClasses; ++i)
      {
        priorProbsCast[i] = static_cast<MultiSTAPLEWeightsType>( priorProbs[i] );
      }
      multistaple2->SetPriorProbabilities( priorProbsCast );
    }

    /** Set the trust in the observers */
    if ( trust.size() == numberOfObservers )
    {
      MultiSTAPLE2ObserverTrustType observerTrustCast( trust.size() );
      for (unsigned int i = 0; i < trust.size(); ++i)
      {
        observerTrustCast[i] = static_cast<MultiSTAPLEWeightsType>( trust[i] );
      }
      multistaple2->SetObserverTrust( observerTrustCast );
    }

    multistaple2->SetInitializeWithMajorityVoting( ( combinationMethod == "VOTE_MULTISTAPLE2" ) );

    /** Set whether soft segmentations are required */
    if ( softOutputFileNames.size() > 0 )
    {
      multistaple2->SetGenerateProbabilisticSegmentations(true);
    }

    /** Set the termination threshold */
    std::cout << "TerminationUpdateThreshold = " << terminationThreshold << std::endl;
    multistaple2->SetTerminationUpdateThreshold( terminationThreshold );

    /** Run!! */
    std::cout << "Performing " << combinationMethod << " algorithm..." << std::endl;
    multistaple2->Update();
    std::cout << "Done performing " << combinationMethod << " algorithm." << std::endl;
    if ( priorProbImageFileNames.size() != numberOfClasses )
    {
      std::cout
        << "Estimated/supplied priorProbabilities were: "
        << multistaple2->GetPriorProbabilities()
        << std::endl;
    }
    std::cout
      << "Estimated/supplied initial observer trust was: "
      << multistaple2->GetObserverTrust()
      << std::endl;
    std::cout << "NumberOfIterations = " << multistaple2->GetElapsedIterations() << std::endl;
    std::cout << "Last maximum confusion matrix element update = " << multistaple2->GetMaximumConfusionMatrixElementUpdate() << std::endl;

    /** Get the hard segmentation */
    hardSegmentation = multistaple2->GetOutput();

    /** Get the soft segmentations */
    if ( softOutputFileNames.size() > 0 )
    {
      for (unsigned int i = 0; i < numberOfClasses; ++i)
      {
        softSegmentationArray[i] = multistaple2->GetProbabilisticSegmentationArray()[i];
      }
    }

    /** Generate the confusion matrix */
    if ( confusionOutputFileName != "" )
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
        for (unsigned int i = 0; i < numberOfClasses; ++i)
        {
          for ( unsigned int j = 0; j < numberOfClasses; ++j)
          {
            cit.Value() = confmat[i][j];
            ++cit;
          }
        }
      } // end while
    } // end if confusion

  } // end if MULTISTAPLE2
  else if ( combinationMethod == "VOTE" )
  {
    /** Run the LabelVoting2 algorithm */
    typename LabelVotingType::Pointer voting = LabelVotingType::New();
    typename MaskGeneratorType::Pointer maskGenerator = MaskGeneratorType::New();
    typename DilateFilterType::Pointer dilater = DilateFilterType::New();
    segmentationCombiner = voting;

    /** Set the number of classes */
    voting->SetNumberOfClasses( numberOfClasses );

    /** Set the inputs */
    for (unsigned int i = 0; i < numberOfObservers; ++i )
    {
      voting->SetInput(i, labelImageArray[i]);
      maskGenerator->SetInput(i, labelImageArray[i]);
    }

    /** Set the mask */
    if ( useMask  && ( numberOfObservers > 1) )
    {
      StructuringElementType kernel;
      KernelRadiusType radius;
      radius.Fill(maskDilationRadius);
      kernel.SetRadius(radius);
      kernel.CreateStructuringElement();
      dilater->SetKernel(kernel);
      dilater->SetForegroundValue( itk::NumericTraits<MaskPixelType>::One );
      dilater->SetBackgroundValue( itk::NumericTraits<MaskPixelType>::Zero );
      dilater->SetInput( maskGenerator->GetOutput() );
      std::cout << "Creating mask (maskDilationRadius = " << maskDilationRadius << ")..." << std::endl;
      dilater->Update();
      voting->SetMaskImage( dilater->GetOutput() );
      std::cout << "Done creating mask." << std::endl;
    }

    voting->SetGenerateConfusionMatrix( (confusionOutputFileName != "") );

    /** Set the prior preferences. They are given as a list of labels in prefOrder.
     * The staple class expects a 'preference'-number for each class;
     * the lower the more preference. The code below does the conversion. */
    PriorPreferenceType priorPref(numberOfClasses);
    for (unsigned int i = 0; i < numberOfClasses; ++i)
    {
      priorPref[ prefOrder[i] ] = i;
    }
    voting->SetPriorPreference( priorPref );

    /** Set the trust in the observers */
    if ( trust.size() == numberOfObservers )
    {
      MultiSTAPLE2ObserverTrustType observerTrustCast( trust.size() );
      for (unsigned int i = 0; i < trust.size(); ++i)
      {
        observerTrustCast[i] = static_cast<MultiSTAPLEWeightsType>( trust[i] );
      }
      voting->SetObserverTrust( observerTrustCast );
    }

    /** Set whether soft segmentations are required */
    if ( softOutputFileNames.size() > 0 )
    {
      voting->SetGenerateProbabilisticSegmentations(true);
    }

    /** Run!! */
    std::cout << "Performing VOTE algorithm..." << std::endl;
    voting->Update();
    std::cout << "Done performing VOTE algorithm." << std::endl;

    std::cout
      << "Estimated/supplied initial observer trust was: "
      << voting->GetObserverTrust()
      << std::endl;

    /** Get the hard segmentation */
    hardSegmentation = voting->GetOutput();

    /** Get the soft segmentations */
    if ( softOutputFileNames.size() > 0 )
    {
      for (unsigned int i = 0; i < numberOfClasses; ++i)
      {
        softSegmentationArray[i] = voting->GetProbabilisticSegmentationArray()[i];
      }
    }

    /** Generate the confusion matrix */
    if ( confusionOutputFileName != "" )
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
        for (unsigned int i = 0; i < numberOfClasses; ++i)
        {
          for ( unsigned int j = 0; j < numberOfClasses; ++j)
          {
            cit.Value() = confmat[i][j];
            ++cit;
          }
        }
      } // end while
    } // end if confusion

  } // end if VOTE
  else
  {
    std::cout
      << "ERROR: The desired combination method "
      << combinationMethod
      << " is not yet supported!"
      << std::endl;
    itkGenericExceptionMacro( << "Specify a different combinationMethod via the \"-m\" option" )

  }


  /** Write soft segmentations */
  if ( softOutputFileNames.size() > 0 )
  {
    std::cout << "Writing soft segmentations..." << std::endl;
    for ( unsigned int i = 0; i < softOutputFileNames.size(); ++i )
    {
      typename ProbImageWriterType::Pointer softWriter =
        ProbImageWriterType::New();
      softWriter->SetFileName( softOutputFileNames[i].c_str() );
      /** Check if the soft segmentation is available. MULTISTAPLE does not
       * generate soft segmentations */
      if ( softSegmentationArray[i].IsNotNull() )
      {
        softWriter->SetInput( softSegmentationArray[ i ] );
        softWriter->Update();
      }
    }
    std::cout << "Done writing soft segmentations." << std::endl;
  }

  /** Write hard segmentations */
  if ( hardOutputFileName != "" )
  {
    typename LabelImageWriterType::Pointer hardWriter =
      LabelImageWriterType::New();
    hardWriter->SetFileName( hardOutputFileName.c_str() );
    if ( hardSegmentation.IsNotNull() )
    {
      hardWriter->SetInput( hardSegmentation );
      std::cout << "Writing hard segmentation..." << std::endl;
      hardWriter->Update();
      std::cout << "Done writing hard segmentation." << std::endl;
    }
  }

  /** Write confusion image */
  if ( confusionOutputFileName != "" )
  {
    typename ConfusionMatrixImageWriterType::Pointer confusionWriter =
      ConfusionMatrixImageWriterType::New();
    confusionWriter->SetFileName( confusionOutputFileName.c_str() );
    if ( confusionMatrixImage.IsNotNull() )
    {
      confusionWriter->SetInput( confusionMatrixImage );
      std::cout << "Writing confusion matrix image..." << std::endl;
      confusionWriter->Update();
      std::cout << "Done writing confusion matrix image..." << std::endl;
    }
  }

} // end CombineSegmentations


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString()
{
  std::string helpText = "This program combines multiple segmentations into one.\n \
  Usage:\n \
  pxcombinesegmentations \
    [-m]     {STAPLE, VOTE, MULTISTAPLE, MULTISTAPLE2, VOTE_MULTISTAPLE2}:\n \
             the method used to combine the segmentations. default: MULTISTAPLE2.\n \
             VOTE_MULTISTAPLE2 is in fact just VOTE followed by MULTISTAPLE2. \
    -in      inputFilename0 [inputFileName1 ... ]: the input segmentations,\n \
             as unsigned char images. More than 2 labels are allowed, but\n \
             with some restrictions: {0,1,2}=ok, {0,3,4}=bad, {1,2,3}=bad. \
    [-n]     numberOfClasses: the number of classes to segment;\n \
             default: 2 (so, 0 and 1). \
    [-P]     priorProbImageFilename0 priorProbImageFilename1 [...]:\n \
             the names of the prior probabilities for each class, stored as float images.\n \
             This has only effect when using [VOTE_]MULTISTAPLE2. \
    [-p]     priorProb0 priorProb1 [...]:\n \
             the prior probabilities for each class, independent of x, so a floating point\n \
             number for each class. This parameter is ignored when \"-P\" is provided as well.\n \
             For VOTE this parameter is ignored. For STAPLE, this number is considered\n \
             as a factor which is multiplied with the estimated prior probability.\n \
             For MULTISTAPLE[2], the number is really the prior probability.\n \
             If -p and -P are not provided, the prior probs are estimated from the data. \
    [-t]     trust0 [trust1 ...]: a factor between 0 and 1 indicating the 'trust' in each observer;\n \
             default: 0.99999 for each observer for [VOTE_]MULTISTAPLE2. 1.0 for VOTE.\n \
             Ignored by STAPLE and MULTISTAPLE; they estimate it by majority voting. \
    [-e]     termination threshold: a small float. the smaller the more accurate the solution;\n \
             default: 1e-5. Ignored by STAPLE and VOTE. \
    [-outs]  outputFilename0 outputFileName1 [...]: the output (soft) probabilistic\n \
             segmentations for each label. These will be float images. \
    [-outh]  outputFilename: the output hard segmentation, stored as a single\n \
             unsigned char image, containing the label numbers.\n \
             The value 'numberOfClasses' corresponds to 'undecided' (if two labels\n \
             are exactly equally likely). \
    [-outc]  confusionImageFileName: 3d float image, in which each slice resembles\n \
             the confusion matrix for each observer. The x-axis corresponds to the\n \
             real label, the y-axis corresponds to the label given by the observer. \
    [-mask]  [maskDilationRadius]: Use a mask if this flag is provided.\n \
             Only taken into account by [VOTE_]MULTISTAPLE2 and VOTE.\n \
             The mask is 0 at those pixels were the decision is unanimous, and 1 elsewhere.\n \
             A dilation is performed with a kernel with radius maskDilationRadius (default:1)\n \
             Pixels that are outside the mask, will have class of the first observer.\n \
             Other pixels are passed through the combination algorithm.\n \
             The confusion matrix will be only based on the pixels within the mask. \
    [-ord]   The order of preferred classes, in cases of undecided pixels. Default: 0 1 2...\n \
             Ignored by STAPLE and MULTISTAPLE. In the default case, class 0 will be\n \
             preferred over class 1, for example. \
    [-iv]    inputlabels for relabeling \
    [-ov]    outputlabels for relabeling. Each input label is replaced by the corresponding\n \
             output label, before the combinationMethod is invoked. NumberOfClasses should be\n \
             valid for the situation after relabeling! \
  Supported: 2D/3D.";

  return helpText;
} // end GetHelpString
