/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCylinderSpatialFunction.txx,v $
  Language:  C++
  Date:      $Date: 2003-09-10 14:29:26 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCylinderSpatialFunction_txx
#define __itkCylinderSpatialFunction_txx

#include "itkCylinderSpatialFunction.h"

namespace itk
{

template <unsigned int VImageDimension,typename TInput>
CylinderSpatialFunction<VImageDimension,TInput>
::CylinderSpatialFunction()
{
  this->m_Radius = 1.0;
  this->m_Center.Fill(0.0);
}

template <unsigned int VImageDimension,typename TInput>
CylinderSpatialFunction<VImageDimension,TInput>
::~CylinderSpatialFunction()
{

}

template <unsigned int VImageDimension,typename TInput>
typename CylinderSpatialFunction<VImageDimension,TInput>::OutputType
CylinderSpatialFunction<VImageDimension,TInput>
::Evaluate(const InputType& position) const
{
  double acc = 0;

  for(unsigned int i = 0; i < VImageDimension - 1; i++)
    {
    acc += (position[i] - this->m_Center[i]) * (position[i] - this->m_Center[i]);
    }

  acc -= this->m_Radius*m_Radius;

  if(acc <= 0) // inside the sphere
    {
    return 1;
    }
  else
    {
    return 0; // outside the sphere
    }
}

template <unsigned int VImageDimension,typename TInput>
void
CylinderSpatialFunction<VImageDimension,TInput>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  unsigned int i;
  os << indent << "Center: [";
  for (i=0; i < VImageDimension - 1; i++)
    {
    os << this->m_Center[i] << ", ";
    }
  os << "]" << std::endl;

  os << indent << "Radius: " << this->m_Radius << std::endl;
}

} // end namespace itk

#endif
