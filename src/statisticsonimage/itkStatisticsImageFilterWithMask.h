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
/**
 * A copy of the itkStatisticsImageFilter, but changed to use a mask.
 * NB: the class is still named StatisticsImageFilter, so cannot be used
 * at the same times as the the itk-class.
 *
 * Original ITK copyright message:
 */

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsImageFilterWithMask.h,v $
  Language:  C++
  Date:      $Date: 2006-06-22 09:29:12 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkStatisticsImageFilter_h_
#define __itkStatisticsImageFilter_h_

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkArray.h"
#include "itkSimpleDataObjectDecorator.h"


namespace itk {

/** \class StatisticsImageFilter
 * \brief Compute min. max, variance and mean of an Image.
 *
 * StatisticsImageFilter computes the minimum, maximum, sum, mean, variance
 * sigma of an image.  The filter needs all of its input image.  It
 * behaves as a filter with an input and output. Thus it can be inserted
 * in a pipeline with other filters and the statistics will only be
 * recomputed if a downstream filter changes.
 *
 * The filter passes its input through unmodified.  The filter is
 * threaded. It computes statistics in each thread then combines them in
 * its AfterThreadedGenerate method.
 *
 * \ingroup MathematicalStatisticsImageFilters
 */
template<class TInputImage>
class ITK_EXPORT StatisticsImageFilter :
    public ImageToImageFilter<TInputImage, TInputImage>
{
public:
  /** Standard Self typedef */
  typedef StatisticsImageFilter     Self;
  typedef ImageToImageFilter<
    TInputImage,TInputImage>        Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Runtime information support. */
  itkTypeMacro( StatisticsImageFilter, ImageToImageFilter );

  /** Image related typedefs. */
  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::Pointer    InputImagePointer;
  typedef typename InputImageType::RegionType RegionType ;
  typedef typename InputImageType::SizeType   SizeType ;
  typedef typename InputImageType::IndexType  IndexType ;
  typedef typename InputImageType::PointType  PointType ;
  typedef typename InputImageType::PixelType  PixelType ;

  /** Image related typedefs. */
  itkStaticConstMacro( ImageDimension, unsigned int,
    InputImageType::ImageDimension ) ;

  /** Type to use for computations. */
  typedef typename NumericTraits<PixelType>::RealType RealType;

  /** Smart Pointer type to a DataObject. */
  typedef typename DataObject::Pointer DataObjectPointer;

  /** Type of DataObjects used for scalar outputs */
  typedef SimpleDataObjectDecorator<RealType>  RealObjectType;
  typedef SimpleDataObjectDecorator<PixelType> PixelObjectType;

  /** Mask support.
   *  Type for the mask of the input image. Only pixels that are "inside"
   * this mask will be considered for the computation of the statistics.
   */
  typedef Image< unsigned char,
    itkGetStaticConstMacro( ImageDimension ) >    MaskType;
  typedef typename MaskType::Pointer              MaskPointer;

  /** Return the computed Minimum. */
  PixelType GetMinimum() const
  { return this->GetMinimumOutput()->Get(); }
  PixelObjectType* GetMinimumOutput();
  const PixelObjectType* GetMinimumOutput() const;

  /** Return the computed Maximum. */
  PixelType GetMaximum() const
  { return this->GetMaximumOutput()->Get(); }
  PixelObjectType* GetMaximumOutput();
  const PixelObjectType* GetMaximumOutput() const;

  /** Return the computed Mean. */
  RealType GetMean() const
    { return this->GetMeanOutput()->Get(); }
  RealObjectType* GetMeanOutput();
  const RealObjectType* GetMeanOutput() const;

  /** Return the computed absolute Mean. */
  RealType GetAbsoluteMean() const
    { return this->GetAbsoluteMeanOutput()->Get(); }
  RealObjectType* GetAbsoluteMeanOutput();
  const RealObjectType* GetAbsoluteMeanOutput() const;

  /** Return the computed Standard Deviation. */
  RealType GetSigma() const
    { return this->GetSigmaOutput()->Get(); }
  RealObjectType* GetSigmaOutput();
  const RealObjectType* GetSigmaOutput() const;

  /** Return the computed Variance. */
  RealType GetVariance() const
    { return this->GetVarianceOutput()->Get(); }
  RealObjectType* GetVarianceOutput();
  const RealObjectType* GetVarianceOutput() const;

  /** Return the compute Sum. */
  RealType GetSum() const
    { return this->GetSumOutput()->Get(); }
  RealObjectType* GetSumOutput();
  const RealObjectType* GetSumOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output.
   */
  virtual DataObjectPointer MakeOutput( unsigned int idx );

  /** Set/Get Mask */
  itkSetObjectMacro(Mask, MaskType);
  itkGetConstObjectMacro(Mask, MaskType);

protected:
  StatisticsImageFilter();
  ~StatisticsImageFilter(){};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Pass the input through unmodified. Do this by Grafting in the AllocateOutputs method. */
  void AllocateOutputs( void );

  /** Initialize some accumulators before the threads run. */
  void BeforeThreadedGenerateData( void );

  /** Do final mean and variance computation from data accumulated in threads. */
  void AfterThreadedGenerateData( void );

  /** Multi-thread version GenerateData. */
  void ThreadedGenerateData( const RegionType & outputRegionForThread,
    ThreadIdType threadId );

  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion( void );

  // Override since the filter produces all of its output
  void EnlargeOutputRequestedRegion( DataObject *data );

  MaskPointer m_Mask;

private:
  StatisticsImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  Array<RealType>  m_ThreadSum;
  Array<RealType>  m_ThreadAbsoluteSum;
  Array<RealType>  m_SumOfSquares;
  Array<long>      m_Count;
  Array<PixelType> m_ThreadMin;
  Array<PixelType> m_ThreadMax;

} ; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStatisticsImageFilterWithMask.txx"
#endif

#endif
