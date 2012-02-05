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
 \brief This program computes the distance between brains, based on a label mask image of one of the brains and a deformation field.
 
 \verbinclude braindistance.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDisplacementFieldJacobianDeterminantFilter.h"
#include "itkDeformationFieldBendingEnergyFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkCropImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkLogImageFilter.h"
#include "vnl/vnl_math.h"
#include <fstream>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program computes the distance between brains, based on\n"
    << "a label mask image of one of the brains and a deformation field.\n"
    << "Usage:\n"
    << "pxbraindistance\n"
    << "  -in      inputFilename: 3D deformation field\n"
    << "  -out     outputFilenames: two output filenames. The first\n"
    << "           one contains mu_tot and sigma_tot. the second one contains mu_i, sigma_i, and sigma_itot.\n"
    << "  -mask    maskFileName: the name of the label image (deformed HAMMER atlas)\n"
    << "  [-m]     method: 0 (jacobian), 1 (bending energy), or 2 (log(jacobian)); default: 0\n"
    << "Supported: -in: 3D vector of floats, 3 elements per vector\n"
    << "-mask: 3D unsigned char or anything that is valid after casting to unsigned char";
  return ss.str();

} // end GetHelpString()


//-------------------------------------------------------------------------------------

/* Declare ComputeBrainDistance function. */
void ComputeBrainDistance(
  const std::string & inputFileName,
  const std::string & maskFileName,
  const std::vector<std::string> & outputFileNames,
  unsigned int method);

//-------------------------------------------------------------------------------------


