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
#ifndef __itkSimpleBoxSpatialFunction_txx_
#define __itkSimpleBoxSpatialFunction_txx_

#include "itkSimpleBoxSpatialFunction.h"

namespace itk
{

template <unsigned int VImageDimension,typename TInput>
SimpleBoxSpatialFunction<VImageDimension,TInput>
::SimpleBoxSpatialFunction()
{
  this->m_PointA.Fill(0.0);
  this->m_PointB.Fill(0.0);
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

  for( unsigned int i = 0; i < VImageDimension; i++ )
  {
    /** Check if the i'th coordinate of position lies within the two points. */
    acc &= (   ( (position[ i ] > this->m_PointA[ i ]) && (position[ i ] < this->m_PointB[ i ]) )
      || ( (position[ i ] > this->m_PointB[ i ]) && (position[ i ] < this->m_PointA[ i ]) )   );
  }

  if(acc)
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
  for (i=0; i < VImageDimension - 1; i++ )
    {
    os << this->m_PointA[ i ] << ", ";
    }
  os << "]" << std::endl;

  os << indent << "PointB: [";
  for (i=0; i < VImageDimension - 1; i++ )
    {
    os << this->m_PointB[ i ] << ", ";
    }
  os << "]" << std::endl;

}

} // end namespace itk

#endif // end #ifndef __itkSimpleBoxSpatialFunction_txx_
