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
#ifndef __itkCohenWeightedKappaStatistic_h_
#define __itkCohenWeightedKappaStatistic_h_

#include "itkKappaStatisticBase.h"

namespace itk {
namespace Statistics {

/** \class CohenWeightedKappaStatistic
 *
 * \brief This class computes the weighted kappa statistic for two observers.
 *
 * The kappa statistic is a measure for the agreement of observers.
 *
 * n:  the number of observers = 2
 * N:  the number of observations
 * k:  the number of categories
 *
 * The implementation is based on the Chapter 14.3 of the book:
 *
 *   Douglas G. Altman, "Practical Statistics for Medical Research"
 *
 * The Cohen weighted kappa statistic is suitable only for two observers.
 *
 * \ingroup Statistics
 *
 */
class ITK_EXPORT CohenWeightedKappaStatistic : public KappaStatisticBase
{
public:
  /** Standard class typedefs. */
  typedef CohenWeightedKappaStatistic Self;
  typedef KappaStatisticBase          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( CohenWeightedKappaStatistic, KappaStatisticBase );

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Typedefs from the Superclass. */
  typedef Superclass::CategoryType  CategoryType;
  typedef Superclass::SampleType    SampleType;
  typedef Superclass::SamplesType   SamplesType;
  typedef std::vector< std::vector<double> > WeightsType;

  /** Set and get the weights. */
  virtual void SetWeights( const WeightsType & weights );
  virtual void SetWeights( const std::string & weights );
  virtual WeightsType GetWeights( void ) const;
  itkGetMacro( WeightsName, std::string );

  /** The function that computes the kappa statistic value. */
  virtual void ComputeKappaStatisticValue( double & Po, double & Pe,
    double & kappa );

  /** The function that computes the kappa statistic value and standard deviation. */
  virtual void ComputeKappaStatisticValueAndStandardDeviation(
    double & Po, double & Pe, double & kappa, double & std, const bool & compare );

protected:
  CohenWeightedKappaStatistic();
  virtual ~CohenWeightedKappaStatistic() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Function to check if the input is valid. */
  virtual bool CheckObservations( const SamplesType & observations ) const;

  /** Function to check if the input is valid. */
  virtual bool CheckWeights( const WeightsType & weights ) const;

private:
  CohenWeightedKappaStatistic(const Self&); // purposely not implemented
  void operator=(const Self&);       // purposely not implemented

  /** A helper function to initialize weights. */
  void InitializeWeights( const std::string & weights, const unsigned int k );

  /** A helper function that calculate the confusion matrix,
   * i.e. the f_{ij}.
   */
  void ComputeConfusionMatrix( const unsigned int N, const unsigned int k );

  /** Member variables. */
  std::string m_WeightsName;
  WeightsType m_Weights;
  SamplesType m_ConfusionMatrix;

}; // end class CohenWeightedKappaStatistic

} // end of namespace Statistics
} // end namespace itk

#endif // end #ifndef __itkCohenWeightedKappaStatistic_h_
