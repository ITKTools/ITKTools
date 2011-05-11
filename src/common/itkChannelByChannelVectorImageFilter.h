/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkChannelByChannelVectorImageFilter_h
#define __itkChannelByChannelVectorImageFilter_h

#include "itkVectorIndexSelectionCastImageFilter.h" // decompose
#include "itkImageToVectorImageFilter.h" // reassemble
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkImageToImageFilter.h"

namespace itk
{
/** \class ChannelByChannelVectorImageFilter
 *  \brief This filter applies, independently per channel, a itk::ImageToImageFilter to an itkVectorImage.
 *  
 *  The user can specify the inputs to this filter in two ways. First, they can specify a single filter to be used
 *  on every channel of the image. Second, they can specify a different filter (of the same class, but with different
 *  parameters) for each channel of the image. Filters with multiple inputs are allowed.
 */
template <class TInputImage, class TOutputImage = TInputImage>
class ITK_EXPORT ChannelByChannelVectorImageFilter
  : public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard typedefs */
  typedef ChannelByChannelVectorImageFilter                  Self;
  typedef itk::ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ChannelByChannelVectorImageFilter, ImageToImageFilter);

  /** Template parameters typedefs */
  typedef TInputImage                                                 InputVectorImageType;
  typedef typename InputVectorImageType::Pointer                      InputVectorImagePointerType;
  typedef typename InputVectorImageType::InternalPixelType            InputPixelType;
  typedef Image<InputPixelType, InputVectorImageType::ImageDimension> InputScalarImageType;

  typedef TOutputImage                                                  OutputVectorImageType;
  typedef typename OutputVectorImageType::Pointer                       OutputVectorImagePointerType;
  typedef typename OutputVectorImageType::InternalPixelType             OutputPixelType;
  typedef Image<OutputPixelType, OutputVectorImageType::ImageDimension> OutputImageType;
  typedef Image<OutputPixelType, OutputVectorImageType::ImageDimension>	OutputScalarImageType;

  typedef itk::ImageToImageFilter<InputScalarImageType, OutputScalarImageType> FilterType;
  typedef typename FilterType::Pointer FilterPointerType;

  void SetAllFilters(FilterPointerType filter);
  void SetFilterForSingleChannel(unsigned int channel, FilterPointerType filter);

protected:
  /** Main computation method */
  virtual void GenerateData(void);
  /** Constructor */
  ChannelByChannelVectorImageFilter();
  /** Destructor */
  virtual ~ChannelByChannelVectorImageFilter() {}
  /**PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /**If the user chooses to specify a filter for each channel separately, they are stored here.*/
  std::vector<FilterPointerType> m_Filters;
  
  /**If the user chooses to only specify a single filter to be used for all channels, it is stored here.*/
  FilterPointerType m_SingleFilter;

private:
  ChannelByChannelVectorImageFilter(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

};
} // End namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkChannelByChannelVectorImageFilter.txx"
#endif

#endif
