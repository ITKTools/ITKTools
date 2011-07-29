#ifndef NaryFilterFactory_h
#define NaryFilterFactory_h

#include "itkInPlaceImageFilter.h"

enum NaryFilterEnum {ADDITION, MEAN, MINUS, TIMES, DIVIDE, MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE, NARYMAGNITUDE};

template <class TInputImage, class TOutputImage>
class NaryFilterFactory
{
public:
  typename itk::InPlaceImageFilter<TInputImage, TOutputImage>::Pointer GetFilter(NaryFilterEnum filterType)
  {
    if(filterType == ADDITION)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryADDITION<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MEAN)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryMEAN<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MINUS)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryMINUS<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == TIMES)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryTIMES<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == DIVIDE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryDIVIDE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MAXIMUM)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryMAXIMUM<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == MINIMUM)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryMINIMUM<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == ABSOLUTEDIFFERENCE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryABSOLUTEDIFFERENCE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else if(filterType == NARYMAGNITUDE)
    {
      typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage, 
					  itk::Functor::NaryNARYMAGNITUDE<typename TInputImage::PixelType, typename TOutputImage::PixelType> >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      return filter.GetPointer();
    }
    else
    {
      std::cerr << "Invalid filter type specified!" << std::endl;
      return NULL;
    }
  }
};

/** Macros for easily instantiating the correct binary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::NaryFunctorImageFilter<
 *   InputImageType,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixelType,OutputPixelType> > PLUSFilterType;
 * naryFilter = (PLUSFilterType::New()).GetPointer();
 *
 */
#define InstantiateNaryFilterNoArg( name ) \
  typedef itk::NaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::Nary##name<InputPixelType, OutputPixelType> > name##FilterType; \
  if ( naryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempNaryFilter = name##FilterType::New(); \
    tempNaryFilter->InPlaceOn(); \
    naryFilter = tempNaryFilter.GetPointer(); \
  }
  
#endif