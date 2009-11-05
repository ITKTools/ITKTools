#ifndef __itkParabolicOpenCloseImageFilter_txx
#define __itkParabolicOpenCloseImageFilter_txx

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "itkParabolicOpenCloseImageFilter.h"
//#define NOINDEX
#ifndef NOINDEX
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#else
#include "itkImageLinearIterator.h"
#include "itkImageLinearConstIterator.h"
#endif
#include "itkStatisticsImageFilter.h"
#include "itkParabolicMorphUtils.h"

namespace itk
{

template <typename TInputImage, bool doOpen,  typename TOutputImage>
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::ParabolicOpenCloseImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfRequiredInputs( 1 );
// needs to be selected according to erosion/dilation
  if (doOpen)
    {
    // erosion then dilation
    m_Extreme1 = NumericTraits<PixelType>::max();
    m_Extreme2 = NumericTraits<PixelType>::min();
    m_MagnitudeSign1 = -1;
    m_MagnitudeSign2 = 1;
    } 
  else
    {
    // dilation then erosion
    m_Extreme1 = NumericTraits<PixelType>::min();
    m_Extreme2 = NumericTraits<PixelType>::max();
    m_MagnitudeSign1 = 1;
    m_MagnitudeSign2 = -1;
    }
  m_Extreme = m_Extreme1;
  m_MagnitudeSign = m_MagnitudeSign1;
  m_UseImageSpacing = false;
  m_Stage=1;  // indicate whether we are on the first pass or the second
}

template <typename TInputImage, bool doOpen, typename TOutputImage>
int
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::SplitRequestedRegion(int i, int num, OutputImageRegionType& splitRegion)
{
  // Get the output pointer
  OutputImageType * outputPtr = this->GetOutput();
  const typename TOutputImage::SizeType& requestedRegionSize 
    = outputPtr->GetRequestedRegion().GetSize();

  int splitAxis;
  typename TOutputImage::IndexType splitIndex;
  typename TOutputImage::SizeType splitSize;

  // Initialize the splitRegion to the output requested region
  splitRegion = outputPtr->GetRequestedRegion();
  splitIndex = splitRegion.GetIndex();
  splitSize = splitRegion.GetSize();

  // split on the outermost dimension available
  // and avoid the current dimension
  splitAxis = outputPtr->GetImageDimension() - 1;
  while (requestedRegionSize[splitAxis] == 1 || splitAxis == (int)m_CurrentDimension)
    {
    --splitAxis;
    if (splitAxis < 0)
      { // cannot split
      itkDebugMacro("  Cannot Split");
      return 1;
      }
    }

  // determine the actual number of pieces that will be generated
  typename TOutputImage::SizeType::SizeValueType range = requestedRegionSize[splitAxis];
  int valuesPerThread = (int)::ceil(range/(double)num);
  int maxThreadIdUsed = (int)::ceil(range/(double)valuesPerThread) - 1;

  // Split the region
  if (i < maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
    }
  if (i == maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    // last thread needs to process the "rest" dimension being split
    splitSize[splitAxis] = splitSize[splitAxis] - i*valuesPerThread;
    }
  
  // set the split region ivars
  splitRegion.SetIndex( splitIndex );
  splitRegion.SetSize( splitSize );

  itkDebugMacro("  Split Piece: " << splitRegion );

  return maxThreadIdUsed + 1;
}



template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::SetScale( ScalarRealType scale )
{
  RadiusType s;
  s.Fill(scale);
  this->SetScale( s );
}

#if 0
template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  InputImagePointer image = const_cast<InputImageType *>( this->GetInput() );
  if( image )
    {
    image->SetRequestedRegion( this->GetInput()->GetLargestPossibleRegion() );
    }
}
#endif

template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage,doOpen, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if (out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}


template <typename TInputImage, bool doOpen,  typename TOutputImage >
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage >
::GenerateData(void)
{

  typedef ImageLinearConstIteratorWithIndex< TInputImage  >  InputConstIteratorType;
  typedef ImageLinearIteratorWithIndex< TOutputImage >  OutputIteratorType;

  // for stages after the first
  typedef ImageLinearConstIteratorWithIndex< TOutputImage  >  OutputConstIteratorType;


  typedef ImageRegion< TInputImage::ImageDimension > RegionType;

  typename TInputImage::ConstPointer   inputImage(    this->GetInput ()   );
  typename TOutputImage::Pointer       outputImage(   this->GetOutput()   );


  //const unsigned int imageDimension = inputImage->GetImageDimension();

  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();

  // Set up the multithreaded processing
  typename ImageSource< TOutputImage >::ThreadStruct str;
  str.Filter = this;
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallback, &str);

  // multithread the execution - stage 1
  m_Stage=1;
  for( unsigned int d=0; d<ImageDimension; d++ )
    {
    m_CurrentDimension = d;
    this->GetMultiThreader()->SingleMethodExecute();
    }
  // swap over the parameters controlling erosion/dilation
  m_Extreme = m_Extreme2;
  m_MagnitudeSign = m_MagnitudeSign2;
  
  // multithread the execution - stage 2
  m_Stage=2;
  for( unsigned int d=0; d<ImageDimension; d++ )
    {
    m_CurrentDimension = d;
    this->GetMultiThreader()->SingleMethodExecute();
    }
  // swap them back
  m_Extreme = m_Extreme1;
  m_MagnitudeSign = m_MagnitudeSign1;
  m_Stage=1;
  
}

