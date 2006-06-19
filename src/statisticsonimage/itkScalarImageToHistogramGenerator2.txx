/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToHistogramGenerator2.txx,v $
  Language:  C++
  Date:      $Date: 2006-06-19 12:51:56 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkScalarImageToHistogramGenerator2_txx
#define _itkScalarImageToHistogramGenerator2_txx

#include "itkScalarImageToHistogramGenerator2.h"


namespace itk { 
namespace Statistics {


template < class TImage >
ScalarImageToHistogramGenerator2< TImage >
::ScalarImageToHistogramGenerator2() 
{
  m_ImageToListAdaptor = AdaptorType::New();
  m_HistogramGenerator = GeneratorType::New();
  m_HistogramGenerator->SetListSample( m_ImageToListAdaptor );
}



template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetInput( const ImageType * image ) 
{
  m_ImageToListAdaptor->SetImage( image );
}


template < class TImage >
const typename ScalarImageToHistogramGenerator2< TImage >::HistogramType *
ScalarImageToHistogramGenerator2< TImage >
::GetOutput() const
{
  return m_HistogramGenerator->GetOutput();
}



template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::Compute() 
{
  m_HistogramGenerator->Update();
}



template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetNumberOfBins( unsigned int numberOfBins ) 
{
  typename HistogramType::SizeType size;
  size.Fill( numberOfBins );
  m_HistogramGenerator->SetNumberOfBins( size );
}



template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetMarginalScale( double marginalScale )
{
  m_HistogramGenerator->SetMarginalScale( marginalScale );
}




template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "ImageToListSample adaptor = " << m_ImageToListAdaptor << std::endl;
  os << "HistogramGenerator = " << m_HistogramGenerator << std::endl;
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMin(const PixelType & histogramMin)
{
  typename HistogramType::MeasurementVectorType hmin;
  hmin.Fill(histogramMin);
  m_HistogramGenerator->SetHistogramMin(hmin);
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMax(const PixelType & histogramMax)
{
   typename HistogramType::MeasurementVectorType hmax;
   hmax.Fill(histogramMax);
   m_HistogramGenerator->SetHistogramMax(hmax);
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetAutoMinMax(bool autoMinMax)
{
  m_HistogramGenerator->SetAutoMinMax(autoMinMax);
}




} // end of namespace Statistics 
} // end of namespace itk

#endif


