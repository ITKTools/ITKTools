#ifndef __itkSimpleBoxSpatialFunction_txx
#define __itkSimpleBoxSpatialFunction_txx

#include "itkSimpleBoxSpatialFunction.h"

namespace itk
{

template <unsigned int VImageDimension,typename TInput>
SimpleBoxSpatialFunction<VImageDimension,TInput>
::SimpleBoxSpatialFunction()
{
  m_PointA.Fill(0.0);
  m_PointB.Fill(0.0);
}

template <unsigned int VImageDimension,typename TInput>
SimpleBoxSpatialFunction<VImageDimension,TInput>
::~SimpleBoxSpatialFunction()
{

}

template <unsigned int VImageDimension,typename TInput>
typename SimpleBoxSpatialFunction<VImageDimension,TInput>::OutputType
SimpleBoxSpatialFunction<VImageDimension,TInput>
::Evaluate(const InputType& position) const
{
  bool acc = true;

  for(unsigned int i = 0; i < VImageDimension; i++)
  {
    /** Check if the i'th coordinate of position lies within the two points. */
    acc &= (   ( (position[i] > m_PointA[i]) && (position[i] < m_PointB[i]) )
      || ( (position[i] > m_PointB[i]) && (position[i] < m_PointA[i]) )   );
  }

  if (acc)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

template <unsigned int VImageDimension,typename TInput>
void
SimpleBoxSpatialFunction<VImageDimension,TInput>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  unsigned int i;
  os << indent << "PointA: [";
  for (i=0; i < VImageDimension - 1; i++)
    {
    os << m_PointA[i] << ", ";
    }
  os << "]" << std::endl;
  
  os << indent << "PointB: [";
  for (i=0; i < VImageDimension - 1; i++)
    {
    os << m_PointB[i] << ", ";
    }
  os << "]" << std::endl;

}

} // end namespace itk

#endif
