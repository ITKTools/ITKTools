#include "itkKappaStatisticBase.h"

namespace itk { 
namespace Statistics {


/**
 * *************** Constructor ****************
 */

KappaStatisticBase
::KappaStatisticBase()
{
  this->m_NumberOfObservers = 0;
  this->m_NumberOfObservations = 0;
  this->m_NumberOfCategories = 0;

} // end constructor


/**
 * *************** SetObservations ****************
 */

void KappaStatisticBase
::SetObservations( const SamplesType observations )
{
  bool check = this->CheckObservations( observations );
  if ( check )
  {
    this->Modified();
    this->m_Observations = observations;

    this->ComputeNumberOfObservers();
    this->ComputeNumberOfObservations();
    this->ComputeNumberOfCategories();
  }
  else
  {
    InvalidArgumentError exp(__FILE__, __LINE__);
    ::itk::OStringStream message;
    message << "itk::ERROR: " << this->GetNameOfClass()
      << "(" << this << "): "
      << "Invalid size of observations.";
    exp.SetDescription( message.str() );
    exp.SetLocation( ITK_LOCATION );
    throw exp;
  }

} // end SetObservations()


/**
 * *************** GetObservations ****************
 */

KappaStatisticBase::SamplesType
KappaStatisticBase
::GetObservations( void ) const
{
  return this->m_Observations;
} // end GetObservations()


/**
 * *************** ComputeNumberOfObservers ****************
 */

void KappaStatisticBase
::ComputeNumberOfObservers( void )
{
  this->m_NumberOfObservers = this->m_Observations.size();
} // end ComputeNumberOfObservers()


/**
 * *************** ComputeNumberOfObservations ****************
 */

void KappaStatisticBase
::ComputeNumberOfObservations( void )
{
  if ( this->m_Observations.size() )
  {
    this->m_NumberOfObservations = this->m_Observations[ 0 ].size();
  }
  else
  {
    this->m_NumberOfObservations = 0;
  }
} // end ComputeNumberOfObservations()


/**
 * *************** ComputeNumberOfCategories ****************
 */

void KappaStatisticBase
::ComputeNumberOfCategories( void )
{
  /** We count the number of different values in m_Observations.
   * This is done by copying the matrix to a list, sorting the list,
   * and removing adjacent duplicate elements. After these steps the
   * list should contain only the different values, each value once.
   */
  std::list<CategoryType> categories( 0 );
  for ( unsigned int i = 0; i < this->m_Observations.size(); ++i )
  {
    for ( unsigned int j = 0; j < this->m_Observations[ 0 ].size(); ++j )
    {
      categories.push_back( this->m_Observations[ i ][ j ] );
    }
  }
  categories.sort();
  categories.unique();

  /** Store the indices corresponding to the category label. */
  std::list<CategoryType>::iterator iter;
  unsigned int l = 0;
  for ( iter = categories.begin(); iter != categories.end(); iter++ )
  {
    this->m_Indices[ *iter ] = l;
    ++l;
  }

  this->m_NumberOfCategories = categories.size();
} // end ComputeNumberOfCategories()


/**
 * *************** CheckObservations ****************
 */

bool KappaStatisticBase
::CheckObservations( const SamplesType & observations ) const
{
  /** Check that at least two observers are compared. */
  if ( observations.size() < 2 ) return false;

  /** Check that at least one observation is made. */
  if ( observations[ 0 ].size() < 1 ) return false;

  /** Check that all observers have an equal amount of
   *  observations. The kappa statistic is a paired statistic,
   *  and therefore at least the number of observations should
   *  be equal.
   */
  unsigned int obs = observations[ 0 ].size();
  for ( unsigned int i = 1; i < observations.size(); ++i )
  {
    if ( obs != observations[ i ].size() ) return false;
  }

  /** Return a value. */
  return true;
  
} // end CheckObservations()


/**
 * *************** PrintSelf ****************
 */

void KappaStatisticBase
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Number of observers:    "
    << this->m_NumberOfObservers << std::endl;
  os << indent << "Number of observations: "
    << this->m_NumberOfObservations << std::endl;
  os << indent << "Number of categories:   "
    << this->m_NumberOfCategories << std::endl;

} // end PrintSelf()


} // end of namespace Statistics
} // end namespace itk

