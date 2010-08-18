/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOtsuThresholdWithMaskImageCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-04-23 03:53:36 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOtsuThresholdWithMaskImageCalculator_h
#define __itkOtsuThresholdWithMaskImageCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkNumericTraits.h"

namespace itk
{

/** \class OtsuThresholdWithMaskImageCalculator
 * \brief Computes the Otsu's threshold for an image.
 *
 * This calculator computes the Otsu's threshold which separates an image
 * into foreground and background components. The method relies on a
 * histogram of image intensities. The basic idea is to maximize the
 * between-class variance.
 *
 * This class is templated over the input image type.
 *
 * \warning This method assumes that the input image consists of scalar pixel
 * types.
 *
 * \ingroup Operators
 */
template <class TInputImage>
class ITK_EXPORT OtsuThresholdWithMaskImageCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef OtsuThresholdWithMaskImageCalculator Self;
  typedef Object                       Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(OtsuThresholdWithMaskImageCalculator, Object);

  /** Type definition for the input image. */
  typedef TInputImage  ImageType;

  /** Pointer type for the image. */
  typedef typename TInputImage::Pointer  ImagePointer;

  /** Const Pointer type for the image. */
  typedef typename TInputImage::ConstPointer ImageConstPointer;

  /** Type definition for the input image pixel type. */
  typedef typename TInputImage::PixelType PixelType;

  /** Type definition for the input image region type. */
  typedef typename TInputImage::RegionType RegionType;

  /** Image related typedefs. */
  itkStaticConstMacro( ImageDimension, unsigned int,
    TInputImage::ImageDimension );
  typedef Image< unsigned char,
    itkGetStaticConstMacro( ImageDimension ) >      MaskImageType;
  typedef typename MaskImageType::Pointer           MaskImagePointer;
  typedef typename MaskImageType::ConstPointer      MaskImageConstPointer;

  /** Set the input image. */
  itkSetConstObjectMacro(Image,ImageType);

  /** Set the mask image */
  itkSetObjectMacro( MaskImage, MaskImageType );

  /** Compute the Otsu's threshold for the input image. */
  void Compute(void);

  /** Return the Otsu's threshold value. */
  itkGetConstMacro(Threshold,PixelType);

  /** Set/Get the number of histogram bins. Default is 128. */
  itkSetClampMacro( NumberOfHistogramBins, unsigned long, 1,
                    NumericTraits<unsigned long>::max() );
  itkGetConstMacro( NumberOfHistogramBins, unsigned long );

  /** Set the region over which the values will be computed */
  void SetRegion( const RegionType & region );

protected:
  OtsuThresholdWithMaskImageCalculator();
  virtual ~OtsuThresholdWithMaskImageCalculator() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  OtsuThresholdWithMaskImageCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PixelType             m_Threshold;
  unsigned long         m_NumberOfHistogramBins;
  ImageConstPointer     m_Image;
  MaskImagePointer      m_MaskImage;
  RegionType            m_Region;
  bool                  m_RegionSetByUser;

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOtsuThresholdWithMaskImageCalculator.txx"
#endif

#endif
