/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMinErrorThresholdImageCalculator.h,v $
  Language:  C++
  Date:      $Date: 2010-05-04 13:18:45 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMinErrorThresholdImageCalculator_h
#define __itkMinErrorThresholdImageCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkNumericTraits.h"

namespace itk
{

/** \class MinErrorThresholdImageCalculator
 * \brief Computes a threshold using the minimum error thresholding algorithm
 *
 * This calculator computes a threshold using the minimum error thresholding algorithm to
 * separate the image pixels into foreground and background components. The method relies on the
 * histogram of image intensities, and it tries to find the best mixture of two distributions
 * that fits the histogram with minimum error. This calculator provides two options for the mixture
 * which are a mixture of Gaussians and a mixture of Poissons. The minimum error threshold is the
 * one that minimizes the error criterion function, which depends on the chosen mixture type
 * \warning This method assumes that the input image consists of scalar pixel
 * types.
 *
 * \ingroup Operators
 * \author Yousef Al-Kofahi, Rensselear Polytechnic Institute (RPI), Troy, NY USA
 */
template <class TInputImage>
class ITK_EXPORT MinErrorThresholdImageCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef MinErrorThresholdImageCalculator Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MinErrorThresholdImageCalculator, Object);

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

  /** Set the input image. */
  itkSetConstObjectMacro(Image,ImageType);

  /** Compute the MinError's threshold for the input image. */
  void Compute( void );

  /** This function sets the option to use a mixture of Gaussians */
  void UseGaussianMixture(bool);

  /** Get the MinError's threshold value. */
  itkGetMacro(Threshold,PixelType);

  /** Return the estimated mixture parameters. */
  itkGetMacro(AlphaLeft,double);
  itkGetMacro(AlphaRight,double);
  itkGetMacro(PriorLeft,double);
  itkGetMacro(PriorRight,double);
  itkGetMacro(StdLeft,double);
  itkGetMacro(StdRight,double);
  /** Set/Get the number of histogram bins. Default is 128. */
  itkSetClampMacro( NumberOfHistogramBins, unsigned long, 1,
                    NumericTraits<unsigned long>::max() );
  itkGetMacro( NumberOfHistogramBins, unsigned long );

  /** Set the region over which the values will be computed */
  void SetRegion( const RegionType & region );

protected:
  MinErrorThresholdImageCalculator();
  virtual ~MinErrorThresholdImageCalculator() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MinErrorThresholdImageCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Some needed variables */
  PixelType            m_Threshold;
  unsigned long        m_NumberOfHistogramBins;
  ImageConstPointer    m_Image;
  RegionType           m_Region;
  bool                 m_RegionSetByUser;
  double         m_AlphaLeft;
  double         m_AlphaRight;
  double         m_PriorLeft;
  double         m_PriorRight;
  double         m_StdLeft;
  double           m_StdRight;
  unsigned int       m_UseGaussian;
  unsigned int       m_usePoisson;
};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMinErrorThresholdImageCalculator.txx"
#endif

#endif
