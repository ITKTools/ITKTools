#include "itkFleissKappaStatistic.h"

namespace itk {
namespace Statistics {


/**
 * *************** Constructor ****************
 */

FleissKappaStatistic
::FleissKappaStatistic()
{
} // end constructor


/**
 * *************** ComputeObservationMatrix ****************
 */

void FleissKappaStatistic
::ComputeObservationMatrix( const unsigned int n,
  const unsigned int N, const unsigned int k )
{
  /**
   * n:  the number of observers
   * N:  the number of observations
   * k:  the number of categories
   *
   * The observation matrix is of size N by k, i.e. the columns
   * contain the k categories and each row contains the
   * observations of the n observers.
   * An element n_{ij} of the observation matrix should denote
   * the number of observers that give observation / subject / case
   * i a rating in category j.
   */

  /** Construct the observation matrix. */
  this->m_ObservationMatrix.resize( 0 );
  this->m_ObservationMatrix.resize( N );
  for( unsigned int i = 0; i < N; ++i )
  {
    this->m_ObservationMatrix[ i ].resize( k, 0 );
  }

  /** We loop over the observations, and increase the correct bin. */
  for( unsigned int l = 0; l < n; ++l )
  {
    for( unsigned int i = 0; i < N; ++i )
    {
      unsigned int ind = this->m_Indices[ this->m_Observations[ l ][ i ] ];
      this->m_ObservationMatrix[ i ][ ind ]++;
    }
  }

} // end ComputeObservationMatrix()


/**
 * *************** ComputeKappaStatisticValue ****************
 */

void FleissKappaStatistic
::ComputeKappaStatisticValue( double & Po, double & Pe, double & kappa )
{
  /** The observations has to be set previously by the user. */
  this->CheckObservations( this->m_Observations );

  /** Get some numbers. */
  unsigned int n = this->GetNumberOfObservers();
  unsigned int N = this->GetNumberOfObservations();
  unsigned int k = this->GetNumberOfCategories();

  /** Compute the observation matrix. */
  this->ComputeObservationMatrix( n, N, k );

  /** We are ready to compute the kappa statistic.
   * This is done in parts:
   * - calculate p[ j ] and Pe
   * - calculate P[ i ] and Po
   */
  std::vector< double > p( k, 0.0 );
  std::vector< double > P( N, 0.0 );
  Po = Pe = kappa = 0.0;
  for( unsigned int j = 0; j < k; ++j )
  {
    for( unsigned int i = 0; i < N; ++i )
    {
      p[ j ] += static_cast<double>( this->m_ObservationMatrix[ i ][ j ] );
    }
    p[ j ] /= static_cast<double>( n * N );
    Pe += p[ j ] * p[ j ];
  }

  for( unsigned int i = 0; i < N; ++i )
  {
    for( unsigned int j = 0; j < k; ++j )
    {
      double nij = static_cast<double>( this->m_ObservationMatrix[ i ][ j ] );
      P[ i ] += nij * nij - nij;
    }
    P[ i ] /= n * ( n - 1.0 );
    Po += P[ i ];
  }
  Po /= N;

  // the above can probably be done in one loop over i and j,
  // but this is much better readable.

  /** Compute kappa. */
  kappa = ( Po - Pe ) / ( 1.0 - Pe );

} // end ComputeKappaStatisticValue()


/**
 * *************** ComputeKappaStatisticValueAndStandardDeviation ****************
 */

void FleissKappaStatistic
::ComputeKappaStatisticValueAndStandardDeviation(
  double & Po, double & Pe, double & kappa, double & std, const bool & compare )
{
  /** The observations has to be set previously by the user. */
  this->CheckObservations( this->m_Observations );

  /** Get some numbers. */
  unsigned int n = this->GetNumberOfObservers();
  unsigned int N = this->GetNumberOfObservations();
  unsigned int k = this->GetNumberOfCategories();

  /** Compute the observation matrix. */
  this->ComputeObservationMatrix( n, N, k );

  /** We are ready to compute the kappa statistic.
   * This is done in parts:
   * - calculate p[ j ] and Pe
   * - calculate P[ i ] and Po
   */
  std::vector< double > p( k, 0.0 );
  std::vector< double > P( N, 0.0 );
  double p3 = 0.0;
  Po = Pe = kappa = std = 0.0;
  for( unsigned int j = 0; j < k; ++j )
  {
    for( unsigned int i = 0; i < N; ++i )
    {
      p[ j ] += static_cast<double>( this->m_ObservationMatrix[ i ][ j ] );
    }
    p[ j ] /= static_cast<double>( n * N );
    Pe += p[ j ] * p[ j ];
    p3 += p[ j ] * p[ j ] * p[ j ];
  }

  for( unsigned int i = 0; i < N; ++i )
  {
    for( unsigned int j = 0; j < k; ++j )
    {
      double nij = static_cast<double>( this->m_ObservationMatrix[ i ][ j ] );
      P[ i ] += nij * nij - nij;
    }
    P[ i ] /= n * ( n - 1.0 );
    Po += P[ i ];
  }
  Po /= N;

  // the above can probably be done in one loop over i and j,
  // but this is much better readable.

  /** Compute the standard deviation. */
  std = Pe - ( 2.0 * n - 3.0 ) * Pe * Pe + 2.0 * ( n - 2.0 ) * p3;
  std /= ( 1.0 - Pe ) * ( 1.0 - Pe );
  std *= 2.0 / ( N * n * ( n - 1.0 ) );
  std = std::sqrt( std );

  /** Compute kappa. */
  kappa = ( Po - Pe ) / ( 1.0 - Pe );

} // end ComputeKappaStatisticValueAndStandardDeviation()


/**
 * *************** PrintSelf ****************
 */

void
FleissKappaStatistic
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  /** Print the observation matrix. */
  unsigned int N = this->m_ObservationMatrix.size();
  unsigned int k = 0;
  if( N > 0 ) k = this->m_ObservationMatrix[ 0 ].size();
  if( N > 0 && k > 0 )
  {
    os << indent << "Observation matrix:" << std::endl;
    for( unsigned int i = 0; i < N; ++i )
    {
      os << indent;
      for( unsigned int j = 0; j < k; ++j )
      {
        os << this->m_ObservationMatrix[ i ][ j ] << " ";
      }
      os << std::endl;
    }
  }
  else
  {
    os << indent << "Observation matrix: (unknown)" << std::endl;
  }

} // end PrintSelf()

} // end of namespace Statistics
} // end namespace itk

