
#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDeformationFieldJacobianDeterminantFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"


/** 
 * *************** Deformation2Transformation *********************
 * convert between deformation fields and transformation 'fields'
 */

template< class TImage>
void Deformation2Transformation(
  TImage * inputImage,
  const std::string & outputFileName,
  bool def2trans) 
{
  
  //inputimagetype = outputimagetype
  typedef TImage                                      ImageType; 
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef typename ImageType::PixelType               PixelType;
  typedef typename PixelType::ValueType               ComponentType;
  typedef itk::ImageFileWriter< ImageType >     			WriterType;
  typedef itk::ImageRegionIteratorWithIndex<
    ImageType >                                       IteratorType;
  typedef typename ImageType::IndexType               IndexType;
  typedef typename ImageType::PointType               PointType;
	  
  typename WriterType::Pointer writer = WriterType::New();

  /** We are going to change the image, so make sure these changes are not undone */
  inputImage->Update();
  inputImage->DisconnectPipeline();

  IteratorType it(inputImage, inputImage->GetLargestPossibleRegion());
  it.GoToBegin();
  double plusormin1 = 1.0;
  std::string message = "from deformation to transformation";
  if (!def2trans)
  {
    plusormin1 = -1.0;
    message = "from transformation to deformation";
  }
  std::cout << "Changing image " << message << "..." << std::endl;
  while ( !( it.IsAtEnd() ) )
  {
    const IndexType & index = it.GetIndex();
    PixelType & value = it.Value();
    PointType point;
    inputImage->TransformIndexToPhysicalPoint(index,point);
    for (unsigned int i=0; i < Dimension; ++i)
    {
      value[i] += static_cast<ComponentType>( plusormin1 * point[i] );
    }    
    ++it;
  }
  std::cout << "Ready changing image " << message << "." << std::endl;
  
	/** Write the output image. */
	writer->SetInput( inputImage );
	writer->SetFileName( outputFileName.c_str() );
  std::cout << "Saving the resulting image to disk as: " << outputFileName << std::endl;
	writer->Update();
	std::cout << "Done." << std::endl;

} // end Deformation2Transformation


/** 
 * ******************* ComputeMagnitude ************************
 * write magnitude of deformation field to disk 
 */

template<class TVectorImage, class TScalarImage>
void ComputeMagnitude(
  TVectorImage * inputImage,
  const std::string & outputFileName)
{
  typedef TVectorImage                                InputImageType; 
  typedef TScalarImage                                OutputImageType; 
  typedef itk::ImageFileWriter< OutputImageType >			WriterType;
  typedef itk::GradientToMagnitudeImageFilter<
    InputImageType, OutputImageType >                 MagnitudeFilterType;

  typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
  typename WriterType::Pointer writer = WriterType::New();
    
  magnitudeFilter->SetInput( inputImage );
  std::cout << "Computing magnitude image..." << std::endl;
  magnitudeFilter->Update();
  std::cout << "Done computing magnitude image." << std::endl;

  /** Write the output image. */
	writer->SetInput( magnitudeFilter->GetOutput() );
	writer->SetFileName( outputFileName.c_str() );
  std::cout << "Saving the resulting image to disk as: " << outputFileName << std::endl;
	writer->Update();
	std::cout << "Done." << std::endl;

} // end ComputeMagnitude


/** 
 * ******************* ComputeJacobian ************************
 * write jacobian of deformation field to disk 
 */

template<class TVectorImage, class TScalarImage>
void ComputeJacobian(
  TVectorImage * inputImage,
  const std::string & outputFileName)
{
  typedef TVectorImage                                InputImageType; 
  typedef TScalarImage                                OutputImageType; 
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef itk::ImageFileWriter< OutputImageType >			WriterType;
  typedef itk::DeformationFieldJacobianDeterminantFilter<
    InputImageType, OutputPixelType >                 JacobianFilterType;
  
  typename JacobianFilterType::Pointer jacobianFilter = JacobianFilterType::New();
  typename WriterType::Pointer writer = WriterType::New();

  jacobianFilter->SetUseImageSpacingOn();    
  jacobianFilter->SetInput( inputImage );
  std::cout << "Computing jacobian image..." << std::endl;
  jacobianFilter->Update();
  std::cout << "Done computing jacobian image." << std::endl;

  /** Write the output image. */
	writer->SetInput( jacobianFilter->GetOutput() );
	writer->SetFileName( outputFileName.c_str() );
  std::cout << "Saving the resulting image to disk as: " << outputFileName << std::endl;
	writer->Update();
	std::cout << "Done." << std::endl;

} // end ComputeJacobian



/**
 * **************** DeformationFieldOperator *******************
 *
 * converts between deformation fields and transformation 'fields',
 * and compute magnitudes/jacobians.
 */

/** Declare ThresholdImage. */
template< class TComponent, unsigned int NDimension >
void DeformationFieldOperator(
  const std::string & inputFileName,
	const std::string & outputFileName,
  const std::string & ops)
{
	/** constants */
	const unsigned int Dimension = NDimension;
  	
	/** TYPEDEF's. */
	typedef TComponent                         					ComponentType;
  typedef ComponentType                               ScalarPixelType;
  typedef itk::Vector< ComponentType, Dimension >     VectorPixelType;

  typedef itk::Image< ScalarPixelType, Dimension >    ScalarImageType;
	typedef itk::Image< VectorPixelType, Dimension >    VectorImageType;
	typedef itk::ImageFileReader< VectorImageType >     ReaderType;
	
	/** DECLARATION'S. */
  typename VectorImageType::Pointer workingImage;
	typename ReaderType::Pointer reader = ReaderType::New();
	

	/** Read in the inputImage. */
	reader->SetFileName( inputFileName.c_str() );
  std::cout << "Reading input image: " << inputFileName << std::endl;		
	reader->Update();
	std::cout << "Input image read." << std::endl;
	
  /** Change to Transformation or Deformation by adding/subtracting pixel coordinates */
	workingImage = reader->GetOutput();

  /** Do something with this image and save the result */
  if ( ops == "DEF2TRANS" )
  {
    Deformation2Transformation<VectorImageType>(workingImage, outputFileName, true);
  }
  else if ( ops == "TRANS2DEF" )
  {
    Deformation2Transformation<VectorImageType>(workingImage, outputFileName, false);
  }
  else if ( ops == "MAGNITUDE" )
  {
    ComputeMagnitude<VectorImageType, ScalarImageType>(workingImage, outputFileName);
  }
  else if ( ops == "JACOBIAN" )
  {
    ComputeJacobian<VectorImageType, ScalarImageType>(workingImage, outputFileName);
  }
  else
  {
    itkGenericExceptionMacro(<< "<< invalid operator: " << ops );
  }
  

} // end ThresholdImage


