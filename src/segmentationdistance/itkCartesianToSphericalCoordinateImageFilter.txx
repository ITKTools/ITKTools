#ifndef _itkCartesianToSphericalCoordinateImageFilter_txx
#define _itkCartesianToSphericalCoordinateImageFilter_txx

#include "itkCartesianToSphericalCoordinateImageFilter.h"
#include "itkProgressReporter.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkNumericTraits.h"


namespace itk
{

/**
 * Initialize new instance
 */
template< class TInputImage, class TOutputImage >
CartesianToSphericalCoordinateImageFilter<TInputImage, TOutputImage>
::CartesianToSphericalCoordinateImageFilter()
{
  this->m_OutputSpacing.Fill(1.0);
  this->m_InputSpacing.Fill(1.0);
  this->m_OutputOrigin.Fill(0.0);
  this->m_OutputSize.Fill( 0 );
  this->m_OutputStartIndex.Fill( 0 );
  this->m_Interpolator = 0;
  this->m_MaskImage = 0;
  this->m_MaximumNumberOfSamplesPerVoxel = 5;

  this->m_RandomGenerator = RandomGeneratorType::GetInstance();

}


/**
 * Print out a description of self
 *
 * \todo Add details about this class
 */
template< class TInputImage, class TOutputImage >
void
CartesianToSphericalCoordinateImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "OutputSize: " << this->m_OutputSize << std::endl;
  os << indent << "OutputStartIndex: " << this->m_OutputStartIndex << std::endl;
  os << indent << "OutputSpacing: " << this->m_OutputSpacing << std::endl;
  os << indent << "OutputOrigin: " << this->m_OutputOrigin << std::endl;

  return;
}


/**
 * Inform pipeline of necessary input image region
 *
 * We do the easy thing and request the entire input image.
 */
template< class TInputImage, class TOutputImage >
void
CartesianToSphericalCoordinateImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if( !this->GetInput() )
  {
    return;
  }

  // get pointers to the input and output
  InputImagePointer  inputPtr  =
    const_cast< TInputImage *>( this->GetInput() );

  // Request the entire input image
  InputImageRegionType inputRegion;
  inputRegion = inputPtr->GetLargestPossibleRegion();
  inputPtr->SetLargestPossibleRegion(inputRegion);
  inputPtr->SetRequestedRegion(inputRegion);

  return;
}


/**
 * Inform pipeline of required output region
 * Computes the output spacing etc.
 */
template< class TInputImage, class TOutputImage >
void
CartesianToSphericalCoordinateImageFilter<TInputImage,TOutputImage>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  InputImageConstPointer inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput();
  if( !outputPtr )
  {
    return;
  }

  /** Check if the output size is valid. */
  if( this->m_OutputSize[0] < 2 )
  {
    itkExceptionMacro( << "Invalid OutputSize! OutputSize[0] must be greater than 1.");
  }
  if( this->m_OutputSize[1] < 2 )
  {
    itkExceptionMacro( << "Invalid OutputSize! OutputSize[1] must be greater than 1.");
  }
  if( this->m_OutputSize[2] < 2 )
  {
    itkExceptionMacro( << "Invalid OutputSize! OutputSize[2] must be greater than 1.");
  }

  /** Set the output origin to zero. Later we may extend this to
   * only generate a part of the r,theta,phi space */
  this->m_OutputOrigin.Fill(0.0);
  outputPtr->SetOrigin( this->m_OutputOrigin );

  /** Set output size and index; size was given by the user */
  this->m_OutputStartIndex.Fill(0);
  typename TOutputImage::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( this->m_OutputSize );
  outputLargestPossibleRegion.SetIndex( this->m_OutputStartIndex );
  outputPtr->SetLargestPossibleRegion( outputLargestPossibleRegion );

  /** Compute spacing theta */
  this->m_OutputSpacing[1] = 2.0 * vnl_math::pi / this->m_OutputSize[1];
  /** Compute spacing phi */
  this->m_OutputSpacing[2] = vnl_math::pi / (this->m_OutputSize[2]-1);

  /** Compute maximum R */
  IndexType inputIndex = inputPtr->GetRequestedRegion().GetIndex();
  SizeType inputSize = inputPtr->GetRequestedRegion().GetSize();
  PointType cor = this->GetCenterOfRotation();
  double maxR = 0.0;
  IndexType cornerIndex = inputIndex;
  for( unsigned int i = 0; i < 2; ++i )
  {
    cornerIndex[0] = inputIndex[0] + i * (inputSize[0] -1);
    for( unsigned int j = 0; j < 2; ++j )
    {
      cornerIndex[1] = inputIndex[1] + j * (inputSize[1] -1);
      for( unsigned int k = 0; k < 2; ++k)
      {
        cornerIndex[2] = inputIndex[2] + k * (inputSize[2] -1 );
        PointType cornerPoint;
        inputPtr->TransformIndexToPhysicalPoint( cornerIndex, cornerPoint);
        VectorType vec = cornerPoint - cor;
        maxR = std::max( maxR, vec.GetNorm() );
      }
    }
  }

  /** Compute spacing r */
  this->m_OutputSpacing[0] = maxR / (this->m_OutputSize[0]-1);

  /** Set the spacing */
  outputPtr->SetSpacing( this->m_OutputSpacing );

  return;
}


