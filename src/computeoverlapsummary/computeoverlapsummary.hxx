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
#ifndef __computeoverlapsummary_hxx
#define __computeoverlapsummary_hxx

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLabelOverlapMeasuresImageFilter.h"

template< class TComponentType, unsigned int VDimension >
void
ITKToolsComputeOverlapSummary< TComponentType, VDimension >
::ComputeOverlapSummary(
 const std::string & inputImage1, 
 const std::string & inputImage2, 
 const std::string & outputFileName,
 const std::string & seperator )
{
  typedef itk::ImageFileReader<InputImageType>  ReaderType;
  typename ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( inputImage1.c_str() );
  typename ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( inputImage2.c_str() );

  typedef itk::LabelOverlapMeasuresImageFilter<InputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetSourceImage( reader1->GetOutput() );
  filter->SetTargetImage( reader2->GetOutput() );
  filter->Update();

  FILE * pFile;
  pFile = fopen ( outputFileName.c_str(), "w");

  if (pFile==NULL) {
    throw invalidfilexception();
  }

  fprintf ( pFile, "%s%s%s%s%s%s%s%s%s%s%s\n", "Label", seperator, "Union (jaccard)", seperator, "Mean (dice)", seperator, "Volume sim.", seperator, "False negative", seperator, "False positive");
  fprintf ( pFile, "%s%s%f%s%f%s%f%s%f%s%f\n", "Total", seperator, filter->GetUnionOverlap(), seperator, filter->GetMeanOverlap(), seperator, filter->GetVolumeSimilarity(), seperator, filter->GetFalseNegativeError(), seperator, filter->GetFalsePositiveError() );

  typename FilterType::MapType labelMap = filter->GetLabelSetMeasures();
  typename FilterType::MapType::const_iterator it;
  for( it = labelMap.begin(); it != labelMap.end(); ++it )
  {
    if( (*it).first == 0 )
    {
      continue;
    }

    int label = (*it).first;
    fprintf ( pFile, "%i%s%f%s%f%s%f%s%f%s%f\n", label, seperator, filter->GetUnionOverlap( label ), seperator, filter->GetMeanOverlap( label ), seperator, filter->GetVolumeSimilarity( label ), seperator, filter->GetFalseNegativeError( label ), seperator, filter->GetFalsePositiveError( label ) );
  }
  fclose (pFile);
} // end ComputeOverlapSummary()

#endif // end #ifndef __computeoverlapsummary_hxx
