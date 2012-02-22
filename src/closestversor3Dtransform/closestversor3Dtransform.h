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
#ifndef __closestversor3Dtransform_h_
#define __closestversor3Dtransform_h_

#include "itkImage.h"
#include "itkPoint.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkVersorRigid3DTransform.h"

#include "vnl/vnl_math.h"

#include <iostream>
#include <fstream>
#include <iomanip>


/**
 * ******************* ReadLandmarks *******************
 */

void ReadLandmarks(
  std::string landmarkFileName,
  std::vector< itk::Point<double,3> > & landmarkContainer )
{
  /** Typedef's. */
  const unsigned int Dimension = 3;
  typedef itk::Image< short, Dimension >              ImageType;
  typedef itk::VersorRigid3DTransform< double >       TransformType;
  typedef TransformType::ParametersType               ParametersType;
  typedef itk::LandmarkBasedTransformInitializer<
    TransformType, ImageType, ImageType >             EstimatorType;
  typedef EstimatorType::LandmarkPointType            LandmarkType;
  typedef EstimatorType::LandmarkPointContainer       LandmarkContainer;

  /** Open file for reading and read landmarks. */
  std::ifstream landmarkFile( landmarkFileName.c_str() );
  if( landmarkFile.is_open() )
  {
    LandmarkType landmark;
    while ( !landmarkFile.eof() )
    {
      for( unsigned int i = 0; i < Dimension; i++ )
      {
        landmarkFile >> landmark[ i ];
      }
      landmarkContainer.push_back( landmark );
    }
  }
  landmarkFile.close();
  landmarkContainer.pop_back();

} // end ReadLandMarks()


/**
 * ******************* ComputeClosestVersor *******************
 */

void ComputeClosestVersor(
  std::string fixedLandmarkFileName,
  std::string movingLandmarkFileName,
  std::vector<double> & parameters,
  std::vector<double> & centerOfRotation )
{
  /** Some consts. */
  const unsigned int  Dimension = 3;
  typedef short PixelType;

  /** Typedefs. */
  typedef itk::Image< PixelType, Dimension >          ImageType;
  typedef itk::VersorRigid3DTransform< double >       TransformType;
  typedef TransformType::ParametersType               ParametersType;
  typedef TransformType::CenterType                   CenterType;
  typedef itk::LandmarkBasedTransformInitializer<
    TransformType, ImageType, ImageType >             EstimatorType;
  typedef EstimatorType::LandmarkPointType            LandmarkType;
  typedef EstimatorType::LandmarkPointContainer       LandmarkContainer;

  /** Read the fixed landmark points. */
  LandmarkContainer fixedLandmarkContainer;
  ReadLandmarks( fixedLandmarkFileName, fixedLandmarkContainer );

  /** Read the moving landmark points. */
  LandmarkContainer movingLandmarkContainer;
  ReadLandmarks( movingLandmarkFileName, movingLandmarkContainer );

  /** Check the sizes. */
  if( fixedLandmarkContainer.size() != movingLandmarkContainer.size() )
  {
    std::cerr << "ERROR: the two sets of landmarks are not of the same size." << std::endl;
    return;
  }

  /** Create transform. */
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

  /** Create estimator. */
  EstimatorType::Pointer estimator = EstimatorType::New();
  estimator->SetTransform( transform );
  estimator->SetFixedLandmarks(  fixedLandmarkContainer );
  estimator->SetMovingLandmarks( movingLandmarkContainer );

  /** Run. */
  estimator->InitializeTransform();

  /** Get the parameters of the estimated closest rigid transformation. */
  ParametersType params = transform->GetParameters();
  unsigned int nop = transform->GetNumberOfParameters();
  parameters.resize( nop, 0.0 );
  for( unsigned int i = 0; i < nop; ++i )
  {
    parameters[ i ] = params[ i ];
  }

  /** Get the estimated center of rotation. */
  CenterType center = transform->GetCenter();
  centerOfRotation.resize( Dimension, 0.0 );
  for( unsigned int i = 0; i < Dimension; ++i )
  {
    centerOfRotation[ i ] = center[ i ];
  }

} // end ComputeClosestVersor()


/**
 * ******************* ConvertVersorToEuler *******************
 */

void ConvertVersorToEuler(
  const std::vector<double> & parVersor,
  std::vector<double> & parEuler )
{
  /** Create an Euler parameter vector. */
  unsigned int nop = parVersor.size();
  if( nop != 6 ) return;
  parEuler.resize( nop, 0.0 );

  /** Easy notation. */
  double q0 = vcl_sqrt( 1.0 - parVersor[ 0 ] * parVersor[ 0 ]
    - parVersor[ 1 ] * parVersor[ 1 ] - parVersor[ 2 ] * parVersor[ 2 ] );
  double q1 = parVersor[ 0 ];
  double q2 = parVersor[ 1 ];
  double q3 = parVersor[ 2 ];

  /** Computer Euler angles. */
  parEuler[ 0 ] = vcl_atan2( 2.0 * ( q0 * q1 + q2 * q3 ), 1.0 - 2.0 * ( q1 * q1 + q2 * q2 ) );
  parEuler[ 1 ] = vcl_asin( 2.0 * ( q0 * q2 - q3 * q1 ) );
  parEuler[ 2 ] = vcl_atan2( 2.0 * ( q0 * q3 + q1 * q2 ), 1.0 - 2.0 * ( q2 * q2 + q3 * q3 ) );
  parEuler[ 3 ] = parVersor[ 3 ];
  parEuler[ 4 ] = parVersor[ 4 ];
  parEuler[ 5 ] = parVersor[ 5 ];

} // end ConvertVersorToEuler()

#endif // end #ifndef __closestversor3Dtransform_h_
