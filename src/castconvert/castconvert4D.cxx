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
#include "castconvert.h"


void ITKToolsCastConvert4D(
  unsigned int dim,
  itk::ImageIOBase::IOComponentType componentType,
  ITKToolsCastConvertBase * & filter )
{
  if( !filter ) filter = ITKToolsCastConvert< 4, short >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, unsigned short >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, char >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, unsigned char >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, int >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, unsigned int >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, long >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, unsigned long >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, float >::New( dim, componentType );
  if( !filter ) filter = ITKToolsCastConvert< 4, double >::New( dim, componentType );

} // end ITKToolsCastConvert4D()
