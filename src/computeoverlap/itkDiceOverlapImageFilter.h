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
#ifndef __itkDiceOverlapImageFilter_h_
#define __itkDiceOverlapImageFilter_h_

#include "itkImageToImageFilter.h"
#include <map>
#include <set>


namespace itk
{

/** \class DiceOverlapImageFilter
 * \brief Computes the Dice overlap per label
 *
 * \ingroup IntensityImageFilters
 * \ingroup Multithreaded
 */

template < typename TInputImage >
class ITK_EXPORT DiceOverlapImageFilter:
    public ImageToImageFilter< TInputImage, TInputImage >
{
public:

  /** Standard class typedefs. */
  typedef DiceOverlapImageFilter                          Self;
  typedef ImageToImageFilter< TInputImage, TInputImage >  Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( DiceOverlapImageFilter, ImageToImageFilter );

  /** Image dimension. */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Pixel Type of the input image */
  typedef TInputImage                                       InputImageType;
  typedef typename InputImageType::Pointer                  InputImagePointer;
  typedef typename InputImageType::ConstPointer             InputImageConstPointer;
  typedef typename InputImageType::PixelType                InputPixelType;
  typedef typename InputImageType::RegionType               InputImageRegionType;
  typedef typename NumericTraits<
    InputPixelType>::RealType                               RealType;
  typedef typename NumericTraits<
    InputPixelType>::ScalarRealType                         ScalarRealType;

  /** Dice computation related typedefs. */
  typedef std::map<InputPixelType, std::size_t>             OverlapMapType;
  typedef std::map<InputPixelType, ScalarRealType>          OverlapMapRealType;
  typedef std::set<InputPixelType>                          LabelsType;

  /** Set and get the user-requested labels for which the overlaps a. */
  //itkSetMacro( RequestedLabels, LabelsType );
  virtual void SetRequestedLabels( const LabelsType & arg )
  {
    if( this->m_RequestedLabels != arg )
    {
      this->m_RequestedLabels = arg;
      this->Modified();
    }
  }
  //itkGetConstReferenceMacro( RequestedLabels, LabelsType );
  virtual const LabelsType & GetRequestedLabels() const
  {
    return this->m_RequestedLabels;
  }

  /** Get the Dice overlaps, all of them. */
  //itkGetConstReferenceMacro( DiceOverlap, OverlapMapRealType );
  virtual const OverlapMapRealType & GetDiceOverlap() const
  {
    return this->m_DiceOverlap;
  }

  /** Print the Dice overlaps, only the requested ones. */
  void PrintRequestedDiceOverlaps( void );

// #ifdef ITK_USE_CONCEPT_CHECKING
//   /** Begin concept checking */
//   itkConceptMacro( InputHasNumericTraitsCheck,
//     (Concept::HasNumericTraits<InputPixelType>) );
//   /** End concept checking */
// #endif

protected:
  DiceOverlapImageFilter();
  virtual ~DiceOverlapImageFilter() {};

  /** PrintSelf. */
  void PrintSelf( std::ostream& os, Indent indent ) const;

  virtual void BeforeThreadedGenerateData( void );
  virtual void AfterThreadedGenerateData( void );

  /** ThreadedGenerateData. */
  virtual void ThreadedGenerateData(
    const InputImageRegionType & outputRegionForThread,
    ThreadIdType threadId );

private:
  DiceOverlapImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Member variables. */
  LabelsType                    m_RequestedLabels;

  std::vector<OverlapMapType>   m_SumA;
  std::vector<OverlapMapType>   m_SumB;
  std::vector<OverlapMapType>   m_SumC;
  OverlapMapRealType            m_DiceOverlap;

}; // end class DiceOverlapImageFilter

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiceOverlapImageFilter.txx"
#endif

#endif // end #ifndef __itkDiceOverlapImageFilter_h_