int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser
    = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-mask", "The mask filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filenames." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments (mandatory): input deformation field */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get arguments (mandatory): input deformation field */
  std::string maskFileName = "";
  parser->GetCommandLineArgument( "-mask", maskFileName );

  /** Get arguments (optional): method */
  unsigned int method = 0;
  parser->GetCommandLineArgument( "-m", method );

  /** Get arguments (mandatory): Output filenames */
  std::vector< std::string > outputFileNames;
  parser->GetCommandLineArgument( "-out", outputFileNames );
  if ( outputFileNames.size() != 2 )
  {
    std::cerr << "ERROR: You should specify \"-out\", followed by 2 filenames." << std::endl;
  }

  /** Determine image properties. */
  std::string ComponentTypeIn = "float";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = Dimension;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    std::cerr << "ERROR: error while getting image properties of the input image!"
      << std::endl;
    return 1;
  }
  if ( (Dimension != 3) || (NumberOfComponents != Dimension) )
  {
    std::cerr << "ERROR: the input image is not of the right format: 3D, "
      << "vectors of length 3 it should be!" << std::endl;
    return 1;
  }
  for ( unsigned int i = 0; i < Dimension; ++i )
  {
    if ( imagesize[i] < 3 )
    {
      std::cerr << "ERROR: the image is too small in one of the dimensions. "
        << "Minimum size is 3 for each dimension." << std::endl;
      return 1;
    }
  }

  /** Run the program. */
  try
  {
    ComputeBrainDistance( inputFileName, maskFileName, outputFileNames, method );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


//------------------------------------------------------------------

/* Function that does (x-b)^2 */
namespace itk
{
  namespace Function {

  template< class TInput, class TOutput = TInput>
  class SubtractSquare
  {
  public:
    typedef typename NumericTraits<TInput>::RealType RealType;

    SubtractSquare()
    {
      this->m_ScalarToSubtract = itk::NumericTraits<RealType>::Zero;
    }
    ~SubtractSquare() {}

    bool operator!=( const SubtractSquare & ) const
    {
      return false;
    }

    bool operator==( const SubtractSquare & other ) const
    {
      return !(*this != other);
    }

    void SetScalarToSubtract( RealType scalar )
    {
      this->m_ScalarToSubtract = scalar;
    }
    RealType GetScalarToSubtract( void )
    {
      return this->m_ScalarToSubtract;
    }

    inline TOutput operator()( const TInput & A )
    {
      const RealType ra = static_cast<RealType>( A ) - this->m_ScalarToSubtract;
      return static_cast<TOutput>( ra * ra );
    }
  protected:
    RealType m_ScalarToSubtract;

  }; // end class

  } // end Function namespace
} // end itk namespace

//---------------------------------------------------------------------

/* write a vector of doubles to an ostream */
std::ostream& operator<<(std::ostream& os, std::vector<double>& vec)
{
  for ( unsigned int i =0; i< (vec.size()-1); ++i)
  {
    os << vec[i] << "\t";
  }
  os << vec[ vec.size()-1 ];
  return os;
}

//------------------------------------------------------------------

/* Implement ComputeBrainDistance function. */
void ComputeBrainDistance(
  const std::string & inputFileName,
  const std::string & maskFileName,
  const std::vector<std::string> & outputFileNames,
  unsigned int method )
{
  /** Typedefs. */
  typedef float                                           InputComponentType;
  const unsigned int Dimension = 3;
  typedef itk::Vector<InputComponentType, Dimension>      InputPixelType;
  typedef unsigned char                                   MaskPixelType;
  typedef float                                           InternalPixelType;

  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::Image< MaskPixelType, Dimension >          MaskImageType;
  typedef itk::Image< InternalPixelType, Dimension >      InternalImageType;

  typedef itk::ImageFileReader< InputImageType >          InputReaderType;
  typedef itk::ImageFileReader< MaskImageType >           MaskReaderType;

  typedef itk::ImageRegionIteratorWithIndex<
    InputImageType >                                      IteratorType;
  typedef InputImageType::IndexType                       IndexType;
  typedef InputImageType::PointType                       PointType;
  typedef InputImageType::RegionType                      RegionType;
  typedef RegionType::SizeType                            SizeType;

  typedef itk::DisplacementFieldJacobianDeterminantFilter<
    InputImageType, InternalPixelType, InternalImageType> JacobianFilterType;
  typedef itk::DeformationFieldBendingEnergyFilter<
    InputImageType, InternalPixelType, InternalImageType> BendingEnergyFilterType;

  typedef itk::BinaryThresholdImageFilter<
    MaskImageType, MaskImageType >                        ThresholderType;
  typedef itk::LabelStatisticsImageFilter<
    InternalImageType, MaskImageType >                    StatFilterType;
  typedef itk::StatisticsImageFilter< MaskImageType >     MaximumComputerType;

  typedef itk::RegionOfInterestImageFilter<
    InternalImageType, InternalImageType >                JacobianCropFilterType;
  typedef itk::RegionOfInterestImageFilter<
    MaskImageType, MaskImageType >                        MaskCropFilterType;

  typedef itk::Function::SubtractSquare<
    InternalPixelType >                                   SubtractSquareFunctionType;
  typedef itk::UnaryFunctorImageFilter<
    InternalImageType, InternalImageType,
     SubtractSquareFunctionType >                         SubtractSquareFilterType;

  typedef itk::IntensityWindowingImageFilter<
    InternalImageType, InternalImageType >                WindowFilterType;
  typedef itk::LogImageFilter<
    InternalImageType, InternalImageType >                LogFilterType;

  /** Instantiate main variables */
  InputReaderType::Pointer inputReader = InputReaderType::New();
  InputImageType::Pointer inputImage = 0;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  MaskCropFilterType::Pointer maskCropFilter = MaskCropFilterType::New();
  MaskImageType::Pointer labelMask = 0;
  MaskImageType::Pointer brainMask = 0;
  InternalImageType::Pointer jacobian = 0;
  JacobianCropFilterType::Pointer jacobianCropFilter = JacobianCropFilterType::New();
  ThresholderType::Pointer thresholder = ThresholderType::New();
  StatFilterType::Pointer statFilterBrainMask = StatFilterType::New();
  StatFilterType::Pointer statFilterLabels = StatFilterType::New();
  StatFilterType::Pointer statFilterLabelsSpecial = StatFilterType::New();
  MaximumComputerType::Pointer maximumComputer  = MaximumComputerType::New();
  SubtractSquareFilterType::Pointer subsqFilter = SubtractSquareFilterType::New();
  WindowFilterType::Pointer windowFilter = WindowFilterType::New();
  LogFilterType::Pointer logFilter = LogFilterType::New();

  /** method 0 or 1 */
  JacobianFilterType::Pointer jacobianFilter = 0;
  if ( method==0 || method ==2 )
  {
    jacobianFilter = JacobianFilterType::New();
  }
  else
  {
    jacobianFilter = BendingEnergyFilterType::New();
  }

  /** Read image. */
  std::cout << "Reading input image..." << std::endl;
  inputReader->SetFileName( inputFileName );
  inputImage = inputReader->GetOutput();
  inputImage->Update();

  /** Compute the 'jacobian' (or bending energy) and crop */
  std::cout << "Computing jacobian image..." << std::endl;
  jacobianFilter->SetUseImageSpacingOn();
  jacobianFilter->SetInput( inputImage );
  jacobianFilter->UpdateLargestPossibleRegion();
  RegionType oldregion = jacobianFilter->GetOutput()->GetLargestPossibleRegion();
  SizeType oldsize = oldregion.GetSize();
  IndexType oldindex = oldregion.GetIndex();
  RegionType newregion;
  SizeType newsize;
  IndexType newindex;
  for (unsigned int d = 0; d < Dimension; ++d)
  {
    newsize[d] = oldsize[d] - 2;
    newindex[d] = oldindex[d] + 1;
  }
  newregion.SetIndex( newindex);
  newregion.SetSize( newsize );
  jacobianCropFilter->SetRegionOfInterest( newregion );
  jacobianCropFilter->SetInput( jacobianFilter->GetOutput() );
  jacobianCropFilter->Update();

  const double maxJac = 3.0;
  if ( method ==2 )
  {
    /** Clamp and take log */
    windowFilter->SetInput( jacobianCropFilter->GetOutput() );
    windowFilter->SetOutputMinimum( 1.0 / maxJac );
    windowFilter->SetWindowMinimum( 1.0 / maxJac );
    windowFilter->SetOutputMaximum( maxJac );
    windowFilter->SetWindowMaximum( maxJac );
    logFilter->SetInput( windowFilter->GetOutput() );
    logFilter->Update();
    jacobian = logFilter->GetOutput();
  }
  else
  {
    /** Do nothing */
    jacobian = jacobianCropFilter->GetOutput();
  }

  /** Read the label mask and crop */
  std::cout << "Reading label mask image..." << std::endl;
  maskReader->SetFileName( maskFileName );
  maskReader->Update();
  maskCropFilter->SetRegionOfInterest( newregion );
  maskCropFilter->SetInput( maskReader->GetOutput() );
  labelMask = maskCropFilter->GetOutput();
  labelMask->Update();

  /** Generate brain mask by thresholding at 0 (assumes Hammer atlas) */
  std::cout << "Thresholding label image..." << std::endl;
  thresholder->SetLowerThreshold( itk::NumericTraits<MaskPixelType>::Zero );
  thresholder->SetUpperThreshold( itk::NumericTraits<MaskPixelType>::Zero );
  thresholder->SetInsideValue( itk::NumericTraits<MaskPixelType>::Zero );
  thresholder->SetOutsideValue( itk::NumericTraits<MaskPixelType>::One );
  thresholder->SetInput( labelMask );
  brainMask = thresholder->GetOutput();
  brainMask->Update();

  /** Compute mu_tot and sigma_tot over brain mask */
  std::cout << "Compute mu_tot and sigma_tot over entire brain..."  << std::endl;
  statFilterBrainMask->SetInput( jacobian );
  statFilterBrainMask->SetLabelInput( brainMask );
  statFilterBrainMask->UseHistogramsOff();
  statFilterBrainMask->Update();
  double mu_tot = 0.0;
  double sigma_tot = 0.0;
  if ( statFilterBrainMask->HasLabel( 1 ) )
  {
    mu_tot = statFilterBrainMask->GetMean(1);
    sigma_tot = statFilterBrainMask->GetSigma(1);
  }
  else
  {
    itkGenericExceptionMacro( << "ERROR: the thresholded label mask image "
      << "does not contain any 1's" );
  }

  /** Compute maximum label nr */
  std::cout << "Compute maximum label nr..." << std::endl;
  maximumComputer->SetInput( labelMask );
  maximumComputer->Update();
  MaskPixelType maxLabelNr = maximumComputer->GetMaximum();

  /** Compute mean_i and sigma_i for each segment_i */
  std::cout << "Compute mu_i and sigma_i for each brain segment_i in the "
    << "label image..."  << std::endl;
  statFilterLabels->SetInput( jacobian );
  statFilterLabels->SetLabelInput( labelMask );
  statFilterLabels->UseHistogramsOff();
  statFilterLabels->Update();
  std::vector<double> mu_i( maxLabelNr + 1, 0.0);
  std::vector<double> sigma_i( maxLabelNr + 1, 0.0);
  for ( MaskPixelType i = 0; i <= maxLabelNr; ++i )
  {
    if ( statFilterLabels->HasLabel( i ) )
    {
      mu_i[ i ] = statFilterLabels->GetMean( i );
      sigma_i[ i ] = statFilterLabels->GetSigma( i );
    }
    else
    {
      /** Some bogus value which will never occur */
      mu_i[ i ] = -1000.0;
      sigma_i[ i ] = -1000.0;
    }
  }

  /** Compute (Jac - mu_tot)^2 image */
  std::cout << "Compute ( jacobian - mu_tot )^2... " << std::endl;
  subsqFilter->GetFunctor().SetScalarToSubtract( mu_tot );
  subsqFilter->SetInput( jacobian );
  subsqFilter->Update();

  /** Compute sigma_i,tot for each segment_i */
  std::cout << "Compute sigma_i,tot = sqrt[ mean[ ( jacobian - mu_tot )^2 ] ] "
    << "per segment_i... " << std::endl;
  statFilterLabelsSpecial->SetInput( subsqFilter->GetOutput() );
  statFilterLabelsSpecial->SetLabelInput( labelMask );
  statFilterLabelsSpecial->UseHistogramsOff();
  statFilterLabelsSpecial->Update();
  std::vector<double> sigma_itot( maxLabelNr + 1, 0.0);
  for ( MaskPixelType i = 0; i <= maxLabelNr; ++i )
  {
    if ( statFilterLabelsSpecial->HasLabel( i ) )
    {
      sigma_itot[ i ] = vcl_sqrt( statFilterLabelsSpecial->GetMean( i ) );
    }
    else
    {
      /** Some bogus value which will never occur */
      sigma_itot[ i ] = -1000.0;
    }
  }

  /** Write results to files */
  std::cout << "Write results to files" << std::endl;
  std::ofstream mutotsigmatot( outputFileNames[0].c_str() );
  if ( ! mutotsigmatot.is_open() )
  {
    itkGenericExceptionMacro( << "The output file " << outputFileNames[0]
    << " cannot be opened!" )
  }
  mutotsigmatot << mu_tot << "\t" << sigma_tot << std::endl;
  mutotsigmatot.close();
  std::ofstream musigmaperlabel( outputFileNames[1].c_str() );
  if ( ! musigmaperlabel.is_open() )
  {
    itkGenericExceptionMacro( << "The output file " << outputFileNames[1]
    << " cannot be opened!" )
  }
  musigmaperlabel << mu_i << std::endl;
  musigmaperlabel << sigma_i << std::endl;
  musigmaperlabel << sigma_itot << std::endl;
  musigmaperlabel.close();

  std::cout << "Ready!" << std::endl;

} // end ComputeBrainDistance()
