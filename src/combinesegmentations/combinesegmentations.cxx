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

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "combinesegmentations.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program combines multiple segmentations into one.\n"
    << "Usage:\n"
    << "pxcombinesegmentations\n"
    << "[-m]     {STAPLE, VOTE, MULTISTAPLE, MULTISTAPLE2, VOTE_MULTISTAPLE2}:\n"
    << "        the method used to combine the segmentations. default: MULTISTAPLE2.\n"
    << "        VOTE_MULTISTAPLE2 is in fact just VOTE followed by MULTISTAPLE2.\n"
    << "-in      inputFilename0 [inputFileName1 ... ]: the input segmentations,\n"
    << "        as unsigned char images. More than 2 labels are allowed, but\n"
    << "        with some restrictions: {0,1,2}=ok, {0,3,4}=bad, {1,2,3}=bad.\n"
    << "[-n]     numberOfClasses: the number of classes to segment;\n"
    << "        default: 2 (so, 0 and 1).\n"
    << "[-P]     priorProbImageFilename0 priorProbImageFilename1 [...]:\n"
    << "        the names of the prior probabilities for each class, stored as float images.\n"
    << "        This has only effect when using [VOTE_]MULTISTAPLE2.\n"
    << "[-p]     priorProb0 priorProb1 [...]:\n"
    << "        the prior probabilities for each class, independent of x, so a floating point\n"
    << "        number for each class. This parameter is ignored when \"-P\" is provided as well.\n"
    << "        For VOTE this parameter is ignored. For STAPLE, this number is considered\n"
    << "        as a factor which is multiplied with the estimated prior probability.\n"
    << "       For MULTISTAPLE[2], the number is really the prior probability.\n"
    << "        If -p and -P are not provided, the prior probs are estimated from the data.\n"
    << "[-t]     trust0 [trust1 ...]: a factor between 0 and 1 indicating the 'trust' in each observer;\n"
    << "        default: 0.99999 for each observer for [VOTE_]MULTISTAPLE2. 1.0 for VOTE.\n"
    << "        Ignored by STAPLE and MULTISTAPLE; they estimate it by majority voting.\n"
    << "[-e]     termination threshold: a small float. the smaller the more accurate the solution;\n"
    << "        default: 1e-5. Ignored by STAPLE and VOTE.\n"
    << "[-outs]  outputFilename0 outputFileName1 [...]: the output (soft) probabilistic\n"
    << "        segmentations for each label. These will be float images.\n"
    << "[-outh]  outputFilename: the output hard segmentation, stored as a single\n"
    << "        unsigned char image, containing the label numbers.\n"
    << "       The value 'numberOfClasses' corresponds to 'undecided' (if two labels\n"
    << "        are exactly equally likely).\n"
    << "[-outc]  confusionImageFileName: 3d float image, in which each slice resembles\n"
    << "        the confusion matrix for each observer. The x-axis corresponds to the\n"
    << "        real label, the y-axis corresponds to the label given by the observer.\n"
    << "[-mask]  [maskDilationRadius]: Use a mask if this flag is provided.\n"
    << "        Only taken into account by [VOTE_]MULTISTAPLE2 and VOTE.\n"
    << "        The mask is 0 at those pixels were the decision is unanimous, and 1 elsewhere.\n"
    << "        A dilation is performed with a kernel with radius maskDilationRadius (default:1)\n"
    << "        Pixels that are outside the mask, will have class of the first observer.\n"
    << "        Other pixels are passed through the combination algorithm.\n"
    << "        The confusion matrix will be only based on the pixels within the mask.\n"
    << "[-ord]   The order of preferred classes, in cases of undecided pixels. Default: 0 1 2...\n"
    << "        Ignored by STAPLE and MULTISTAPLE. In the default case, class 0 will be\n"
    << "        preferred over class 1, for example.\n"
    << "[-iv]    inputlabels for relabeling\n"
    << "[-ov]    outputlabels for relabeling. Each input label is replaced by the corresponding\n"
    << "        output label, before the combinationMethod is invoked. NumberOfClasses should be\n"
    << "        valid for the situation after relabeling!\n"
    << "[-z]    compression flag; if provided, the output image is compressed\n"
    << "[-threads] maximum number of threads to use.\n"
    << "Supported: 2D/3D.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
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
  if( inValues.size() != outValues.size() )
  {
    std::cerr << "ERROR: Number of values following after \"-iv\" and \"-ov\" should be equal." << std::endl;
    return EXIT_FAILURE;
  }

  /** Get the number of classes to segment (not mandatory) */
  unsigned char numberOfClasses = 2;
  parser->GetCommandLineArgument( "-n", numberOfClasses );

  /** Get the prior probability images (not mandatory) */
  std::vector< std::string >  priorProbImageFileNames;
  bool retP = parser->GetCommandLineArgument( "-P", priorProbImageFileNames );
  if(retP)
  {
    if( priorProbImageFileNames.size() != numberOfClasses )
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
      return EXIT_FAILURE;
    }
  }

  /** Get the prior probabilities (not mandatory) */
  std::vector< float >  priorProbs(0);
  bool retp = parser->GetCommandLineArgument( "-p", priorProbs );
  if( retp && !retP )
  {
    if( priorProbs.size() != numberOfClasses )
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
      return EXIT_FAILURE;
    }
  }
  if( retp && retP )
  {
    std::cout << "WARNING: \"-p\" is ignored when \"-P\" is given as well!" << std::endl;
  }

  /** Get the trust factor for each observer (not mandatory) */
  std::vector< float > trust(0);
  bool rett = parser->GetCommandLineArgument( "-t", trust );
  if( rett )
  {
    if( trust.size() != inputSegmentationFileNames.size() )
    {
      std::cerr
        << "ERROR: Number of trust factors should be equal to the number of "
        << "input segmentations."
        << std::endl;
      std::cerr
        << "i.e., \"-t\" should be followed by "
        << inputSegmentationFileNames.size()
        << " numbers or just totally omitted." << std::endl;
      return EXIT_FAILURE;
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
  if( retouts )
  {
    if( softOutputFileNames.size() != numberOfClasses  &&
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
      return EXIT_FAILURE;
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
  for( unsigned int i = 0; i < numberOfClasses; ++i )
  {
    prefOrder[ i ] = i;
  }
  parser->GetCommandLineArgument( "-ord", prefOrder );

  /** Use compression */
  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Threads. */
  unsigned int maximumNumberOfThreads
    = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  parser->GetCommandLineArgument(
    "-threads", maximumNumberOfThreads );
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(
    maximumNumberOfThreads );

  /** Determine image properties. */
  itk::IOPixelEnum pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentEnum componentType = itk::IOComponentEnum::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputSegmentationFileNames[0], pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << componentType << std::endl;
  std::cout << "\tDimension:          " << dim << std::endl;
  std::cout << "\tNumberOfComponents: " << numberOfComponents << std::endl;

  /** Check for vector images. */
  bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  if( !retNOCCheck ) return EXIT_FAILURE;

  /** Class that does the work. */
  ITKToolsCombineSegmentationsBase * filter = 0;

  try
  {
    // now call all possible template combinations.
    //if( !filter ) filter = ITKToolsCombineSegmentations< 2, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCombineSegmentations< 2, unsigned char >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    //if( !filter ) filter = ITKToolsCombineSegmentations< 3, char >::New( dim, componentType );
    if( !filter ) filter = ITKToolsCombineSegmentations< 3, unsigned char >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputSegmentationFileNames = inputSegmentationFileNames;
    filter->m_PriorProbImageFileNames = priorProbImageFileNames;
    filter->m_SoftOutputFileNames = softOutputFileNames;
    filter->m_HardOutputFileName = hardOutputFileName;
    filter->m_ConfusionOutputFileName = confusionOutputFileName;
    filter->m_NumberOfClasses = numberOfClasses;
    filter->m_PriorProbs = priorProbs;
    filter->m_Trust = trust;
    filter->m_TerminationThreshold = terminationThreshold;
    filter->m_CombinationMethod = combinationMethod;
    filter->m_UseMask = useMask;
    filter->m_MaskDilationRadius = maskDilationRadius;
    filter->m_PrefOrder = prefOrder;
    filter->m_InValues = inValues;
    filter->m_OutValues = outValues;
    filter->m_UseCompression = useCompression;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
