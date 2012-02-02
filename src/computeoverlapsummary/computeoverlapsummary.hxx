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

  fprintf ( pFile, "%s%s%s%s%s%s%s%s%s%s%s\n", "Label", seperator.c_str(), "Union (jaccard)", seperator.c_str(), "Mean (dice)", seperator.c_str(), "Volume sim.", seperator.c_str(), "False negative", seperator.c_str(), "False positive");
  fprintf ( pFile, "%s%s%f%s%f%s%f%s%f%s%f\n", "Total", seperator.c_str(), filter->GetUnionOverlap(), seperator.c_str(), filter->GetMeanOverlap(), seperator.c_str(), filter->GetVolumeSimilarity(), seperator.c_str(), filter->GetFalseNegativeError(), seperator.c_str(), filter->GetFalsePositiveError() );

  typename FilterType::MapType labelMap = filter->GetLabelSetMeasures();
  typename FilterType::MapType::const_iterator it;
  for( it = labelMap.begin(); it != labelMap.end(); ++it )
  {
    if( (*it).first == 0 )
    {
      continue;
    }

    int label = (*it).first;
    fprintf ( pFile, "%i%s%f%s%f%s%f%s%f%s%f\n", label, seperator.c_str(), filter->GetUnionOverlap( label ), seperator.c_str(), filter->GetMeanOverlap( label ), seperator.c_str(), filter->GetVolumeSimilarity( label ), seperator.c_str(), filter->GetFalseNegativeError( label ), seperator.c_str(), filter->GetFalsePositiveError( label ) );
  }
  fclose (pFile);
} // end ComputeOverlapSummary()

#endif // end #ifndef __computeoverlapsummary_hxx
