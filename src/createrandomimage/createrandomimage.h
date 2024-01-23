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
#ifndef __createrandomimage_h_
#define __createrandomimage_h_

#include "ITKToolsBase.h"

#include "itkImageFileWriter.h"
#include "itkArray.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageRandomIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
//#include "itkDiscreteGaussianImageFilter.h"
//#include "itkRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkComposeImageFilter.h"
#include "itkExceptionObject.h"
#include "itkNumericTraits.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include <iostream>
#include <string>
#include <math.h>
#include <map>


/** \class ITKToolsCreateRandomImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCreateRandomImageBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCreateRandomImageBase()
  {
    this->m_OutputFileName = "";
    this->m_Min_value = 0.0f;
    this->m_Max_value = 0.0f;
    this->m_Resolution = 0;
    this->m_Sigma = 0.0f;
    this->m_Rand_seed = 0;
    this->m_SpaceDimension = 0;
  };
  /** Destructor. */
  ~ITKToolsCreateRandomImageBase(){};

  /** Input member parameters. */
  std::string m_OutputFileName;
  itk::Array<unsigned int> m_Sizes;
  double m_Min_value;
  double m_Max_value;
  unsigned long m_Resolution;
  double m_Sigma;
  int m_Rand_seed;
  unsigned int m_SpaceDimension;

}; // end class ITKToolsCreateRandomImageBase


