/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkComposeComplexImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2007-05-09 09:08:34 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkComposeComplexImageFilter_h
#define __itkComposeComplexImageFilter_h

#include "itkBinaryFunctorImageFilter.h"
#include <complex>

/** \class ComposeComplexImageFilter
 * \brief Implements pixel-wise composition of a complex pixel from two scalar images.
 *
 * This filter receives two scalar images as input. Each image containing
 * one of the complex components. The filter produces as output a
 * complex image in which the two components have been unified. The Component
 * type is preserved from the PixelType of the input images.
 *
 * \ingroup IntensityImageFilters
 */

namespace itk
{
  
namespace Function {  
  
template< class TInput, class TOutputComponentType >
class ComposeComplex
{
public:
  typedef std::complex<TTOutputComponentType> OutputType;
  ComposeComplex() {}
  ~ComposeComplex() {}
  bool operator!=( const ComposeComplex & ) const
  {
    return false;
  }
  bool operator==( const ComposeComplex & other ) const
  {
    return !(*this != other);
  }
  inline OutputType operator()(  const TInput & s1, 
                                 const TInput & s2 )
  {
    OutputType c( static_cast<TOutputComponentType>( s1 ),
      static_cast<TOutputComponentType>( s2 ) );
    return c;
  }
}; 
}

template < typename TInputImage,
  typename TOutputComponentType = ITK_TYPENAME TInputImage::PixelType >
class ITK_EXPORT ComposeComplexImageFilter :
    public
BinaryFunctorImageFilter<TInputImage, TInputImage,
  Image< std::complex< TOutputComponentType >,
  ::itk::GetImageDimension<TInputImage>::ImageDimension >,
  Function::ComposeComplex< ITK_TYPENAME TInputImage::PixelType,
    TOutputComponentType>   >
{
public:
  /** Standard class typedefs. */
  typedef ComposeComplexImageFilter   Self;
  typedef BinaryFunctorImageFilter<TInputImage, TInputImage,
    Image< std::complex< TOutputComponentType >,
      ::itk::GetImageDimension<TInputImage>::ImageDimension >,
    Function::ComposeComplex<ITK_TYPENAME TInputImage::PixelType,
      TOutputComponentType> >         Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;

  typedef typename Superclass::OutputImageType OutputImageType;
  
  /** Method for creation through the object factory. */
  itkNewMacro( Self );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( InputHasNumericTraitsCheck,
    ( Concept::HasNumericTraits<typename TInputImage::PixelType> ) );
  /** End concept checking */
#endif

protected:
  ComposeComplexImageFilter() {}
  virtual ~ComposeComplexImageFilter() {}

private:
  ComposeComplexImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end class ComposeComplexImageFilter

} // end namespace itk


#endif
