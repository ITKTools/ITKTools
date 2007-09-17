#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

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

/** run: A macro to call a function. */
#define run(function,dim) \
if ( Dimension == dim ) \
{ \
    function< dim >(  \
      inputImage1FileName, \
      inputImage2FileName, \
      inputPoints1FileName, \
      inputPoints2FileName, \
      outputImageFileName, \
      kernelName,          \
      stiffness );        \
    supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare DeformationFieldGenerator. */
template< unsigned int Dimension >
void DeformationFieldGenerator(
  const std::string & inputImage1FileName,
  const std::string & inputImage2FileName,
  const std::string & inputPoints1FileName,
  const std::string & inputPoints2FileName,
  const std::string & outputImageFileName,
  const std::string & kernelName,
  double stiffness );

/** Declare other functions. */
void PrintHelp(void);


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	/** Check arguments for help. */
	if ( argc < 9 || argc > 15 )
	{
		PrintHelp();
		return 1;
	}

	/** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  std::string inputImage1FileName = "";
  std::string inputImage2FileName = "";
  std::string inputPoints1FileName = "";
  std::string inputPoints2FileName = "";
  std::string outputImageFileName = "";
  std::string kernelName = "TPS"; 
  double stiffness = 0.0;
	
  /** Get arguments. */
	
	bool retin1  = parser->GetCommandLineArgument( "-in1", inputImage1FileName );
  bool retin2  = parser->GetCommandLineArgument( "-in2", inputImage2FileName );
  bool retipp1 = parser->GetCommandLineArgument( "-ipp1", inputPoints1FileName );
  bool retipp2 = parser->GetCommandLineArgument( "-ipp2", inputPoints2FileName );
  bool retout  = parser->GetCommandLineArgument( "-out", outputImageFileName );
  bool retk    = parser->GetCommandLineArgument( "-k", kernelName );
  bool retstif = parser->GetCommandLineArgument( "-s", stiffness );
	
	/** Check if the required arguments are given. */
	if ( !retin1 )
	{
		std::cerr << "ERROR: You should specify \"-in1\"." << std::endl;
		return 1;
	}
  if ( !retipp1 )
	{
		std::cerr << "ERROR: You should specify \"-ipp1\"." << std::endl;
		return 1;
	}
  if ( !retipp2 )
	{
		std::cerr << "ERROR: You should specify \"-ipp2\"." << std::endl;
		return 1;
	}
  if ( !retout )
	{
		std::cerr << "ERROR: You should specify \"-out\"." << std::endl;
		return 1;
	}

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string	PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
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
 	
	/** Run the program for 2D and 3D. */
  bool supported = false;
	try
	{
		run( DeformationFieldGenerator, 3 );
    run( DeformationFieldGenerator, 2 );
  }
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr <<
      "pixel (component) type = " << ComponentType <<
      " ; dimension = " << Dimension 
      << std::endl;
    return 1;
  }
	
	/** End program. */
	return 0;

} // end main


	/**
	 * ***************** DeformationFieldGenerator ****************
	 */