/**
 * GenerateData
 */
template< class TInputImage, class TOutputImage >
void
CartesianToSphericalCoordinateImageFilter<TInputImage,TOutputImage>
::GenerateData( void )
{
  InputImageConstPointer inputImage = this->GetInput();
  OutputImagePointer outputImage = this->GetOutput();

  /** allocate the memory and fill with zeros */
  this->AllocateOutputs();
  outputImage->FillBuffer(0.0);

  /** The parzen kernel */
  KernelType::Pointer kernel = KernelType::New();

  /** The sumImage and the counts image. The counts image counts
   * for each output voxel how much total weight was assigned.
   * The sum image stores the cumulative weight*pixelvalue
   * So, sumImage ./ counts image is a kind of weighted average. */
  typename InternalImageType::Pointer sumImage = InternalImageType::New();
  typename InternalImageType::Pointer countsImage = InternalImageType::New();

  bool useInterpolator = false;
  if( this->m_Interpolator.IsNotNull() )
  {
    this->m_Interpolator->SetInputImage( inputImage );
    useInterpolator = true;
  }

  /** Cache the spacing, used by the random coordinate generator */
  this->m_InputSpacing = inputImage->GetSpacing();

  /** Add an extra theta, corresponding to 2pi. Later the content of
   * this column will be merged with the theta=0 column */
  OutputImageRegionType tempRegion = outputImage->GetLargestPossibleRegion();
  SizeType tempSize = tempRegion.GetSize();
  tempSize[1] +=1;
  /** Also increase the other dimensions, to be sure that no problems
   * occurs when r=rmax or phi=phimax
   * In principal these last rows will be empty  */
  tempSize[0]+=1;
  tempSize[2]+=1;
  tempRegion.SetSize( tempSize);

  sumImage->SetRegions( tempRegion );
  countsImage->SetRegions( tempRegion );
  sumImage->SetOrigin( outputImage->GetOrigin() );
  countsImage->SetOrigin( outputImage->GetOrigin() );
  sumImage->SetSpacing( outputImage->GetSpacing() );
  countsImage->SetSpacing( outputImage->GetSpacing() );
  sumImage->Allocate();
  countsImage->Allocate();
  sumImage->FillBuffer(0.0);
  countsImage->FillBuffer(0.0);

  /** Compute (dVrtp') /(dVxyz'); This factor will be needed
   * for computation of the number of samples per voxel
   * dVrtp' = min(dr, dtheta, dphi)^3
   * dVxyz' = max( dx, dy, dz)^3
   * This makes sure that we will take enough samples for sure.
   */
  double dVrtp = itk::NumericTraits<double>::max();
  double dVxyz = 0.0;
  for( unsigned int i = 0; i < ImageDimension; ++i )
  {
    dVrtp = std::min( this->m_OutputSpacing[ i ], dVrtp);
    dVxyz = std::max( this->m_InputSpacing[ i ], dVxyz);
  }
  double deltaVolumeRatioFactor =
    ( dVrtp / dVxyz ) * ( dVrtp / dVxyz ) * ( dVrtp / dVxyz );

  const double invMaximumNumberOfSamplesPerVoxel =
    1.0 / static_cast<double>(this->m_MaximumNumberOfSamplesPerVoxel);

  /** Set up iterators over input image and input mask */
  typedef ImageRegionConstIteratorWithIndex< InputImageType > InputIteratorType;
  InputIteratorType inIt( inputImage, inputImage->GetRequestedRegion() );
  inIt.GoToBegin();

  typedef ImageRegionConstIteratorWithIndex< MaskImageType > MaskIteratorType;
  MaskIteratorType maskIt;
  bool useMask = false;
  if( this->m_MaskImage.IsNotNull() )
  {
    useMask = true;
    maskIt = MaskIteratorType( this->m_MaskImage, inputImage->GetRequestedRegion() );
    maskIt.GoToBegin();
  }

  PointType cor = this->GetCenterOfRotation();

  while ( !inIt.IsAtEnd() )
  {
    /** Compute vector to cor */
    bool validPixel = true;
    if(useMask)
    {
      if( maskIt.Value() == 0 )
      {
        validPixel = false;
      }
    }

    if( validPixel )
    {
      const IndexType & inIndex = inIt.GetIndex();
      double inValue = inIt.Value();
      PointType inPoint;
      inputImage->TransformIndexToPhysicalPoint(inIndex, inPoint);

      /** distance of indexpoint to cor  */
      VectorType vec0 = inPoint - cor;
      /** compute r^2 sin(phi) */
      const double r2 = vec0.GetSquaredNorm() ;
      const double sinphi = vcl_sin( std::acos( vec0[2] / std::sqrt(r2) ) );

      /** Compute the number of samples needed */
      const double deltaVolumeRatio = deltaVolumeRatioFactor * r2 * sinphi;
      unsigned int numberOfSamplesPerVoxel = 1;
      if( deltaVolumeRatio <= invMaximumNumberOfSamplesPerVoxel )
      {
        numberOfSamplesPerVoxel = this->m_MaximumNumberOfSamplesPerVoxel;
      }
      else
      {
        /** Use ceil: at least 1 sample! */
        numberOfSamplesPerVoxel = static_cast<unsigned int>(
          std::ceil( 1.0 / deltaVolumeRatio ) );
      }

      /** For the first iteration use the indexPoint. This makes sure that,
      * if only one point is used, that point is the indexPoint */
      PointType randomPoint = inPoint;

      for( unsigned int i = 0; i < numberOfSamplesPerVoxel; ++i )
      {
        /** if an interpolator is used, and if the randomPoint is a valid point
        * then use it.
        * if no interpolator is used, we simply use the voxel value itself:
        * nearest neighbor interpolatorion  */
        if( useInterpolator )
        {
          if( this->m_Interpolator->IsInsideBuffer( randomPoint ) )
          {
            inValue = this->m_Interpolator->Evaluate( randomPoint);
          }
          else
          {
            continue;
          }
        }

        /** distance of random point to cor */
        VectorType vec = randomPoint - cor;
        const double x = vec[0];
        const double y = vec[1];
        const double z = vec[2];

        /** compute r, theta and phi */
        const double r = vec.GetNorm() ;
        double theta = std::atan2( y, x);
        if( theta<0 )
        {
          theta += 2.0* vnl_math::pi;
        }
        const double phi = std::acos( z / r );

        /** Find out in which voxels in the sumImage and countImage we have to do something */
        PointType rtpPoint;
        ContinuousIndexType rtpCIndex;
        IndexType rtpIndex0;
        IndexType rtpIndex;
        ParzenWeightContainerType parzenWeight;

        rtpPoint[0] = r;
        rtpPoint[1] = theta;
        rtpPoint[2] = phi;
        sumImage->TransformPhysicalPointToContinuousIndex( rtpPoint, rtpCIndex);
        for( unsigned int i=0 ; i < ImageDimension; ++i )
        {
          rtpIndex0[ i ] = static_cast<int>( std::floor( rtpCIndex[ i ] ) );
          parzenWeight(i,0) = kernel->Evaluate(
            static_cast<double>(rtpIndex0[ i ]) - rtpCIndex[ i ] );
          parzenWeight(i,1) = kernel->Evaluate(
            static_cast<double>(rtpIndex0[ i ]+1) - rtpCIndex[ i ] );
        }

        /** Update the sumImage and countsImage */
        for( unsigned int i = 0; i < 2; ++i )
        {
          rtpIndex[0] = rtpIndex0[0] + i;
          for( unsigned int j = 0; j < 2; ++j )
          {
            rtpIndex[1] = rtpIndex0[1] + j;
            for( unsigned int k = 0; k < 2; ++k)
            {
              rtpIndex[2] = rtpIndex0[2] + k;
              const double parzenValue =
                parzenWeight(0,i)*parzenWeight(1,j)*parzenWeight(2,k);

              sumImage->GetPixel( rtpIndex ) += inValue*parzenValue;
              countsImage->GetPixel( rtpIndex ) += parzenValue;
            }
          }
        }

        /** Randomly pick a coordinate in the neighborhood of this pixel */
        this->GenerateRandomCoordinate(inPoint, randomPoint);

      } // next random coordinate

    } // end if validPixel

    /** inc image iterators */
    ++inIt;
    if( useMask )
    {
      ++maskIt;
    }

  } // next pixel

  /** Add the last theta slice to the first theta slice */
  typedef ImageSliceConstIteratorWithIndex< InternalImageType > InternalConstSliceIteratorType;
  typedef ImageSliceIteratorWithIndex< InternalImageType > InternalSliceIteratorType;

  OutputImageRegionType lastThetaSliceRegion = sumImage->GetLargestPossibleRegion();
  OutputImageRegionType firstThetaSliceRegion = sumImage->GetLargestPossibleRegion();

  SizeType lastThetaSliceSize = lastThetaSliceRegion.GetSize();
  SizeType firstThetaSliceSize = firstThetaSliceRegion.GetSize();
  IndexType lastThetaSliceIndex = lastThetaSliceRegion.GetIndex();

  lastThetaSliceIndex[1] = lastThetaSliceSize[1]-1;
  lastThetaSliceSize[1] = 1;
  firstThetaSliceSize[1] = 1;


  lastThetaSliceRegion.SetSize( lastThetaSliceSize);
  lastThetaSliceRegion.SetIndex( lastThetaSliceIndex);
  firstThetaSliceRegion.SetSize( firstThetaSliceSize);

  InternalConstSliceIteratorType sumLastSliceIterator( sumImage, lastThetaSliceRegion);
  InternalConstSliceIteratorType countsLastSliceIterator( countsImage, lastThetaSliceRegion);
  InternalSliceIteratorType sumFirstSliceIterator( sumImage, firstThetaSliceRegion);
  InternalSliceIteratorType countsFirstSliceIterator( countsImage, firstThetaSliceRegion);
  sumFirstSliceIterator.SetFirstDirection(0);
  sumFirstSliceIterator.SetSecondDirection(2);
  sumLastSliceIterator.SetFirstDirection(0);
  sumLastSliceIterator.SetSecondDirection(2);
  countsFirstSliceIterator.SetFirstDirection(0);
  countsFirstSliceIterator.SetSecondDirection(2);
  countsLastSliceIterator.SetFirstDirection(0);
  countsLastSliceIterator.SetSecondDirection(2);
  sumFirstSliceIterator.GoToBegin();
  sumLastSliceIterator.GoToBegin();
  countsFirstSliceIterator.GoToBegin();
  countsLastSliceIterator.GoToBegin();

  while ( !sumFirstSliceIterator.IsAtEnd() )
  {
    while( !sumFirstSliceIterator.IsAtEndOfSlice() )
    {
      while( !sumFirstSliceIterator.IsAtEndOfLine() )
      {
        sumFirstSliceIterator.Value() += sumLastSliceIterator.Value();
        countsFirstSliceIterator.Value() += countsLastSliceIterator.Value();
        ++sumFirstSliceIterator;
        ++sumLastSliceIterator;
        ++countsFirstSliceIterator;
        ++countsLastSliceIterator;
      }
      sumFirstSliceIterator.NextLine();
      sumLastSliceIterator.NextLine();
      countsFirstSliceIterator.NextLine();
      countsLastSliceIterator.NextLine();
    }
    sumFirstSliceIterator.NextSlice();
    sumLastSliceIterator.NextSlice();
    countsFirstSliceIterator.NextSlice();
    countsLastSliceIterator.NextSlice();
  }

  /** Compute the output = sumImage ./ countsImage */
  typedef ImageRegionConstIterator< InternalImageType > InternalConstIteratorType;
  typedef ImageRegionIterator< OutputImageType > OutputIteratorType;
  InternalConstIteratorType sumIt( sumImage, outputImage->GetLargestPossibleRegion() );
  sumIt.GoToBegin();
  InternalConstIteratorType countsIt( countsImage, outputImage->GetLargestPossibleRegion() );
  countsIt.GoToBegin();
  OutputIteratorType outIt( outputImage, outputImage->GetLargestPossibleRegion() );
  outIt.GoToBegin();

  if( NumericTraits<OutputPixelType>::is_integer )
  {
    /** use a round operation */
    while ( !outIt.IsAtEnd() )
    {
      const double counts = countsIt.Value();
      if( counts > 1e-14 )
      {
        outIt.Value() = static_cast<OutputPixelType>(
          vnl_math_rnd( sumIt.Value() / counts ) );
      }
      ++sumIt;
      ++countsIt;
      ++outIt;
    }
  }
  else
  {
    /** just cast */
    while ( !outIt.IsAtEnd() )
    {
      const double counts = countsIt.Value();
      if( counts > 1e-14 )
      {
        outIt.Value() = static_cast<OutputPixelType>( sumIt.Value() / counts );
      }
      ++sumIt;
      ++countsIt;
      ++outIt;
    }
  }


} // end GenerateData

/**
* ******************* GenerateRandomCoordinate *******************
*/

template< class TInputImage, class TOutputImage >
void
CartesianToSphericalCoordinateImageFilter<TInputImage,TOutputImage>::
GenerateRandomCoordinate(
const PointType & inputPoint,
PointType &       randomPoint)
{
  for( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    randomPoint[ i ] = static_cast<CoordRepType>(
      this->m_RandomGenerator->GetUniformVariate(
      inputPoint[ i ] - 0.5* this->m_InputSpacing[ i ],
      inputPoint[ i ] + 0.5* this->m_InputSpacing[ i ] ) );
  }
} // end GenerateRandomCoordinate



} // end namespace itk

#endif

