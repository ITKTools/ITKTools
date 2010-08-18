/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToHistogramGenerator2.h,v $
  Language:  C++
  Date:      $Date: 2009-08-08 14:18:12 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageToHistogramGenerator2_h
#define __itkScalarImageToHistogramGenerator2_h


#include "itkImageToListSampleAdaptor.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkObject.h"

namespace itk {
namespace Statistics {

/** \class ScalarImageToHistogramGenerator2
 *
 * \brief TODO
 */
template< class TImageType >
class ScalarImageToHistogramGenerator2 : public Object
{
public:
  /** Standard typedefs */
  typedef ScalarImageToHistogramGenerator2  Self;
  typedef Object                           Superclass;
  typedef SmartPointer<Self>               Pointer;
  typedef SmartPointer<const Self>         ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ScalarImageToHistogramGenerator2, Object);

  /** standard New() method support */
  itkNewMacro(Self);

  typedef TImageType                                      ImageType;
  typedef itk::Statistics::ImageToListSampleAdaptor<
                                              ImageType
                                                      >   AdaptorType;
  typedef typename AdaptorType::Pointer                   AdaptorPointer;
  typedef typename ImageType::PixelType                   PixelType;
  typedef typename NumericTraits< PixelType >::RealType   RealPixelType;

  typedef itk::Statistics::Histogram< double > HistogramType;
  typedef itk::Statistics::SampleToHistogramFilter< AdaptorType, HistogramType > GeneratorType;

  typedef typename GeneratorType::Pointer                   GeneratorPointer;

  typedef typename HistogramType::Pointer                   HistogramPointer;
  typedef typename HistogramType::ConstPointer              HistogramConstPointer;

public:

  /** Triggers the Computation of the histogram */
  void Compute( void );

  /** Connects the input image for which the histogram is going to be computed */
  void SetInput( const ImageType * );

  /** Return the histogram. o
   \warning This output is only valid after the Compute() method has been invoked
   \sa Compute */
  const HistogramType * GetOutput() const;

  /** Set number of histogram bins */
  void SetNumberOfBins( unsigned int numberOfBins );

  /** Set marginal scale value to be passed to the histogram generator */
  void SetMarginalScale( double marginalScale );

  /** Set the minimum value from which the bins will be computed */
  void SetHistogramMin( RealPixelType minimumValue );

  /** Set the maximum value from which the bins will be computed */
  void SetHistogramMax( RealPixelType maximumValue );

  /** SK: changed wrt original ITK versions:
   * Add this function:
   */
  void SetAutoMinMax(bool autoMinMax);

protected:
  ScalarImageToHistogramGenerator2();
  virtual ~ScalarImageToHistogramGenerator2() {};
  void PrintSelf(std::ostream& os, Indent indent) const;


private:

  AdaptorPointer      m_ImageToListAdaptor;

  GeneratorPointer    m_HistogramGenerator;

  ScalarImageToHistogramGenerator2(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScalarImageToHistogramGenerator2.txx"
#endif

#endif
