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
#ifndef __itkDeformationFieldBendingEnergyFilter_h_
#define __itkDeformationFieldBendingEnergyFilter_h_

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

#endif // end #ifndef __itkDeformationFieldBendingEnergyFilter_h_
