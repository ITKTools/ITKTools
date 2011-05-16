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
 \brief This program generates a deformation field (from fixed to moving image) based on some corresponding points.
 
 \verbinclude deformationfieldgenerator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformixInputPointFileReader.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkThinPlateSplineKernelTransform.h"
#include "itkThinPlateR2LogRSplineKernelTransform.h"
#include "itkVolumeSplineKernelTransform.h"
#include "itkElasticBodySplineKernelTransform.h"
#include "itkElasticBodyReciprocalSplineKernelTransform.h"
#include "vnl/vnl_math.h"


//-------------------------------------------------------------------------------------

/**
  * ******************* GetHelpString *******************
  */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program generates a deformation field (from fixed" << std::endl
  << "to moving image) based on some corresponding points." << std::endl
  << "Usage:" << std::endl
  << "pxdeformationfieldgenerator" << std::endl
    << "-in1     inputFilename1: the fixed image on which the" << std::endl
    << "          deformaton field must be defined." << std::endl
    << "[-in2]   inputFilename2: only needed to convert from" << std::endl
    << "          indices to point if the second input point" << std::endl
    << "          contains indices." << std::endl
    << "-ipp1    inputPointFile1: a transformix style input point file" << std::endl
    << "          with points in the fixed image." << std::endl
    << "-ipp2    inputPointFile2: a transformix style input point file" << std::endl
    << "          with the corresponding points in the moving image." << std::endl
    << "[-s]     stiffness: a number that allows to vary between" << std::endl
    << "          interpolating and approximating spline." << std::endl
    << "          0.0 = interpolating = default." << std::endl
    << "          Stiffness values are usually rather small," << std::endl
    << "          typically in the range of 0.001 to 0.1." << std::endl
    << "[-k]     kernelType: the type of kernel transform that's used to" << std::endl
    << "          generate the deformation field." << std::endl
    << "          TPS: thin plate spline (default)" << std::endl
    << "          TPSR2LOGR: thin plate spline R2logR" << std::endl
    << "          VS: volume spline" << std::endl
    << "          EBS: elastic body spline" << std::endl
    << "          EBSR: elastic body reciprocal spline" << std::endl
    << "          See ITK documentation and the there cited paper" << std::endl
    << "          for more information on these methods." << std::endl
    << "-out     outputFilename: the name of the resulting deformation field," << std::endl
    << "          which is written as a vector<float,dim> image." << std::endl
  << "Supported: 2D, 3D, any scalar pixeltype.";
  return ss.str();
} // end GetHelpString


/** DeformationFieldGenerator */

class ITKToolsDeformationFieldGeneratorBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsDeformationFieldGeneratorBase()
  {
    m_InputImage1FileName = "";
    m_InputImage2FileName = "";
    m_InputPoints1FileName = "";
    m_InputPoints2FileName = "";
    m_OutputImageFileName = "";
    m_KernelName = "";
    m_Stiffness = 0.0f;
  };
  ~ITKToolsDeformationFieldGeneratorBase(){};

  /** Input parameters */
  std::string m_InputImage1FileName;
  std::string m_InputImage2FileName;
  std::string m_InputPoints1FileName;
  std::string m_InputPoints2FileName;
  std::string m_OutputImageFileName;
  std::string m_KernelName;
  double m_Stiffness;
    
}; // end CropImageBase


template< unsigned int VDimension >
class ITKToolsDeformationFieldGenerator : public ITKToolsDeformationFieldGeneratorBase
{
public:
  typedef ITKToolsDeformationFieldGenerator Self;

  ITKToolsDeformationFieldGenerator(){};
  ~ITKToolsDeformationFieldGenerator(){};

