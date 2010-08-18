/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToHistogramGenerator2.txx,v $
  Language:  C++
  Date:      $Date: 2009-08-17 18:29:01 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageToHistogramGenerator2_txx
#define __itkScalarImageToHistogramGenerator2_txx

#include "itkScalarImageToHistogramGenerator2.h"


namespace itk {
namespace Statistics {


template < class TImage >
ScalarImageToHistogramGenerator2< TImage >
::ScalarImageToHistogramGenerator2()
{
  m_ImageToListAdaptor = AdaptorType::New();
  m_HistogramGenerator = GeneratorType::New();
  m_HistogramGenerator->SetInput( m_ImageToListAdaptor );
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
  size.SetSize(1);
  size.Fill( numberOfBins );
  m_HistogramGenerator->SetHistogramSize( size );
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMin( RealPixelType minimumValue )
{
  typedef typename GeneratorType::HistogramMeasurementVectorType     MeasurementVectorType;
  MeasurementVectorType minVector(1);
  minVector[0] = minimumValue;
  m_HistogramGenerator->SetHistogramBinMinimum( minVector );
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMax( RealPixelType maximumValue )
{
  typedef typename GeneratorType::HistogramMeasurementVectorType     MeasurementVectorType;
  MeasurementVectorType maxVector(1);
  maxVector[0] = maximumValue;
  m_HistogramGenerator->SetHistogramBinMaximum( maxVector );
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
::SetAutoMinMax(bool autoMinMax)
{
  m_HistogramGenerator->SetAutoMinimumMaximum(autoMinMax);
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

} // end of namespace Statistics
} // end of namespace itk

#endif
