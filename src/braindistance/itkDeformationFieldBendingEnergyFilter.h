/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDeformationFieldBendingEnergyFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-05-20 16:21:47 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDeformationFieldBendingEnergyFilter_h
#define __itkDeformationFieldBendingEnergyFilter_h

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkVector.h"
#include "vnl/vnl_matrix.h"

//This class inherits from itkDisplacementFieldJacobianDeterminantFilter
//and simply overrides the EvaluateAtNeighborhood function.
#include "itkDisplacementFieldJacobianDeterminantFilter.h"

namespace itk
{
/** \class DeformationFieldBendingEnergyFilter
 *
 * \brief Computes a scalar image from a vector image (e.g., deformation field)
 * input, where each output scalar at each pixel is the bending energy
 * of the vector field at that location.  
 * The bending energy happens to be identical for Transformation and Displacement fields.
 * 
 * Bending energy = sum of all squared second order derivatives.
 *
 * This class is a specialization of the DisplacementFieldJacobianDeterminantFilter, further
 * details regarding it's implementation should be reviewed in 
 * itkDisplacementFieldJacobianDeterminantFilter.h.
 *
 * \ingroup GradientFilters
 *
 * \sa DisplacementFieldJacobianDeterminantFilter
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 *
 *
 * \author Stefan Klein, Erasmus MC Rotterdam, The Netherlands.
 */
template < typename TInputImage,
           typename TRealType = float,
           typename TOutputImage = Image< TRealType,
                                          ::itk::GetImageDimension<TInputImage>::ImageDimension >
>
class ITK_EXPORT DeformationFieldBendingEnergyFilter :
    public DisplacementFieldJacobianDeterminantFilter< TInputImage,TRealType,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef DeformationFieldBendingEnergyFilter Self;
  typedef DisplacementFieldJacobianDeterminantFilter<
    TInputImage,TRealType,TOutputImage>
                                                    Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(DeformationFieldBendingEnergyFilter, DisplacementFieldJacobianDeterminantFilter);

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TOutputImage::PixelType OutputPixelType;
  typedef typename TInputImage::PixelType  InputPixelType;

  /** Image typedef support */
  typedef TInputImage                       InputImageType;
  typedef TOutputImage                      OutputImageType;
  typedef typename InputImageType::Pointer  InputImagePointer;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** The dimensionality of the input and output images. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Length of the vector pixel type of the input image. */
  itkStaticConstMacro(VectorDimension, unsigned int,
                      InputPixelType::Dimension);

  /** Define the data type and the vector of data type used in calculations. */
  typedef TRealType RealType;
  typedef Vector<
    TRealType, ::itk::GetVectorDimension<InputPixelType>::VectorDimension>
                    RealVectorType;
  typedef Image<
    RealVectorType, ::itk::GetImageDimension<TInputImage>::ImageDimension>
                    RealVectorImageType;

  /** Type of the iterator that will be used to move through the image.  Also
      the type which will be passed to the evaluate function */
  typedef ConstNeighborhoodIterator<RealVectorImageType> ConstNeighborhoodIteratorType;
  typedef typename ConstNeighborhoodIteratorType::RadiusType RadiusType;

  /** Superclass typedefs. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual TRealType EvaluateAtNeighborhood(const ConstNeighborhoodIteratorType &it) const;
protected:
  DeformationFieldBendingEnergyFilter();
  virtual ~DeformationFieldBendingEnergyFilter() {}

private:
  DeformationFieldBendingEnergyFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDeformationFieldBendingEnergyFilter.txx"
#endif

#endif