  static Self * New( unsigned int dimension )
  {
    if ( VDimension == dimension )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs. */
    typedef short InputPixelType;
    typedef float DeformationVectorValueType;
    typedef double CoordRepType;

    typedef itk::Image<InputPixelType, VDimension>       InputImageType;
    typedef itk::ImageFileReader< InputImageType >      InputImageReaderType;

    typedef itk::Vector<
      DeformationVectorValueType, VDimension>            DeformationVectorType;
    typedef itk::Image<DeformationVectorType, VDimension> DeformationFieldType;
    typedef itk::ImageRegionIteratorWithIndex<
      DeformationFieldType>                             DeformationFieldIteratorType;
    typedef itk::ImageFileWriter<DeformationFieldType>  DeformationFieldWriterType;
    typedef typename DeformationFieldType::IndexType    IndexType;
    typedef typename DeformationFieldType::PointType    PointType;

    typedef typename DeformationFieldType::RegionType   RegionType;
    typedef typename DeformationFieldType::PointType    OriginType;
    typedef typename DeformationFieldType::SpacingType  SpacingType;
    typedef typename DeformationFieldType::IndexType    IndexType;
    typedef typename IndexType::IndexValueType          IndexValueType;

    typedef itk::KernelTransform<
      CoordRepType, VDimension>                     KernelTransformType;
    typedef itk::ThinPlateSplineKernelTransform<
      CoordRepType, VDimension>                     TPSTransformType;
    typedef itk::ThinPlateR2LogRSplineKernelTransform<
      CoordRepType, VDimension>                     TPSR2LOGRTransformType;
    typedef itk::VolumeSplineKernelTransform<
      CoordRepType, VDimension>                     VSTransformType;
    typedef itk::ElasticBodySplineKernelTransform<
      CoordRepType, VDimension>                     EBSTransformType;
    typedef itk::ElasticBodyReciprocalSplineKernelTransform<
      CoordRepType, VDimension>                     EBSRTransformType;

    typedef typename KernelTransformType::PointSetType  PointSetType;
    typedef itk::TransformixInputPointFileReader<
      PointSetType >                                    IPPReaderType;

    /** Declarations */
    typename InputImageReaderType::Pointer reader1 = InputImageReaderType::New();
    typename InputImageReaderType::Pointer reader2 = InputImageReaderType::New();
    typename IPPReaderType::Pointer ipp1Reader = IPPReaderType::New();
    typename IPPReaderType::Pointer ipp2Reader = IPPReaderType::New();
    typename PointSetType::Pointer inputPointSet1 = 0;
    typename PointSetType::Pointer inputPointSet2 = 0;
    typename KernelTransformType::Pointer kernelTransform = 0;
    typename DeformationFieldType::Pointer deformationField = DeformationFieldType::New();
    typename DeformationFieldWriterType::Pointer writer = DeformationFieldWriterType::New();

    ipp1Reader->SetFileName( m_InputPoints1FileName.c_str() );
    std::cout << "Reading input point file 1: " << m_InputPoints1FileName << std::endl;
    try
    {
      ipp1Reader->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Error while opening input point file 1." << std::endl;
      std::cerr << err << std::endl;
    }

    if ( ipp1Reader->GetPointsAreIndices() )
    {
      std::cout << "  Input points are specified as image indices." << std::endl;
    }
    else
    {
      std::cout << "  Input points are specified in world coordinates." << std::endl;
    }
    unsigned int nrofpoints1 = ipp1Reader->GetNumberOfPoints();
    std::cout << "  Number of specified input points: " << nrofpoints1 << std::endl;
    inputPointSet1 = ipp1Reader->GetOutput();

    ipp2Reader->SetFileName( m_InputPoints2FileName.c_str() );
    std::cout << "Reading input point file 2: " << m_InputPoints2FileName << std::endl;
    try
    {
      ipp2Reader->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Error while opening input point file 2." << std::endl;
      std::cerr << err << std::endl;
    }

    if ( ipp2Reader->GetPointsAreIndices() )
    {
      std::cout << "  Input points are specified as image indices." << std::endl;
    }
    else
    {
      std::cout << "  Input points are specified in world coordinates." << std::endl;
    }
    unsigned int nrofpoints2 = ipp2Reader->GetNumberOfPoints();
    std::cout << "  Number of specified input points: " << nrofpoints2 << std::endl;
    inputPointSet2 = ipp2Reader->GetOutput();

    if ( nrofpoints2 != nrofpoints1 )
    {
      itkGenericExceptionMacro( << "Number of input points does not equal number of output points!" );
    }
    const unsigned int nrofpoints = nrofpoints1;


    /** Read input images */
    std::cout << "Reading Input image(s)." << std::endl;
    reader1->SetFileName( m_InputImage1FileName.c_str() );
    reader1->UpdateOutputInformation();
    if ( ipp2Reader->GetPointsAreIndices() )
    {
      if ( m_InputImage2FileName!=""  )
      {
	reader2->SetFileName( m_InputImage2FileName.c_str() );
	reader2->UpdateOutputInformation();
      }
      else
      {
	std::cerr << "The input points in " << m_InputImage2FileName
	  << " are given as indices, but no accompanying image is provided." << std::endl;
	itkGenericExceptionMacro( << "Second input image is needed!" );
      }
    }


    /** Convert from index to point, if necessary */
    if ( ipp1Reader->GetPointsAreIndices() )
    {
      typename DeformationFieldType::Pointer dummyImage = DeformationFieldType::New();
      dummyImage->SetSpacing( reader1->GetOutput()->GetSpacing() );
      dummyImage->SetOrigin( reader1->GetOutput()->GetOrigin() );
      dummyImage->SetRegions( reader1->GetOutput()->GetLargestPossibleRegion() );
      typename PointSetType::Pointer tempPointSet = PointSetType::New();
      for ( unsigned int j = 0; j < nrofpoints; j++ )
      {
	PointType point;
	IndexType index;
	inputPointSet1->GetPoint(j, &point);
	for ( unsigned int i = 0; i < VDimension; i++ )
	{
	  index[i] = static_cast< IndexValueType >( vnl_math_rnd( point[i] ) );
	}
	dummyImage->TransformIndexToPhysicalPoint( index, point );
	tempPointSet->SetPoint(j, point);
      }
      inputPointSet1 = tempPointSet;
    }

    /** Convert from index to point, if necessary */
    if ( ipp2Reader->GetPointsAreIndices() )
    {
      typename DeformationFieldType::Pointer dummyImage = DeformationFieldType::New();
      dummyImage->SetSpacing( reader2->GetOutput()->GetSpacing() );
      dummyImage->SetOrigin( reader2->GetOutput()->GetOrigin() );
      dummyImage->SetRegions( reader2->GetOutput()->GetLargestPossibleRegion() );
      typename PointSetType::Pointer tempPointSet = PointSetType::New();
      for ( unsigned int j = 0; j < nrofpoints; j++ )
      {
	PointType point;
	IndexType index;
	inputPointSet2->GetPoint(j, &point);
	for ( unsigned int i = 0; i < VDimension; i++ )
	{
	  index[i] = static_cast< IndexValueType >( vnl_math_rnd( point[i] ) );
	}
	dummyImage->TransformIndexToPhysicalPoint( index, point );
	tempPointSet->SetPoint(j, point);
      }
      inputPointSet2 = tempPointSet;
    }

    if ( m_KernelName == "TPS" )
    {
      kernelTransform = TPSTransformType::New();
    }
    else if ( m_KernelName == "TPSR2LOGR" )
    {
      kernelTransform = TPSR2LOGRTransformType::New();
    }
    else if ( m_KernelName == "VS" )
    {
      kernelTransform = VSTransformType::New();
    }
    else if ( m_KernelName == "EBS" )
    {
      kernelTransform = EBSTransformType::New();
    }
    else if ( m_KernelName == "EBSR" )
    {
      kernelTransform = EBSRTransformType::New();
    }
    else
    {
      std::cerr << "Invalid kernel transform type: " << m_KernelName << std::endl;
      itkGenericExceptionMacro( << "Unknown kernel transform!." );
    }

    kernelTransform->SetStiffness( m_Stiffness );
    kernelTransform->SetSourceLandmarks( inputPointSet1 );
    kernelTransform->SetTargetLandmarks( inputPointSet2 );
    kernelTransform->ComputeWMatrix();

    /** Define the deformation field and an iterator on it */
    deformationField->SetSpacing( reader1->GetOutput()->GetSpacing() );
    deformationField->SetOrigin( reader1->GetOutput()->GetOrigin() );
    deformationField->SetRegions( reader1->GetOutput()->GetLargestPossibleRegion() );
    deformationField->Allocate();
    DeformationFieldIteratorType iterator(
      deformationField, deformationField->GetLargestPossibleRegion() );
    iterator.GoToBegin();

    std::cout << "Generating deformation field. " << std::endl;

    while ( !iterator.IsAtEnd() )
    {
      PointType pointin;
      const IndexType & index = iterator.GetIndex();
      deformationField->TransformIndexToPhysicalPoint( index, pointin);
      PointType pointout = kernelTransform->TransformPoint( pointin );
      DeformationVectorType vec = pointout - pointin;
      iterator.Set( vec );
      ++iterator;
    }

    std::cout << "Saving deformation field to disk as " << m_OutputImageFileName << std::endl;
    writer->SetFileName( m_OutputImageFileName.c_str() );
    writer->SetInput( deformationField );
    writer->Update();
  }

}; // end DeformationFieldGenerator

