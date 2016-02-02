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
#ifndef __ComputeOverlapOld_h_
#define __ComputeOverlapOld_h_

#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkAndImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkThresholdLabelerImageFilter.h"
#include "itkNumericTraits.h"

#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"


/** \class ITKToolsComputeOverlapOldBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsComputeOverlapOldBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsComputeOverlapOldBase()
  {
    //std::vector<std::string> m_InputFileNames;
    this->m_MaskFileName1 = "";
    this->m_MaskFileName2 = "";
    this->m_T1 = 0;
    this->m_T2 = 0;
    this->m_Tolerance = 1e-3;
  };
  /** Destructor. */
  ~ITKToolsComputeOverlapOldBase(){};

  /** Input member parameters */
  std::vector<std::string> m_InputFileNames;
  std::string m_MaskFileName1;
  std::string m_MaskFileName2;
  unsigned int m_T1;
  unsigned int m_T2;
  double m_Tolerance;

}; // end ITKToolsComputeOverlapOldBase


/** \class ITKToolsComputeOverlapOld
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsComputeOverlapOld : public ITKToolsComputeOverlapOldBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsComputeOverlapOld Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsComputeOverlapOld(){};
  ~ITKToolsComputeOverlapOld(){};

  /** Run function. */
  void Run( void )
  {
    /** Some typedef's. */
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef typename ImageType::Pointer                 ImagePointer;
    typedef typename ImageType::PixelType               PixelType;
    typedef itk::ImageFileReader<ImageType>             ImageReaderType;
    typedef typename ImageReaderType::Pointer           ImageReaderPointer;
    typedef itk::AndImageFilter<
      ImageType, ImageType, ImageType>                  AndFilterType;
    typedef typename AndFilterType::Pointer             AndFilterPointer;
    typedef itk::ImageRegionConstIterator<ImageType>    IteratorType;
    typedef itk::ThresholdLabelerImageFilter<
      ImageType, ImageType>                             ThresholdFilterType;
    typedef typename ThresholdFilterType::Pointer       ThresholdFilterPointer;
    typedef typename ThresholdFilterType::ThresholdVector ThresholdVectorType;

    /**
    * Setup pipeline
    */

    /** Create readers and an AND filter. */
    ImageReaderPointer reader1 = ImageReaderType::New();
    reader1->SetFileName( this->m_InputFileNames[ 0 ].c_str() );
    ImageReaderPointer reader2 = ImageReaderType::New();
    reader2->SetFileName( this->m_InputFileNames[ 1 ].c_str() );
    AndFilterPointer finalANDFilter = AndFilterType::New();
    finalANDFilter->SetCoordinateTolerance( this->m_Tolerance );
    finalANDFilter->SetDirectionTolerance( this->m_Tolerance );

    /** Create images, threshold filters, and threshold vectors. */
    ImagePointer im1 = 0;
    ImagePointer im2 = 0;
    ThresholdFilterPointer thresholder1 = 0;
    ThresholdFilterPointer thresholder2 = 0;
    ThresholdVectorType thresholdVector1( 2 );
    ThresholdVectorType thresholdVector2( 2 );

    /** If there is a threshold given for image1, use it. */
    if( this->m_T1 != 0 )
    {
      thresholder1 = ThresholdFilterType::New();
      thresholdVector1[ 0 ] = this->m_T1;
      thresholdVector1[ 1 ] = itk::NumericTraits<PixelType>::max();
      thresholder1->SetThresholds( thresholdVector1 );
      thresholder1->SetInput( reader1->GetOutput() );
      thresholder1->SetCoordinateTolerance( this->m_Tolerance );
      thresholder1->SetDirectionTolerance( this->m_Tolerance );
      im1 = thresholder1->GetOutput();
    }
    /** Otherwise, just take the input image1. */
    else
    {
      im1 = reader1->GetOutput();
    }

    /** If there is a threshold given for image2, use it. */
    if( this->m_T2 != 0 )
    {
      thresholder2 = ThresholdFilterType::New();
      thresholdVector2[ 0 ] = this->m_T2;
      thresholdVector2[ 1 ] = itk::NumericTraits<PixelType>::max();
      thresholder2->SetThresholds( thresholdVector2 );
      thresholder2->SetInput( reader2->GetOutput() );
      thresholder2->SetCoordinateTolerance( this->m_Tolerance );
      thresholder2->SetDirectionTolerance( this->m_Tolerance );
      im2 = thresholder2->GetOutput();
    }
    else
    {
      im2 = reader2->GetOutput();
    }

    /** Create readers for the masks and AND filters. */
    ImageReaderPointer maskReader1 = 0;
    ImageReaderPointer maskReader2 = 0;
    AndFilterPointer im2ANDmask1Filter = 0;
    AndFilterPointer im1ANDmask2Filter = 0;

    /** If there is a mask given for image1, use it on image2. */
    if( this->m_MaskFileName1 != "" )
    {
      maskReader1 = ImageReaderType::New();
      maskReader1->SetFileName( this->m_MaskFileName1.c_str() );
      im2ANDmask1Filter = AndFilterType::New();
      im2ANDmask1Filter->SetInput1( im2 );
      im2ANDmask1Filter->SetInput2( maskReader1->GetOutput() );
      im2ANDmask1Filter->SetCoordinateTolerance( this->m_Tolerance );
      im2ANDmask1Filter->SetDirectionTolerance( this->m_Tolerance );
      finalANDFilter->SetInput1( im2ANDmask1Filter->GetOutput() );
    }
    /** Otherwise, just use image2. */
    else
    {
      finalANDFilter->SetInput1( im2 );
    }

    /** If there is a mask given for image2, use it on image1. */
    if( this->m_MaskFileName2 != "" )
    {
      maskReader2 = ImageReaderType::New();
      maskReader2->SetFileName( this->m_MaskFileName2.c_str() );
      im1ANDmask2Filter = AndFilterType::New();
      im1ANDmask2Filter->SetInput1( im1 );
      im1ANDmask2Filter->SetInput2( maskReader2->GetOutput() );
      im1ANDmask2Filter->SetCoordinateTolerance( this->m_Tolerance );
      im1ANDmask2Filter->SetDirectionTolerance( this->m_Tolerance );
      finalANDFilter->SetInput2( im1ANDmask2Filter->GetOutput() );
    }
    /** Otherwise, just use image1. */
    else
    {
      finalANDFilter->SetInput2( im1 );
    }

    /** UPDATE! */
    try
    {
      finalANDFilter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
      throw excp;
    }

    /**
    * Now calculate the L1-norm.
    */

    /** Create iterators. */
    IteratorType iteratorA( finalANDFilter->GetInput(1),
      finalANDFilter->GetInput(1)->GetLargestPossibleRegion() );
    IteratorType iteratorB( finalANDFilter->GetInput(0),
      finalANDFilter->GetInput(0)->GetLargestPossibleRegion() );
    IteratorType iteratorC( finalANDFilter->GetOutput(),
      finalANDFilter->GetOutput()->GetLargestPossibleRegion() );

    /** Determine size of first object. */
    long long sumA = 0;
    for ( iteratorA.GoToBegin(); !iteratorA.IsAtEnd(); ++iteratorA )
    {
      if( iteratorA.Value() )
      {
        ++sumA;
      }
    }
    std::cout << "Size of first object: " << sumA << std::endl;

    /** Determine size of second object. */
    long long sumB = 0;
    for ( iteratorB.GoToBegin(); !iteratorB.IsAtEnd(); ++iteratorB )
    {
      if( iteratorB.Value() )
      {
        ++sumB;
      }
    }
    std::cout << "Size of second object: " << sumB << std::endl;

    /** Determine size of cross-section. */
    long long sumC = 0;
    for ( iteratorC.GoToBegin(); !iteratorC.IsAtEnd(); ++iteratorC )
    {
      if( iteratorC.Value() )
      {
        ++sumC;
      }
    }
    std::cout << "Size of cross-section of both objects: " << sumC << std::endl;

    /** Calculate the overlap. */
    double overlap;
    if( ( sumA + sumB ) == 0 )
    {
      overlap = 0;
    }
    else
    {
      overlap = static_cast<double>( 2 * sumC ) / static_cast<double>( sumA + sumB );
    }

    /** Format the output and show overlap. */
    std::cout << std::fixed << std::showpoint;
    std::cout << "Overlap: " << overlap << std::endl;

  } // end Run()

}; // end ITKToolsComputeOverlapOld

#endif // end #ifndef __ComputeOverlapOld_h_
