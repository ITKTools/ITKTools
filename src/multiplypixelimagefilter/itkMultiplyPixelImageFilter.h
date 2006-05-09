/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiplyPixelImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006-05-09 15:34:58 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiplyPixelImageFilter_h
#define __itkMultiplyPixelImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{
  
/** \class MultiplyPixelImageFilter
 * \brief Multiplies the intensity values pixel-wise with a multiplyer.
 *
 * \ingroup IntensityImageFilters Multithreaded
 */

namespace Function {
//namespace Functor { ???
  
template< class TInput, class TOutput>
class MultiplyPixel
{
public:
	typedef typename NumericTraits<TInput>::RealType RealType;

  MultiplyPixel() { this->m_Multiplyer = NumericTraits< RealType >::One; };
  ~MultiplyPixel() {}

  bool operator!=( const MultiplyPixel & other ) const
  {
		if ( other.GetMultiplyer() != this->m_Multiplyer )
		{
			return true;
		}
		else return false;
  }
  bool operator==( const MultiplyPixel & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  {
		const RealType ra = static_cast<RealType>( A );
    return static_cast<TOutput>( ra * this->m_Multiplyer );
  }
	/** Set m_Addition. */
	void SetMultiplyer( RealType arg ){ this->m_Multiplyer = arg; };
	RealType GetMultiplyer(void) const { return this->m_Multiplyer; };

private:
	RealType m_Multiplyer;
}; // end class MultiplyPixel
} // end namespace Function

template <class TInputImage, class TOutputImage>
class ITK_EXPORT MultiplyPixelImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage,
	Function::MultiplyPixel< typename TInputImage::PixelType,
	typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef MultiplyPixelImageFilter					Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
		Function::MultiplyPixel< typename TInputImage::PixelType, 
		typename TOutputImage::PixelType> >		  Superclass;
  typedef SmartPointer<Self>								Pointer;
  typedef SmartPointer<const Self>					ConstPointer;

	/** Typedefs. */
	typedef typename TInputImage::PixelType		InputPixelType;
	typedef typename NumericTraits<InputPixelType>::RealType RealType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

	/** Set the number that has to be added. */
	void SetMultiplyer( RealType arg ){
		if ( arg != this->GetFunctor().GetMultiplyer() )
		{
			this->GetFunctor().SetMultiplyer( arg );
			this->Modified();
		}
	};
  
protected:
  MultiplyPixelImageFilter() {}
  virtual ~MultiplyPixelImageFilter() {}

private:
  MultiplyPixelImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