//-------------------------------------------------------------------------------------


/** Declare other functions. */
std::string GetHelpString(void);


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  parser->MarkArgumentAsRequired( "-in1", "The inputImage1 filename." );
  parser->MarkArgumentAsRequired( "-ipp1", "The inputPoints1 filename." );
  parser->MarkArgumentAsRequired( "-ipp2", "The inputPoints2 filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  std::string inputImage1FileName = "";
  std::string inputImage2FileName = "";
  std::string inputPoints1FileName = "";
  std::string inputPoints2FileName = "";
  std::string outputImageFileName = "";
  std::string kernelName = "TPS";
  double stiffness = 0.0;

  /** Get arguments. */

  parser->GetCommandLineArgument( "-in1", inputImage1FileName );
  parser->GetCommandLineArgument( "-in2", inputImage2FileName );
  parser->GetCommandLineArgument( "-ipp1", inputPoints1FileName );
  parser->GetCommandLineArgument( "-ipp2", inputPoints2FileName );
  parser->GetCommandLineArgument( "-out", outputImageFileName );
  parser->GetCommandLineArgument( "-k", kernelName );
  parser->GetCommandLineArgument( "-s", stiffness );

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputImage1FileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  if (NumberOfComponents > 1)
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }


  /** Class that does the work */
  ITKToolsDeformationFieldGeneratorBase * deformationFieldGenerator = NULL;

  /** Short alias */
  unsigned int dim = Dimension;

  try
  {    
    // now call all possible template combinations.
    if (!deformationFieldGenerator) deformationFieldGenerator = ITKToolsDeformationFieldGenerator< 2 >::New( dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!deformationFieldGenerator) deformationFieldGenerator = ITKToolsDeformationFieldGenerator< 3 >::New( dim );
#endif
    if (!deformationFieldGenerator) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << " dimension = " << Dimension
        << std::endl;
      return 1;
    }

    deformationFieldGenerator->m_InputImage1FileName = inputImage1FileName;
    deformationFieldGenerator->m_InputImage2FileName = inputImage2FileName;
    deformationFieldGenerator->m_InputPoints1FileName = inputPoints1FileName;
    deformationFieldGenerator->m_InputPoints2FileName = inputPoints2FileName;
    deformationFieldGenerator->m_OutputImageFileName = outputImageFileName;
    deformationFieldGenerator->m_KernelName = kernelName;
    deformationFieldGenerator->m_Stiffness = stiffness;
  
    deformationFieldGenerator->Run();
    
    delete deformationFieldGenerator;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete deformationFieldGenerator;
    return 1;
  }
  

  /** End program. */
  return 0;

} // end main
