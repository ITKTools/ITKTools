#ifndef __itkParabolicErodeDilateImageFilter_txx
#define __itkParabolicErodeDilateImageFilter_txx

#include "itkParabolicErodeDilateImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

//#define NOINDEX
#ifndef NOINDEX
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#else
#include "itkImageLinearIterator.h"
#include "itkImageLinearConstIterator.h"
#endif
#include "itkParabolicMorphUtils.h"

namespace itk
{

template <typename TInputImage, bool doDilate, typename TOutputImage>
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
::ParabolicErodeDilateImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfRequiredInputs( 1 );
// needs to be selected according to erosion/dilation

  if(doDilate)
    {
    this->m_Extreme = NumericTraits<PixelType>::min();
    this->m_MagnitudeSign = 1;
    }
  else
    {
    this->m_Extreme = NumericTraits<PixelType>::max();
    this->m_MagnitudeSign = -1;
    }
  this->m_UseImageSpacing = false;
}

template <typename TInputImage, bool doDilate, typename TOutputImage>
int
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
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
    if(splitAxis < 0)
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
  if(i < maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
    }
  if(i == maxThreadIdUsed)
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


template <typename TInputImage, bool doDilate, typename TOutputImage>
void
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
::SetScale( ScalarRealType scale )
{
  RadiusType s;
  s.Fill(scale);
  this->SetScale( s );
}

#if 0
template <typename TInputImage, bool doDilate, typename TOutputImage>
void
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
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
#if 1
template <typename TInputImage, bool doDilate, typename TOutputImage>
void
ParabolicErodeDilateImageFilter<TInputImage,doDilate, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  TOutputImage *out = dynamic_cast<TOutputImage*>(output);

  if(out)
    {
    out->SetRequestedRegion( out->GetLargestPossibleRegion() );
    }
}
#endif

template <typename TInputImage, bool doDilate, typename TOutputImage >
void
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage >
::GenerateData( void )
{

  typename TInputImage::ConstPointer   inputImage(    this->GetInput ()   );
  typename TOutputImage::Pointer       outputImage(   this->GetOutput()        );

  //const unsigned int imageDimension = inputImage->GetImageDimension();
  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  // Set up the multithreaded processing
  typename ImageSource< TOutputImage >::ThreadStruct str;
  str.Filter = this;
  auto& multiThreader = *(this->GetMultiThreader());
  multiThreader.SetMaximumNumberOfThreads(multiThreader.GetNumberOfWorkUnits());
  multiThreader.SetNumberOfWorkUnits(multiThreader.GetMaximumNumberOfThreads());
  multiThreader.SetSingleMethod(this->ThreaderCallback, &str);

  // multithread the execution
  for( unsigned int d=0; d<ImageDimension; d++ )
    {
    this->m_CurrentDimension = d;
    this->GetMultiThreader()->SingleMethodExecute();
    }

}

template <typename TInputImage, bool doDilate, typename TOutputImage>
void
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId )
{
  // compute the number of rows first, so we can setup a progress reporter
  typename std::vector< unsigned int > NumberOfRows;
  InputSizeType   size   = outputRegionForThread.GetSize();

  for( unsigned int i = 0; i < InputImageDimension; i++ )
    {
    NumberOfRows.push_back( 1 );
    for( unsigned int d = 0; d < InputImageDimension; d++)
      {
      if( d != i )
        {
        NumberOfRows[ i ] *= size[ d ];
        }
      }
    }
  float progressPerDimension = 1.0/ImageDimension;

  ProgressReporter * progress = new ProgressReporter(this, threadId, NumberOfRows[m_CurrentDimension], 30, this->m_CurrentDimension * progressPerDimension, progressPerDimension);


  typedef ImageLinearConstIteratorWithIndex< TInputImage  >  InputConstIteratorType;
  typedef ImageLinearIteratorWithIndex< TOutputImage >  OutputIteratorType;

  // for stages after the first
  typedef ImageLinearConstIteratorWithIndex< TOutputImage  >  OutputConstIteratorType;


  typedef ImageRegion< TInputImage::ImageDimension > RegionType;

  typename TInputImage::ConstPointer   inputImage(    this->GetInput ()   );
  typename TOutputImage::Pointer       outputImage(   this->GetOutput()        );


  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  RegionType region = outputRegionForThread;

  InputConstIteratorType  inputIterator(  inputImage,  region );
  OutputIteratorType      outputIterator( outputImage, region );
  OutputConstIteratorType inputIteratorStage2( outputImage, region );

  // setup the progress reporting
//   unsigned int numberOfLinesToProcess = 0;
//   for (unsigned  dd = 0; dd < imageDimension; dd++)
//     {
//     numberOfLinesToProcess += region.GetSize()[dd];
//     }

//   ProgressReporter progress(this,0, numberOfLinesToProcess);

  // deal with the first dimension - this should be copied to the
  // output if the scale is 0
  if( this->m_CurrentDimension == 0)
    {
    if( this->m_Scale[0] > 0)
      {
      // Perform as normal
      //RealType magnitude = 1.0/(2.0 * this->m_Scale[0]);
      unsigned long LineLength = region.GetSize()[0];
      RealType image_scale = this->GetInput()->GetSpacing()[0];

      doOneDimension<InputConstIteratorType,OutputIteratorType,
  RealType, OutputPixelType, doDilate>(inputIterator, outputIterator,
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
      // other dimensions
      if( this->m_Scale[m_CurrentDimension] > 0)
  {
  // create a vector to buffer lines
  unsigned long LineLength = region.GetSize()[m_CurrentDimension];
  //RealType magnitude = 1.0/(2.0 * this->m_Scale[dd]);
      RealType image_scale = this->GetInput()->GetSpacing()[m_CurrentDimension];

      doOneDimension<OutputConstIteratorType,OutputIteratorType,
  RealType, OutputPixelType, doDilate>(inputIteratorStage2, outputIterator,
               *progress, LineLength, this->m_CurrentDimension,
               this->m_MagnitudeSign,
               this->m_UseImageSpacing,
               this->m_Extreme,
               image_scale,
               this->m_Scale[m_CurrentDimension]);
  }
    }
}


template <typename TInputImage, bool doDilate, typename TOutputImage>
void
ParabolicErodeDilateImageFilter<TInputImage, doDilate, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  if( this->m_UseImageSpacing)
    {
    os << "Scale in world units: " << this->m_Scale << std::endl;
    }
  else
    {
    os << "Scale in voxels: " << this->m_Scale << std::endl;
    }
}


} // namespace itk
#endif
