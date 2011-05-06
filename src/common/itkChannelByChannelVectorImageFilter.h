/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
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
