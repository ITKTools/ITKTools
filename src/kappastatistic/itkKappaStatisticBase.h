#ifndef __itkKappaStatisticBase_h
#define __itkKappaStatisticBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include <vector>
#include <map>
#include "vnl/vnl_math.h"

namespace itk {
namespace Statistics {

/** \class KappaStatisticBase
 *
 * \brief This class is an abstract class for all variants of the kappa statistic.
 *
 * Multiple observers (raters) each rate multiple cases (subjects)
 * (which is making an observation). The rating consist of assigning
 * a case to a category.
 * The kappa statistic is a measure for the agreement of observers.
 *
 * n:  the number of observers
 * N:  the number of observations
 * k:  the number of categories
 *
 *
 * \ingroup Statistics
 *
 */
class ITK_EXPORT KappaStatisticBase : public Object
{
public:
  /** Standard class typedefs. */
  typedef KappaStatisticBase          Self;
  typedef Object                      Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( KappaStatisticBase, Object );

  /** Method for creation through the object factory.
   * Is not valid for abstract classes. */
  //itkNewMacro( Self );

  /** Typedefs. */
  typedef unsigned int                CategoryType;
  typedef std::vector< CategoryType > SampleType;
  typedef std::vector< SampleType >   SamplesType;
  typedef unsigned int                CountType;

  /** Set and get the observations. */
  virtual void SetObservations( const SamplesType observations );
  SamplesType GetObservations( void ) const;

  /** Get the number of observers. */
  itkGetConstMacro( NumberOfObservers, CountType );

  /** Get the number of observations. */
  itkGetConstMacro( NumberOfObservations, CountType );

  /** Get the number of categories. */
  itkGetConstMacro( NumberOfCategories, CountType );

  /** The function that computes the kappa statistic value. */
  virtual void ComputeKappaStatisticValue( double & Po, double & Pe,
    double & kappa ) = 0;

  /** The function that computes the kappa statistic value. */
  virtual void ComputeKappaStatisticValueAndStandardDeviation(
    double & Po, double & Pe, double & kappa, double & std, const bool & compare ) = 0;

protected:
  KappaStatisticBase();
  virtual ~KappaStatisticBase() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Function to check if the input is valid. */
  virtual bool CheckObservations( const SamplesType & observations ) const;

  SamplesType m_Observations;
  std::map<unsigned int,unsigned int>  m_Indices;

private:
  KappaStatisticBase(const Self&); // purposely not implemented
  void operator=(const Self&);     // purposely not implemented

  /** Compute the number of observers. */
  virtual void ComputeNumberOfObservers( void );

  /** Compute the number of observations. */
  virtual void ComputeNumberOfObservations( void );

  /** Compute the number of categories. */
  virtual void ComputeNumberOfCategories( void );

  /** Member variables. */
  CountType m_NumberOfObservers;
  CountType m_NumberOfObservations;
  CountType m_NumberOfCategories;

}; // end class KappaStatisticBase

} // end of namespace Statistics
} // end namespace itk

#endif // end #ifndef __itkKappaStatisticBase_h

