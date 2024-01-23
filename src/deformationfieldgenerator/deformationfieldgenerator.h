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
#ifndef __deformationfieldgenerator_h_
#define __deformationfieldgenerator_h_

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


/** \class ITKToolsDeformationFieldGeneratorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsDeformationFieldGeneratorBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsDeformationFieldGeneratorBase()
  {
    this->m_InputImage1FileName = "";
    this->m_InputImage2FileName = "";
    this->m_InputPoints1FileName = "";
    this->m_InputPoints2FileName = "";
    this->m_OutputImageFileName = "";
    this->m_KernelName = "";
    this->m_Stiffness = 0.0f;
  };
  /** Destructor. */
  ~ITKToolsDeformationFieldGeneratorBase(){};

  /** Input member parameters. */
  std::string m_InputImage1FileName;
  std::string m_InputImage2FileName;
  std::string m_InputPoints1FileName;
  std::string m_InputPoints2FileName;
  std::string m_OutputImageFileName;
  std::string m_KernelName;
  double m_Stiffness;

}; // end class ITKToolsDeformationFieldGeneratorBase


/** \class ITKToolsDeformationFieldGenerator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsDeformationFieldGenerator : public ITKToolsDeformationFieldGeneratorBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsDeformationFieldGenerator Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsDeformationFieldGenerator(){};
  ~ITKToolsDeformationFieldGenerator(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef short InputPixelType;
    typedef TComponentType DeformationVectorValueType;
    typedef double CoordRepType;

    typedef itk::Image< InputPixelType, VDimension >        InputImageType;
    typedef itk::ImageFileReader< InputImageType >          InputImageReaderType;
    typedef itk::Vector<
      DeformationVectorValueType, VDimension >              DeformationVectorType;
    typedef itk::Image< DeformationVectorType, VDimension > DeformationFieldType;
    typedef itk::ImageRegionIteratorWithIndex<
      DeformationFieldType >                                DeformationFieldIteratorType;
    typedef itk::ImageFileWriter< DeformationFieldType >    DeformationFieldWriterType;
    typedef typename DeformationFieldType::IndexType        IndexType;
    typedef typename DeformationFieldType::PointType        PointType;

    typedef typename DeformationFieldType::RegionType       RegionType;
    typedef typename DeformationFieldType::PointType        OriginType;
    typedef typename DeformationFieldType::SpacingType      SpacingType;
    typedef typename DeformationFieldType::IndexType        IndexType;
    typedef typename IndexType::IndexValueType              IndexValueType;

    typedef itk::KernelTransform<
      CoordRepType, VDimension>                             KernelTransformType;
    typedef itk::ThinPlateSplineKernelTransform<
      CoordRepType, VDimension>                             TPSTransformType;
    typedef itk::ThinPlateR2LogRSplineKernelTransform<
      CoordRepType, VDimension>                             TPSR2LOGRTransformType;
    typedef itk::VolumeSplineKernelTransform<
      CoordRepType, VDimension>                             VSTransformType;
    typedef itk::ElasticBodySplineKernelTransform<
      CoordRepType, VDimension>                             EBSTransformType;
    typedef itk::ElasticBodyReciprocalSplineKernelTransform<
      CoordRepType, VDimension>                             EBSRTransformType;

    typedef typename KernelTransformType::PointSetType      PointSetType;
    typedef itk::TransformixInputPointFileReader<
      PointSetType >                                        IPPReaderType;

    /** Declarations */
    typename InputImageReaderType::Pointer reader1 = InputImageReaderType::New();
    typename InputImageReaderType::Pointer reader2 = InputImageReaderType::New();
    typename IPPReaderType::Pointer ipp1Reader = IPPReaderType::New();
    typename IPPReaderType::Pointer ipp2Reader = IPPReaderType::New();
    typename PointSetType::Pointer inputPointSet1 = nullptr;
    typename PointSetType::Pointer inputPointSet2 = nullptr;
    typename KernelTransformType::Pointer kernelTransform = nullptr;
    typename DeformationFieldType::Pointer deformationField = DeformationFieldType::New();
    typename DeformationFieldWriterType::Pointer writer = DeformationFieldWriterType::New();

    ipp1Reader->SetFileName( this->m_InputPoints1FileName.c_str() );
    std::cout << "Reading input point file 1: "
      << this->m_InputPoints1FileName << std::endl;
    ipp1Reader->Update();

    if( ipp1Reader->GetPointsAreIndices() )
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

    ipp2Reader->SetFileName( this->m_InputPoints2FileName.c_str() );
    std::cout << "Reading input point file 2: "
      << this->m_InputPoints2FileName << std::endl;
    ipp2Reader->Update();

    if( ipp2Reader->GetPointsAreIndices() )
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

    if( nrofpoints2 != nrofpoints1 )
    {
      itkGenericExceptionMacro( << "Number of input points does not equal number of output points!" );
    }
    const unsigned int nrofpoints = nrofpoints1;

    /** Read input images */
    std::cout << "Reading Input image(s)." << std::endl;
    reader1->SetFileName( this->m_InputImage1FileName.c_str() );
    reader1->UpdateOutputInformation();
    if( ipp2Reader->GetPointsAreIndices() )
    {
      if( this->m_InputImage2FileName!=""  )
      {
        reader2->SetFileName( this->m_InputImage2FileName.c_str() );
        reader2->UpdateOutputInformation();
      }
      else
      {
        std::cerr << "The input points in " << this->m_InputImage2FileName
          << " are given as indices, but no accompanying image is provided." << std::endl;
        itkGenericExceptionMacro( << "Second input image is needed!" );
      }
    }

    /** Convert from index to point, if necessary */
    if( ipp1Reader->GetPointsAreIndices() )
    {
      typename DeformationFieldType::Pointer dummyImage = DeformationFieldType::New();
      dummyImage->SetSpacing( reader1->GetOutput()->GetSpacing() );
      dummyImage->SetOrigin( reader1->GetOutput()->GetOrigin() );
      dummyImage->SetRegions( reader1->GetOutput()->GetLargestPossibleRegion() );
      typename PointSetType::Pointer tempPointSet = PointSetType::New();

      PointType point; point.Fill( 0.0 );
      IndexType index;
      for ( unsigned int j = 0; j < nrofpoints; j++ )
      {
        inputPointSet1->GetPoint( j, &point );
        for ( unsigned int i = 0; i < VDimension; i++ )
        {
          index[i] = static_cast< IndexValueType >( vnl_math_rnd( point[i] ) );
        }
        dummyImage->TransformIndexToPhysicalPoint( index, point );
        tempPointSet->SetPoint( j, point );
      }
      inputPointSet1 = tempPointSet;
    }

    /** Convert from index to point, if necessary */
    if( ipp2Reader->GetPointsAreIndices() )
    {
      typename DeformationFieldType::Pointer dummyImage = DeformationFieldType::New();
      dummyImage->SetSpacing( reader2->GetOutput()->GetSpacing() );
      dummyImage->SetOrigin( reader2->GetOutput()->GetOrigin() );
      dummyImage->SetRegions( reader2->GetOutput()->GetLargestPossibleRegion() );
      typename PointSetType::Pointer tempPointSet = PointSetType::New();

      PointType point; point.Fill( 0.0 );
      IndexType index;
      for ( unsigned int j = 0; j < nrofpoints; j++ )
      {
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

    if( this->m_KernelName == "TPS" )
    {
      kernelTransform = TPSTransformType::New();
    }
    else if( this->m_KernelName == "TPSR2LOGR" )
    {
      kernelTransform = TPSR2LOGRTransformType::New();
    }
    else if( this->m_KernelName == "VS" )
    {
      kernelTransform = VSTransformType::New();
    }
    else if( this->m_KernelName == "EBS" )
    {
      kernelTransform = EBSTransformType::New();
    }
    else if( this->m_KernelName == "EBSR" )
    {
      kernelTransform = EBSRTransformType::New();
    }
    else
    {
      std::cerr << "Invalid kernel transform type: " << this->m_KernelName << std::endl;
      itkGenericExceptionMacro( << "Unknown kernel transform!." );
    }

    kernelTransform->SetStiffness( this->m_Stiffness );
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

    std::cout << "Saving deformation field to disk as " << this->m_OutputImageFileName << std::endl;
    writer->SetFileName( this->m_OutputImageFileName.c_str() );
    writer->SetInput( deformationField );
    writer->Update();

  } // end Run()

}; // end class ITKToolsDeformationFieldGenerator


#endif // end #ifndef __deformationfieldgenerator_h_
