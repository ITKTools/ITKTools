#include "itkCohenWeightedKappaStatistic.h"


namespace itk {
namespace Statistics {


/**
 * *************** Constructor ****************
 */

CohenWeightedKappaStatistic
::CohenWeightedKappaStatistic()
{
  this->m_WeightsName = "";
} // end constructor


/**
 * *************** CheckObservations ****************
 */

bool CohenWeightedKappaStatistic
::CheckObservations( const SamplesType & observations ) const
{
  bool check = Superclass::CheckObservations( observations );
  if ( observations.size() != 2 ) return false;
  return check;
} // end CheckObservations()


/**
 * *************** CheckWeights ****************
 */

bool CohenWeightedKappaStatistic
::CheckWeights( const WeightsType & weights ) const
{
  /** Check that the weights are a square matrix. */
  unsigned int size = weights.size();
  for ( unsigned int i = 0; i < size; ++i )
  {
    if ( weights[ i ].size() != size ) return false;
  }
  return true;
} // end CheckWeights()


/**
 * *************** SetWeights ****************
 */

void CohenWeightedKappaStatistic
::SetWeights( const WeightsType & weights )
{
  bool check = CheckWeights( weights );
  if ( check )
  {
    this->Modified();
    this->m_Weights = weights;
    this->m_WeightsName = "user_defined";
  }
  else
  {
    InvalidArgumentError exp(__FILE__, __LINE__);
    ::itk::OStringStream message;
    message << "itk::ERROR: " << this->GetNameOfClass()
      << "(" << this << "): "
      << "Invalid size of weights.";
    exp.SetDescription( message.str() );
    exp.SetLocation( ITK_LOCATION );
    throw exp;
  }

} // end SetWeights()


/**
 * *************** SetWeights ****************
 */

void CohenWeightedKappaStatistic
::SetWeights( const std::string & weights )
{
  if ( weights == "linear" || weights == "quadratic"
    || weights == "equal" || weights == "identity" )
  {
    this->Modified();
    this->m_WeightsName = weights;
  }
  else
  {
    InvalidArgumentError exp(__FILE__, __LINE__);
    ::itk::OStringStream message;
    message << "itk::ERROR: " << this->GetNameOfClass()
      << "(" << this << "): "
      << "Invalid initialisation of weights.";
    exp.SetDescription( message.str() );
    exp.SetLocation( ITK_LOCATION );
    throw exp;
  }

} // end SetWeights()


/**
 * *************** InitializeWeights ****************
 */

void CohenWeightedKappaStatistic
::InitializeWeights( const std::string & weights, const unsigned int k )
{
  /** Construct the weights matrix. */
  this->m_Weights.resize( 0 );
  this->m_Weights.resize( k );
  for ( unsigned int i = 0; i < k; ++i )
  {
    this->m_Weights[ i ].resize( k, 0 );
  }

  /** An element w_{ij} of the weights matrix should be:
   * equal:     1
   * identity:  I_k, the identity matrix
   * linear:    1 - | i - j | / ( k - 1 )
   * quadratic: 1 - [ (i - j ) / ( k - 1 ) ]^2
   */
  for ( int i = 0; i < k; ++i )
  {
    for ( int j = 0; j < k; ++j )
    {
      if ( weights == "equal" )
      {
        this->m_Weights[ i ][ j ] = 1.0;
      }
      else if ( weights == "identity" )
      {
        if ( i == j ) this->m_Weights[ i ][ j ] = 1.0;
        else this->m_Weights[ i ][ j ] = 0.0;
      }
      else if ( weights == "linear" )
      {
        this->m_Weights[ i ][ j ] = 1.0 - vcl_abs( i - j ) / ( k - 1.0 );
      }
      else if ( weights == "quadratic" )
      {
        this->m_Weights[ i ][ j ] = 1.0 - vnl_math_sqr( ( i - j ) / ( k - 1.0 ) );
      }
    }
  }

} // end InitializeWeights()


/**
 * *************** GetWeights ****************
 */

CohenWeightedKappaStatistic::WeightsType
CohenWeightedKappaStatistic
::GetWeights( void ) const
{
  return this->m_Weights;
} // end GetWeights()


/**
 * *************** ComputeConfusionMatrix ****************
 */

void CohenWeightedKappaStatistic
::ComputeConfusionMatrix( const unsigned int N, const unsigned int k )
{
  /** n:  the number of observers
   *  N:  the number of observations
   *  k:  the number of categories
   */

  /** Construct the confusion matrix. */
  this->m_ConfusionMatrix.resize( 0 );
  this->m_ConfusionMatrix.resize( k );
  for ( unsigned int i = 0; i < k; ++i )
  {
    this->m_ConfusionMatrix[ i ].resize( k, 0 );
  }

  /** An element f_{ij} of the confusion matrix should denote
   * the number of times that observer 1 rates a subject in category i
   * and observer 2 in category j.
   * We loop over the observations, and increase the correct bin.
   */
  for ( unsigned int i = 0; i < N; ++i )
  {
    unsigned int ind0 = this->m_Indices[ this->m_Observations[ 0 ][ i ] ];
    unsigned int ind1 = this->m_Indices[ this->m_Observations[ 1 ][ i ] ];
    this->m_ConfusionMatrix[ ind0 ][ ind1 ]++;
  }

} // end ComputeConfusionMatrix()


/**
 * *************** ComputeKappaStatisticValue ****************
 */

void CohenWeightedKappaStatistic
::ComputeKappaStatisticValue( double & Po, double & Pe, double & kappa )
{
  /** The observations has to be set previously by the user. */
  this->CheckObservations( this->m_Observations );

  /** Get some numbers. */
  unsigned int n = this->GetNumberOfObservers();
  unsigned int N = this->GetNumberOfObservations();
  unsigned int k = this->GetNumberOfCategories();

  /** Check if the weights are set. */
  if ( this->m_WeightsName == "" )
  {
    InvalidArgumentError exp(__FILE__, __LINE__);
    ::itk::OStringStream message;
    message << "itk::ERROR: " << this->GetNameOfClass()
      << "(" << this << "): "
      << "Weights not initialized.";
    exp.SetDescription( message.str() );
    exp.SetLocation( ITK_LOCATION );
    throw exp;
  }

  /** Compute the weights if only the weights name is set. */
  if ( this->m_WeightsName != "user_defined" )
  {
    this->InitializeWeights( this->m_WeightsName, k );
  }

  /** Compute the observation matrix. */
  this->ComputeConfusionMatrix( N, k );

  /** We are ready to compute the kappa statistic.
   * This is done in parts:
   * - calculate the row and column totals of the confusion matrix
   * - calculate Po and Pe
   */
  std::vector< double > row( k, 0.0 );
  std::vector< double > col( k, 0.0 );
  Po = Pe = 0.0;
  for ( unsigned int i = 0; i < k; ++i )
  {
    for ( unsigned int j = 0; j < k; ++j )
    {
      row[ i ] += this->m_ConfusionMatrix[ i ][ j ];
      col[ j ] += this->m_ConfusionMatrix[ i ][ j ];
    }
  }

  for ( unsigned int i = 0; i < k; ++i )
  {
    for ( unsigned int j = 0; j < k; ++j )
    {
      Po += this->m_Weights[ i ][ j ] * this->m_ConfusionMatrix[ i ][ j ];
      Pe += this->m_Weights[ i ][ j ] * row[ i ] * col[ j ];
    }
  }
  Po /= N;
  Pe /= N * N;

  // the above can probably be done in one loop over i and j,
  // but this is much better readable.

  /** Compute kappa. */
  kappa = ( Po - Pe ) / ( 1.0 - Pe );

} // end ComputeKappaStatisticValue()


/**
 * *************** ComputeKappaStatisticValueAndStandardDeviation ****************
 */

void CohenWeightedKappaStatistic
::ComputeKappaStatisticValueAndStandardDeviation(
  double & Po, double & Pe, double & kappa, double & std, const bool & compare )
{
  /** The observations has to be set previously by the user. */
  this->CheckObservations( this->m_Observations );

  /** Get some numbers. */
  unsigned int n = this->GetNumberOfObservers();
  unsigned int N = this->GetNumberOfObservations();
  unsigned int k = this->GetNumberOfCategories();

  /** Check if the weights are set. */
  if ( this->m_WeightsName == "" )
  {
    InvalidArgumentError exp(__FILE__, __LINE__);
    ::itk::OStringStream message;
    message << "itk::ERROR: " << this->GetNameOfClass()
      << "(" << this << "): "
      << "Weights not initialized.";
    exp.SetDescription( message.str() );
    exp.SetLocation( ITK_LOCATION );
    throw exp;
  }

  /** Compute the weights if only the weights name is set. */
  if ( this->m_WeightsName != "user_defined" )
  {
    this->InitializeWeights( this->m_WeightsName, k );
  }

  /** Compute the observation matrix. */
  this->ComputeConfusionMatrix( N, k );

  /** We are ready to compute the kappa statistic.
   * This is done in parts:
   * - calculate the row and column totals of the confusion matrix
   * - calculate Po and Pe
   */
  std::vector< double > row( k, 0.0 );
  std::vector< double > col( k, 0.0 );
  Po = Pe = kappa = std = 0.0;
  std::vector< double > barwi( k, 0.0 );
  std::vector< double > barwj( k, 0.0 );
  for ( unsigned int i = 0; i < k; ++i )
  {
    for ( unsigned int j = 0; j < k; ++j )
    {
      row[ i ] += this->m_ConfusionMatrix[ i ][ j ];
      col[ j ] += this->m_ConfusionMatrix[ i ][ j ];
    }
  }

  for ( unsigned int i = 0; i < k; ++i )
  {
    for ( unsigned int j = 0; j < k; ++j )
    {
      Po += this->m_Weights[ i ][ j ] * this->m_ConfusionMatrix[ i ][ j ];
      Pe += this->m_Weights[ i ][ j ] * row[ i ] * col[ j ];
      barwi[ i ] += this->m_Weights[ i ][ j ] * col[ j ];
      barwj[ i ] += this->m_Weights[ i ][ j ] * row[ j ];
    }
    barwi[ i ] /= N;
    barwj[ i ] /= N;
  }
  Po /= N;
  Pe /= N * N;

  // the above can probably be done in one loop over i and j,
  // but this is much better readable.

  /** Compute kappa. */
  kappa = ( Po - Pe ) / ( 1.0 - Pe );

  /** Compute the standard deviation. */
  double tmp = 0.0;
  for ( unsigned int i = 0; i < k; ++i )
  {
    for ( unsigned int j = 0; j < k; ++j )
    {
      if ( compare )
      {
        tmp += this->m_ConfusionMatrix[ i ][ j ]
          * ( this->m_Weights[ i ][ j ] - ( barwi[ i ] + barwj[ j ] ) * ( 1.0 - kappa ) )
          * ( this->m_Weights[ i ][ j ] - ( barwi[ i ] + barwj[ j ] ) * ( 1.0 - kappa ) );
      }
      else
      {
        tmp += row[ i ] * col[ j ]
          * ( this->m_Weights[ i ][ j ] - ( barwi[ i ] + barwj[ j ] ) )
          * ( this->m_Weights[ i ][ j ] - ( barwi[ i ] + barwj[ j ] ) );
      }
    }
  }
  if ( compare )
  {
    std = tmp / N;
    std -= ( kappa - Pe * ( 1.0 - kappa ) ) * ( kappa - Pe * ( 1.0 - kappa ) );
  }
  else
  {
    std = tmp / N / N;
    std -= Pe * Pe;
  }
  std /= N * ( 1.0 - Pe ) * ( 1.0 - Pe );
  std = vcl_sqrt( std );

} // end ComputeKappaStatisticValueAndStandardDeviation()


/**
 * *************** PrintSelf ****************
 */

void
CohenWeightedKappaStatistic
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  /** Print the confusion matrix. */
  unsigned int k = this->m_ConfusionMatrix.size();
  if ( k > 0 )
  {
    os << indent << "Confusion matrix:" << std::endl;
    for ( unsigned int i = 0; i < k; ++i )
    {
      os << indent;
      for ( unsigned int j = 0; j < k; ++j )
      {
        os << this->m_ConfusionMatrix[ i ][ j ] << " ";
      }
      os << std::endl;
    }
  }
  else
  {
    os << indent << "Confusion matrix: (unknown)" << std::endl;
  }

  /** Print the weight matrix. */
  k = this->m_Weights.size();
  if ( k > 0 )
  {
    os << indent << "Weight matrix:" << std::endl;
    for ( unsigned int i = 0; i < k; ++i )
    {
      os << indent;
      for ( unsigned int j = 0; j < k; ++j )
      {
        os << this->m_Weights[ i ][ j ] << " ";
      }
      os << std::endl;
    }
  }
  else
  {
    os << indent << "Weight matrix: (unknown)" << std::endl;
  }

} // end PrintSelf()


} // end of namespace Statistics
} // end namespace itk

