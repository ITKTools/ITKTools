/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAddToPixelImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006-05-09 15:34:58 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAddToPixelImageFilter_h
#define __itkAddToPixelImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

namespace itk
{
  
/** \class AddToPixelImageFilter
 * \brief Adds a value to the intensity values pixel-wise.
 *
 * \ingroup IntensityImageFilters Multithreaded
 */

namespace Function {
//namespace Functor { ???
  
template< class TInput, class TOutput>
class AddToPixel
{
public:
  AddToPixel() { this->m_Addition = NumericTraits< TInput >::Zero; };
  ~AddToPixel() {}
  bool operator!=( const AddToPixel & other ) const
  {
		if ( other.GetAddition() != this->m_Addition )
		{
			return true;
		}
		else return false;
  }
  bool operator==( const AddToPixel & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  {
    return static_cast<TOutput>( A + this->m_Addition );
  }
	/** Set m_Addition. */
	void SetAddition( TInput arg ){ this->m_Addition = arg; };
	TInput GetAddition(void) const { return this->m_Addition; };

private:
	TInput m_Addition;
}; // end class AddToPixel
} // end namespace Function

template <class TInputImage, class TOutputImage>
class ITK_EXPORT AddToPixelImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage,
	Function::AddToPixel< typename TInputImage::PixelType,
	typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef AddToPixelImageFilter							Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
		Function::AddToPixel< typename TInputImage::PixelType, 
		typename TOutputImage::PixelType> >		  Superclass;
  typedef SmartPointer<Self>								Pointer;
  typedef SmartPointer<const Self>					ConstPointer;

	/** Typedefs. */
	typedef typename TInputImage::PixelType		InputPixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

	/** Set the number that has to be added. */
	void SetAddition( InputPixelType arg ){
		if ( arg != this->GetFunctor().GetAddition() )
		{
			this->GetFunctor().SetAddition( arg );
			this->Modified();
		}
	};
  
protected:
  AddToPixelImageFilter() {}
  virtual ~AddToPixelImageFilter() {}

private:
  AddToPixelImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