template< unsigned int Dimension >
void DeformationFieldGenerator(
  const std::string & inputImage1FileName,
  const std::string & inputImage2FileName,
  const std::string & inputPoints1FileName,
  const std::string & inputPoints2FileName,
  const std::string & outputImageFileName,
  const std::string & kernelName,
  double stiffness)
{
	/** Typedefs. */
  typedef short InputPixelType;
  typedef float DeformationVectorValueType;
  typedef double CoordRepType;
  
  typedef itk::Image<InputPixelType, Dimension>       InputImageType;
  typedef itk::ImageFileReader< InputImageType >	    InputImageReaderType;
  
  typedef itk::Vector<
    DeformationVectorValueType, Dimension>            DeformationVectorType; 
  typedef itk::Image<DeformationVectorType, Dimension> DeformationFieldType;
  typedef itk::ImageRegionIteratorWithIndex<
    DeformationFieldType>                             DeformationFieldIteratorType;  
  typedef itk::ImageFileWriter<DeformationFieldType>  DeformationFieldWriterType;
  typedef typename DeformationFieldType::IndexType    IndexType;
  typedef typename DeformationFieldType::PointType    PointType;

  typedef typename DeformationFieldType::RegionType		RegionType;
  typedef typename DeformationFieldType::PointType    OriginType;
  typedef typename DeformationFieldType::SpacingType  SpacingType;
  typedef typename DeformationFieldType::IndexType    IndexType;
  typedef typename IndexType::IndexValueType          IndexValueType;
  
  typedef itk::KernelTransform<
    CoordRepType, Dimension>                     KernelTransformType;
  typedef itk::ThinPlateSplineKernelTransform<
    CoordRepType, Dimension>                     TPSTransformType;
  typedef itk::ThinPlateR2LogRSplineKernelTransform<
    CoordRepType, Dimension>                     TPSR2LOGRTransformType;
  typedef itk::VolumeSplineKernelTransform<
    CoordRepType, Dimension>                     VSTransformType;
  typedef itk::ElasticBodySplineKernelTransform<
    CoordRepType, Dimension>                     EBSTransformType;
  typedef itk::ElasticBodyReciprocalSplineKernelTransform<
    CoordRepType, Dimension>                     EBSRTransformType;

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

  ipp1Reader->SetFileName( inputPoints1FileName.c_str() );
  std::cout << "Reading input point file 1: " << inputPoints1FileName << std::endl;
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

  ipp2Reader->SetFileName( inputPoints2FileName.c_str() );
  std::cout << "Reading input point file 2: " << inputPoints2FileName << std::endl;
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
  reader1->SetFileName( inputImage1FileName.c_str() );
  reader1->UpdateOutputInformation();
  if ( ipp2Reader->GetPointsAreIndices() )
  {
    if ( inputImage2FileName!=""  )
    {
      reader2->SetFileName( inputImage2FileName.c_str() );
      reader2->UpdateOutputInformation();
    }
    else
    {
      std::cerr << "The input points in " << inputImage2FileName
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
      for ( unsigned int i = 0; i < Dimension; i++ )
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
      for ( unsigned int i = 0; i < Dimension; i++ )
			{
				index[i] = static_cast< IndexValueType >( vnl_math_rnd( point[i] ) );
			}
			dummyImage->TransformIndexToPhysicalPoint( index, point );					
      tempPointSet->SetPoint(j, point);
		} 
    inputPointSet2 = tempPointSet;
	} 

  if ( kernelName == "TPS" )
  {  
    kernelTransform = TPSTransformType::New();
  }
  else if ( kernelName == "TPSR2LOGR" )
  {  
    kernelTransform = TPSR2LOGRTransformType::New();
  }
  else if ( kernelName == "VS" )
  {  
    kernelTransform = VSTransformType::New();
  }
  else if ( kernelName == "EBS" )
  {  
    kernelTransform = EBSTransformType::New();
  }
  else if ( kernelName == "EBSR" )
  {  
    kernelTransform = EBSRTransformType::New();
  }
  else
  {
    std::cerr << "Invalid kernel transform type: " << kernelName << std::endl;
    itkGenericExceptionMacro( << "Unknown kernel transform!." );
  }

  kernelTransform->SetStiffness( stiffness );
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

  std::cout << "Saving deformation field to disk as " << outputImageFileName << std::endl;
  writer->SetFileName( outputImageFileName.c_str() );
  writer->SetInput( deformationField );
  writer->Update();
  
} // end DeformationFieldGenerator


	/**
	 * ******************* PrintHelp *******************
	 */
void PrintHelp()
{
  std::cout << "This program generates a deformation field (from fixed " << std::endl;
  std::cout << "to moving image) based on some corresponding points." << std::endl;
  std::cout << "Usage:" << std::endl << "pxdeformationfieldgenerator" << std::endl;
  std::cout << "  -in1     inputFilename1: the fixed image on which the" << std::endl;
  std::cout << "            deformaton field must be defined." << std::endl;
  std::cout << "  [-in2]   inputFilename2: only needed to convert from" << std::endl;
  std::cout << "            indices to point if the second input point" << std::endl;
  std::cout << "            contains indices." << std::endl;
  std::cout << "  -ipp1    inputPointFile1: a transformix style input point file" << std::endl;
  std::cout << "            with points in the fixed image." << std::endl;
  std::cout << "  -ipp2    inputPointFile2: a transformix style input point file" << std::endl;
  std::cout << "            with the corresponding points in the moving image." << std::endl;
  std::cout << "  [-s]     stiffness: a number that allows to vary between" << std::endl;
  std::cout << "            interpolating and approximating spline." << std::endl;
  std::cout << "            0.0 = interpolating = default." << std::endl;
  std::cout << "            Stiffness values are usually rather small," << std::endl;
  std::cout << "            typically in the range of 0.001 to 0.1." << std::endl;
  std::cout << "  [-k]     kernelType: the type of kernel transform that's used to" << std::endl;
  std::cout << "            generate the deformation field." << std::endl;
  std::cout << "            TPS: thin plate spline (default)" << std::endl;
  std::cout << "            TPSR2LOGR: thin plate spline R2logR" << std::endl;
  std::cout << "            VS: volume spline" << std::endl;
  std::cout << "            EBS: elastic body spline" << std::endl;
  std::cout << "            EBSR: elastic body reciprocal spline" << std::endl;
  std::cout << "            See ITK documentation and the there cited paper" << std::endl;
  std::cout << "            for more information on these methods." << std::endl;
  std::cout << "  -out     outputFilename: the name of the resulting deformation field," << std::endl;
  std::cout << "            which is written as a vector<float,dim> image." << std::endl;
	std::cout << "Supported: 2D, 3D, any scalar pixeltype." << std::endl;
} // end PrintHelp