////////////////////////////////////////////////////////////

template <typename TInputImage, bool doOpen,  typename TOutputImage >
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage >
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId)
{
  // compute the number of rows first, so we can setup a progress reporter
  typename std::vector< unsigned int > NumberOfRows;
  InputSizeType   size   = outputRegionForThread.GetSize();

  for (unsigned int i = 0; i < InputImageDimension; i++)
    {
    NumberOfRows.push_back( 1 );
    for (unsigned int d = 0; d < InputImageDimension; d++)
      {
      if( d != i )
        {
        NumberOfRows[i] *= size[ d ];
        }
      }
    }
  float progressPerDimension = 1.0/ImageDimension;

  ProgressReporter * progress = new ProgressReporter(this, threadId, NumberOfRows[m_CurrentDimension], 30, m_CurrentDimension * progressPerDimension, progressPerDimension);


  typedef ImageLinearConstIteratorWithIndex< TInputImage  >  InputConstIteratorType;
  typedef ImageLinearIteratorWithIndex< TOutputImage >  OutputIteratorType;

  // for stages after the first
  typedef ImageLinearConstIteratorWithIndex< TOutputImage  >  OutputConstIteratorType;


  typedef ImageRegion< TInputImage::ImageDimension > RegionType;

  typename TInputImage::ConstPointer   inputImage(    this->GetInput ()   );
  typename TOutputImage::Pointer       outputImage(   this->GetOutput()   );


  //const unsigned int imageDimension = inputImage->GetImageDimension();

  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  RegionType region = outputRegionForThread;


  InputConstIteratorType  inputIterator(  inputImage,  region );
  OutputIteratorType      outputIterator( outputImage, region );
  OutputConstIteratorType inputIteratorStage2( outputImage, region );

  if (m_Stage == 1)
    {
    // deal with the first dimension - this should be copied to the
    // output if the scale is 0
    if (m_CurrentDimension == 0)
      {
      if (m_Scale[0] > 0)
	{
	// Perform as normal
//     RealType magnitude = 1.0/(2.0 * m_Scale[0]);
	unsigned long LineLength = region.GetSize()[0];
	RealType image_scale = this->GetInput()->GetSpacing()[0];
	
	doOneDimension<InputConstIteratorType,OutputIteratorType, 
	  RealType, OutputPixelType, !doOpen>(inputIterator, outputIterator, 
					      *progress, LineLength, 0, 
					      this->m_MagnitudeSign, 
					      this->m_UseImageSpacing,
					      this->m_Extreme,
					      image_scale, 
					      this->m_Scale[0]);
	}
      else 
	{
	// copy to output
	typedef ImageRegionConstIterator<TInputImage> InItType;
	typedef ImageRegionIterator<TOutputImage> OutItType;
	
	InItType InIt(inputImage, region);
	OutItType OutIt(outputImage, region);
	while (!InIt.IsAtEnd())
	  {
	  OutIt.Set(static_cast<OutputPixelType>(InIt.Get()));
	  ++InIt;
	  ++OutIt;
	  }
	
	}
      }
    else
      {
      // now deal with the other dimensions for first stage
      unsigned long LineLength = region.GetSize()[m_CurrentDimension];
      RealType image_scale = this->GetInput()->GetSpacing()[m_CurrentDimension];
      
      doOneDimension<OutputConstIteratorType,OutputIteratorType, 
	RealType, OutputPixelType, !doOpen>(inputIteratorStage2, outputIterator, 
					    *progress, LineLength, m_CurrentDimension, 
					    this->m_MagnitudeSign, 
					    this->m_UseImageSpacing,
					    this->m_Extreme,
					    image_scale, 
					    this->m_Scale[m_CurrentDimension]);
      
      }
    }
  else
    {
    // deal with the other dimensions for second stage
    if (m_Scale[m_CurrentDimension] > 0)
      {
      //RealType magnitude = 1.0/(2.0 * m_Scale[dd]);
      unsigned long LineLength = region.GetSize()[m_CurrentDimension];
      RealType image_scale = this->GetInput()->GetSpacing()[m_CurrentDimension];
      
      doOneDimension<OutputConstIteratorType,OutputIteratorType,
 	RealType, OutputPixelType, doOpen>(inputIteratorStage2, outputIterator, 
					   *progress, LineLength, m_CurrentDimension, 
					   this->m_MagnitudeSign, 
					   this->m_UseImageSpacing,
					   this->m_Extreme,
					   image_scale, 
					   this->m_Scale[m_CurrentDimension]);
      }
    }
}


template <typename TInputImage, bool doOpen,  typename TOutputImage>
void
ParabolicOpenCloseImageFilter<TInputImage, doOpen, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  if (m_UseImageSpacing)
    {
    os << "Scale in world units: " << m_Scale << std::endl;
    }
  else
    {
    os << "Scale in voxels: " << m_Scale << std::endl;
    }
}


} // namespace itk
#endif
