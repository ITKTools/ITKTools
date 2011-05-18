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
#include "castconverthelpers.h"


void ITKToolsCastConvertDICOM3D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert )
{
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< short, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< unsigned short, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< char, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< unsigned char, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< int, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< unsigned int, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< long, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< unsigned long, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< float, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvertDICOM< double, 3 >::New( outputComponentType, dim );

} // end ITKToolsCastConvertDICOM()
