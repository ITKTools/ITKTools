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
#ifndef __itkSplitSegmentationImageFilter_h_
#define __itkSplitSegmentationImageFilter_h_

#include "itkImageToImageFilter.h"


namespace itk
{

/** \class SplitSegmentationImageFilter
 * \brief Computes the
 *
 * \ingroup IntensityImageFilters
 * \ingroup Singlethreaded
 */

template < typename TInputImage,typename TOutputImage = TInputImage >
class ITK_EXPORT SplitSegmentationImageFilter:
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  /** Standard class typedefs. */
  typedef SplitSegmentationImageFilter                      Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( SplitSegmentationImageFilter, ImageToImageFilter );

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Pixel Type of the input image */
  typedef TInputImage                                       InputImageType;
  typedef typename InputImageType::Pointer                  InputImagePointer;
  typedef typename InputImageType::ConstPointer             InputImageConstPointer;
  typedef typename InputImageType::PixelType                InputPixelType;
  typedef TOutputImage                                      OutputImageType;
  typedef typename OutputImageType::Pointer                 OutputImagePointer;
  typedef typename OutputImageType::PixelType               OutputPixelType;
  typedef typename InputImageType::RegionType               RegionType;
  typedef typename InputImageType::SizeType                 SizeType;
  typedef typename InputImageType::IndexType                IndexType;

  typedef std::vector<OutputPixelType>                      LabelType;

  /** Set the number of splits. */
  itkSetMacro( NumberOfSplitsZ, unsigned int );
  itkSetMacro( NumberOfSplitsY, unsigned int );

  /** Set the output labels. */
  void SetChunkLabels( const LabelType & labels );

protected:
  SplitSegmentationImageFilter();
  virtual ~SplitSegmentationImageFilter() {};

  void PrintSelf( std::ostream & os, Indent indent ) const;

  /** Generate Data */
  virtual void GenerateData( void );

private:
  SplitSegmentationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Member variables. */
  unsigned int m_NumberOfSplitsZ;
  unsigned int m_NumberOfSplitsY;
  LabelType    m_ChunkLabels;

}; // end class SplitSegmentationImageFilter

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSplitSegmentationImageFilter.hxx"
#endif

#endif // end #ifndef __itkSplitSegmentationImageFilter_h_