/** \class ITKToolsCreateBox
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCreateRandomImage : public ITKToolsCreateRandomImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCreateRandomImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCreateRandomImage(){};
  ~ITKToolsCreateRandomImage(){};

  /** Run function. */
  void Run( void )
  {
    /** PixelTypes */
    typedef float                                           InternalValueType;

    /** Typedef's. */
    typedef itk::Image< TComponentType, VDimension >        ImageType;
    typedef itk::Image< InternalValueType, VDimension >     InternalImageType;
    typedef itk::Image< TComponentType, VDimension >        ScalarOutputImageType;
    typedef itk::VectorImage< TComponentType, VDimension >  VectorOutputImageType;
    typedef typename ImageType::Pointer                     ImagePointer;
    typedef typename InternalImageType::Pointer             InternalImagePointer;

    typedef typename ImageType::SizeType      SizeType;
    typedef typename ImageType::IndexType     IndexType;
    typedef typename ImageType::PointType     OriginType;
    typedef typename ImageType::RegionType    RegionType;

    typedef typename InternalImageType::SizeType    InternalSizeType;
    typedef typename InternalImageType::IndexType   InternalIndexType;
    typedef typename InternalImageType::PointType   InternalOriginType;
    typedef typename InternalImageType::RegionType  InternalRegionType;
    typedef std::vector< InternalImagePointer >     SetOfChannelsType;

    typedef itk::ImageRandomIteratorWithIndex< InternalImageType >  RandomIteratorType;
    typedef itk::ImageRegionIterator< InternalImageType >           RegionIteratorType;

    /** Blurring filter */
    //typedef itk::DiscreteGaussianImageFilter<
    // InternalImageType, InternalImageType>              BlurFilterType;
    typedef itk::SmoothingRecursiveGaussianImageFilter<
      InternalImageType, InternalImageType>               BlurFilterType;
    typedef typename BlurFilterType::Pointer              BlurFilterPointer;

    typedef std::vector<BlurFilterPointer>                SetOfBlurrersType;

    typedef itk::CastImageFilter<InternalImageType, ImageType> CastFilterType;
    typedef typename CastFilterType::Pointer CastFilterPointer;
    typedef std::vector<CastFilterPointer> SetOfCastersType;

    typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractFilterType;
    typedef typename ExtractFilterType::Pointer ExtractFilterPointer;
    typedef std::vector<ExtractFilterPointer> SetOfExtractersType;

    /** ImageWriters */
    typedef itk::ImageFileWriter<VectorOutputImageType> VectorWriterType;
    typedef typename VectorWriterType::Pointer VectorWriterPointer;

    /** RandomGenerator */
    typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;
    typedef RandomGeneratorType::Pointer RandomGeneratorPointer;

    /** Create variables */
    VectorWriterPointer vectorWriter = nullptr;

    SetOfChannelsType setOfChannels( this->m_SpaceDimension );
    SetOfBlurrersType setOfBlurrers( this->m_SpaceDimension );
    SetOfCastersType setOfCasters( this->m_SpaceDimension );
    SetOfExtractersType setOfExtracters( this->m_SpaceDimension );

    RandomGeneratorPointer randomGenerator = RandomGeneratorType::GetInstance();
    bool randomiterating = true;

    /** Set the random seed */
    randomGenerator->SetSeed( this->m_Rand_seed );

    /** Convert the itkArray to an itkSizeType and calculate nrOfPixels */
    InternalSizeType internalimagesize;
    InternalIndexType internalimageindex;
    InternalOriginType internalimageorigin;
    InternalRegionType internalimageregion;
    SizeType imagesize;
    IndexType imageindex;
    OriginType imageorigin;
    RegionType imageregion;
    unsigned long nrOfPixels = 1;
    for( unsigned int i = 0; i < VDimension; i++ )
    {
      internalimagesize.SetElement( i, this->m_Sizes[ i ] ); //will be changed later
      imagesize.SetElement( i, this->m_Sizes[ i ] );
      imageindex.SetElement( i, 0 );
      imageorigin.SetElement( i, 0.0 );
      nrOfPixels *= this->m_Sizes[ i ];
    }

    /** Compute the standard deviation of the Gaussian used for blurring
     * the random images. */
    if( this->m_Sigma < 0 )
    {
      this->m_Sigma = static_cast<double>(
        static_cast<double>( nrOfPixels ) /
        static_cast<double>( this->m_Resolution ) /
        pow( 2.0, static_cast<double>( VDimension ) )
        );
    }

    int paddingSize = static_cast<int>( 2.0 * this->m_Sigma );

    for( unsigned int i = 0; i < VDimension; i++ )
    {
      internalimagesize[ i ] += static_cast<unsigned long>( 2 * paddingSize );
      internalimageindex[ i ] = static_cast<long>( -paddingSize );
      internalimageorigin[ i ] = 0;// static_cast<double> ( -paddingSize );
    }

    internalimageregion.SetSize( internalimagesize );
    internalimageregion.SetIndex( internalimageindex );

    imageregion.SetSize( imagesize );
    imageregion.SetIndex( imageindex );

    /** Check whether a random iterator should be used or a region */
    if( this->m_Resolution == 0 )
    {
      randomiterating = false;
    }

    /** Create the images */
    for( unsigned int i = 0; i < this->m_SpaceDimension; i++ )
    {
      setOfChannels[ i ] = InternalImageType::New();
      setOfChannels[ i ]->SetRegions( internalimageregion );
      setOfChannels[ i ]->SetOrigin( internalimageorigin );
      setOfChannels[ i ]->SetRequestedRegion( imageregion );
      setOfChannels[ i ]->Allocate();
      setOfChannels[ i ]->FillBuffer( itk::NumericTraits<InternalValueType>::Zero );

      /** Setting random values to random points */
      if( randomiterating )
      {
        std::cout << "Channel" << i
          << ": Setting random values to " << this->m_Resolution << " random points."
          << std::endl;
        RandomIteratorType iterator
          = RandomIteratorType( setOfChannels[ i ], setOfChannels[ i ]->GetLargestPossibleRegion() );
        iterator.SetNumberOfSamples( this->m_Resolution );
        iterator.GoToBegin();

        while( !iterator.IsAtEnd() )
        {
          /** Set a random value to a random point */
          iterator.Set( static_cast<InternalValueType>(
            randomGenerator->GetUniformVariate( this->m_Min_value, this->m_Max_value ) ) );
          ++iterator;
        }
      }
      else
      {
        std::cout << "Channel" << i
          << ": Setting random values to all voxels in the image."
          << std::endl;
        RegionIteratorType iterator
          = RegionIteratorType( setOfChannels[ i ], setOfChannels[ i ]->GetLargestPossibleRegion() );
        iterator.GoToBegin();
        while( !iterator.IsAtEnd() )
        {
          /** Set a random value to a point */
          iterator.Set( static_cast<InternalValueType>(
            randomGenerator->GetUniformVariate( this->m_Min_value, this->m_Max_value ) ) );
          ++iterator;
        }
      }

      /** The random image is blurred */
      std::cout << "Channel" << i
        << ": Blurring with standard deviation " << this->m_Sigma
        << "." << std::endl;
      setOfBlurrers[ i ] = BlurFilterType::New();
      setOfBlurrers[ i ]->SetSigma( this->m_Sigma );

      //setOfBlurrers[ i ]->SetVariance( sigma*sigma );
      //setOfBlurrers[ i ]->SetUseImageSpacingOff();
      //setOfBlurrers[ i ]->SetMaximumError(0.01);
      //setOfBlurrers[ i ]->SetMaximumKernelWidth(maximumKernelWidth);
      setOfBlurrers[ i ]->SetInput( setOfChannels[ i ] );

      setOfCasters[ i ] = CastFilterType::New();
      setOfCasters[ i ]->SetInput( setOfBlurrers[ i ]->GetOutput() );
      //setOfCasters[ i ]->UpdateLargestPossibleRegion();

      setOfExtracters[ i ] = ExtractFilterType::New();
      setOfExtracters[ i ]->SetInput( setOfCasters[ i ]->GetOutput() );
      setOfExtracters[ i ]->SetExtractionRegion(imageregion);

      // suppress warnings about exceeding the maximum kernel width,
      // generated by the blurrers.
      //  itk::Object::GlobalWarningDisplayOff();
      setOfExtracters[ i ]->Update();
      //  itk::Object::GlobalWarningDisplayOn();
    }

    typedef itk::ComposeImageFilter<ScalarOutputImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    for( unsigned int spaceDimensionIndex = 0; spaceDimensionIndex < this->m_SpaceDimension; ++spaceDimensionIndex )
    {
      imageToVectorImageFilter->SetInput( spaceDimensionIndex, setOfExtracters[spaceDimensionIndex]->GetOutput() );
    }
    imageToVectorImageFilter->Update();

    std::cout << "Saving image to disk as \""
      << this->m_OutputFileName << "\""
      << std::endl;

    vectorWriter = VectorWriterType::New();
    vectorWriter->SetInput(imageToVectorImageFilter->GetOutput());
    vectorWriter->Update();

  } // end Run()

}; // end class ITKToolsCreateRandomImage


#endif // #ifndef __createrandomimage_h_
