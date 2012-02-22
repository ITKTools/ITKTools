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
#ifndef __itkScalarImageToHistogramGenerator2_txx_
#define __itkScalarImageToHistogramGenerator2_txx_

#include "itkScalarImageToHistogramGenerator2.h"


namespace itk {
namespace Statistics {


template < class TImage >
ScalarImageToHistogramGenerator2< TImage >
::ScalarImageToHistogramGenerator2()
{
  this->m_ImageToListAdaptor = AdaptorType::New();
  this->m_HistogramGenerator = GeneratorType::New();
  this->m_HistogramGenerator->SetInput( this->m_ImageToListAdaptor );
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetInput( const ImageType * image )
{
  this->m_ImageToListAdaptor->SetImage( image );
}


template < class TImage >
const typename ScalarImageToHistogramGenerator2< TImage >::HistogramType *
ScalarImageToHistogramGenerator2< TImage >
::GetOutput() const
{
  return this->m_HistogramGenerator->GetOutput();
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::Compute()
{
  this->m_HistogramGenerator->Update();
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetNumberOfBins( unsigned int numberOfBins )
{
  typename HistogramType::SizeType size;
  size.SetSize(1);
  size.Fill( numberOfBins );
  this->m_HistogramGenerator->SetHistogramSize( size );
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMin( RealPixelType minimumValue )
{
  typedef typename GeneratorType::HistogramMeasurementVectorType     MeasurementVectorType;
  MeasurementVectorType minVector(1);
  minVector[0] = minimumValue;
  this->m_HistogramGenerator->SetHistogramBinMinimum( minVector );
}


template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetHistogramMax( RealPixelType maximumValue )
{
  typedef typename GeneratorType::HistogramMeasurementVectorType     MeasurementVectorType;
  MeasurementVectorType maxVector(1);
  maxVector[0] = maximumValue;
  this->m_HistogramGenerator->SetHistogramBinMaximum( maxVector );
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetMarginalScale( double marginalScale )
{
  this->m_HistogramGenerator->SetMarginalScale( marginalScale );
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::SetAutoMinMax(bool autoMinMax)
{
  this->m_HistogramGenerator->SetAutoMinimumMaximum(autoMinMax);
}

template < class TImage >
void
ScalarImageToHistogramGenerator2< TImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << "ImageToListSample adaptor = " << this->m_ImageToListAdaptor << std::endl;
  os << "HistogramGenerator = " << this->m_HistogramGenerator << std::endl;
}

} // end of namespace Statistics
} // end of namespace itk

#endif
