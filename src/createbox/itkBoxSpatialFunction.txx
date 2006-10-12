#ifndef __itkBoxSpatialFunction_txx
#define __itkBoxSpatialFunction_txx

#include "itkBoxSpatialFunction.h"
#include <math.h>

namespace itk
{

template <unsigned int VImageDimension,typename TInput>
BoxSpatialFunction<VImageDimension,TInput>
::BoxSpatialFunction()
{
  this->m_Center.Fill( 0.0 );
  this->m_Radius.Fill( 1.0 );
  this->m_Orientation.Fill( 0.0 );
  this->m_Euler2DTransform = Euler2DTransformType::New();
  this->m_Euler3DTransform = Euler3DTransformType::New();
} // end constructor


template <unsigned int VImageDimension,typename TInput>
BoxSpatialFunction<VImageDimension,TInput>
::~BoxSpatialFunction()
{
} // end destructor


template <unsigned int VImageDimension,typename TInput>
typename BoxSpatialFunction<VImageDimension,TInput>::OutputType
BoxSpatialFunction<VImageDimension,TInput>
::Evaluate( const InputType& position ) const
{
  /** Apply an inverse rotation / orientation to the position. */
  InputType mappedPosition;
  if ( ImageDimension == 2 )
  {
    InputPointType2D point;
    OutputPointType2D outputpoint;
    for( unsigned int i = 0; i < ImageDimension; i++ ) point[ i ] = position[ i ];
    typename Euler2DTransformType::Pointer euler = Euler2DTransformType::New();
    this->m_Euler2DTransform->GetInverse( euler );
    outputpoint = euler->TransformPoint( point );
    for( unsigned int i = 0; i < ImageDimension; i++ ) mappedPosition[ i ] = outputpoint[ i ];
  }
  else if ( ImageDimension == 3 )
  {
    InputPointType3D point;
    OutputPointType3D outputpoint;
    for( unsigned int i = 0; i < ImageDimension; i++ ) point[ i ] = position[ i ];
    typename Euler3DTransformType::Pointer euler = Euler3DTransformType::New();
    this->m_Euler3DTransform->GetInverse( euler );
    outputpoint = euler->TransformPoint( point );
    for( unsigned int i = 0; i < ImageDimension; i++ ) mappedPosition[ i ] = outputpoint[ i ];
  }

  /** Check if mappedPoint is within the box. */
  bool acc = true;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    acc &= abs( mappedPosition[ i ] - this->m_Center[ i ] ) < this->m_Radius[ i ];
  }

  /** Return a value. */
  if ( acc ) return 1;
  else return 0;

} // end Evaluate


template <unsigned int VImageDimension,typename TInput>
void
BoxSpatialFunction<VImageDimension,TInput>
::SetOrientation( const InputType _arg )
{
  if ( this->m_Orientation != _arg )
  {
    this->m_Orientation = _arg;

    if ( ImageDimension == 2 )
    {
      this->m_Euler2DTransform->SetAngle( this->m_Orientation[ 0 ] );
    }
    else if ( ImageDimension == 3 )
    {
      this->m_Euler3DTransform->SetRotation( this->m_Orientation[ 0 ],
        this->m_Orientation[ 1 ], this->m_Orientation[ 2 ] );
    }

    this->Modified();
  }

} // end SetOrientation


template <unsigned int VImageDimension,typename TInput>
void
BoxSpatialFunction<VImageDimension,TInput>
::SetCenter( const InputType _arg )
{
  if ( this->m_Center != _arg )
  {
    this->m_Center = _arg;

    if ( ImageDimension == 2 )
    {
      InputPointType2D center;
      for( unsigned int i = 0; i < ImageDimension; i++ ) center[ i ] = this->m_Center[ i ];
      this->m_Euler2DTransform->SetCenter( center );
    }
    else if ( ImageDimension == 3 )
    {
      InputPointType3D center;
      for( unsigned int i = 0; i < ImageDimension; i++ ) center[ i ] = this->m_Center[ i ];
      this->m_Euler3DTransform->SetCenter( center );
    }

    this->Modified();
  }

} // end SetCenter


template <unsigned int VImageDimension,typename TInput>
void
BoxSpatialFunction<VImageDimension,TInput>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Radius: " << this->m_Radius << std::endl;
  os << indent << "Center: " << this->m_Center << std::endl;
  os << indent << "Orientation: " << this->m_Orientation << std::endl;

} // end PrintSelf

} // end namespace itk

#endif // end #ifndef __itkBoxSpatialFunction_txx
