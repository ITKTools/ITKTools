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
 *  \brief This filter is a helper class to apply per channel a standard itk::ImageToImageFilter to a VectorImage.
 */
template <class TInputImage, class TFilter, class TOutputImage = TInputImage>
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

  typedef TFilter                      FilterType;
  typedef typename FilterType::Pointer FilterPointerType;

protected:
  /** Main computation method */
  virtual void GenerateData(void);
  /** Constructor */
  ChannelByChannelVectorImageFilter();
  /** Destructor */
  virtual ~ChannelByChannelVectorImageFilter() {}
  /**PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  ChannelByChannelVectorImageFilter(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

};
} // End namespace otb
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkChannelByChannelVectorImageFilter.txx"
#endif

#endif
