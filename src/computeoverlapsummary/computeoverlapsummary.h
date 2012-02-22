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
#ifndef __computeoverlapsummary_h_
#define __computeoverlapsummary_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include <string>
#include <vector>

/** \class ITKToolsComputeOverlapSummaryBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsComputeOverlapSummaryBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsComputeOverlapSummaryBase()
  {
    this->m_InputFileName1 = "";
    this->m_InputFileName2 = "";
    this->m_OutputFileName = "";
    this->m_Seperator      = "\t";
  };
  /** Destructor. */
  ~ITKToolsComputeOverlapSummaryBase(){};

  /** Input member parameters. */
  std::string m_InputFileName1;
  std::string m_InputFileName2;
  std::string m_OutputFileName;
  std::string m_Seperator;

}; // end class ITKToolsComputeOverlapSummaryBase

class invalidfilexception: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Could not open output file stream for writing!";
	}
};


/** \class ITKToolsComputeOverlapSummary
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsComputeOverlapSummary : public ITKToolsComputeOverlapSummaryBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsComputeOverlapSummary Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsComputeOverlapSummary(){};
  ~ITKToolsComputeOverlapSummary(){};

  /** Run function. */
  void Run( void )
  {
    typedef itk::Image< TComponentType, VDimension >  InputImageType;
    typedef itk::ImageFileReader<InputImageType>      ReaderType;
    typedef itk::LabelOverlapMeasuresImageFilter<InputImageType> FilterType;

    typename ReaderType::Pointer reader1 = ReaderType::New();
    reader1->SetFileName( this->m_InputFileName1.c_str() );
    typename ReaderType::Pointer reader2 = ReaderType::New();
    reader2->SetFileName( this->m_InputFileName2.c_str() );
   
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetSourceImage( reader1->GetOutput() );
    filter->SetTargetImage( reader2->GetOutput() );
    filter->Update();

    FILE * pFile;
    pFile = fopen( this->m_OutputFileName.c_str(), "w" );

    if( pFile == NULL )
    {
      throw invalidfilexception();
    }

    fprintf( pFile, "%s%s%s%s%s%s%s%s%s%s%s\n",
      "Label", this->m_Seperator.c_str(),
      "Union (jaccard)", this->m_Seperator.c_str(),
      "Mean (dice)", this->m_Seperator.c_str(),
      "Volume sim.", this->m_Seperator.c_str(),
      "False negative", this->m_Seperator.c_str(),
      "False positive" );
    fprintf( pFile, "%s%s%f%s%f%s%f%s%f%s%f\n",
      "Total", this->m_Seperator.c_str(),
      filter->GetUnionOverlap(), this->m_Seperator.c_str(),
      filter->GetMeanOverlap(), this->m_Seperator.c_str(),
      filter->GetVolumeSimilarity(), this->m_Seperator.c_str(),
      filter->GetFalseNegativeError(), this->m_Seperator.c_str(),
      filter->GetFalsePositiveError() );

    typename FilterType::MapType labelMap = filter->GetLabelSetMeasures();
    typename FilterType::MapType::const_iterator it;
    for( it = labelMap.begin(); it != labelMap.end(); ++it )
    {
      if( (*it).first == 0 )
      {
        continue;
      }

      int label = (*it).first;
      fprintf( pFile, "%i%s%f%s%f%s%f%s%f%s%f\n",
        label, this->m_Seperator.c_str(),
        filter->GetUnionOverlap( label ), this->m_Seperator.c_str(),
        filter->GetMeanOverlap( label ), this->m_Seperator.c_str(),
        filter->GetVolumeSimilarity( label ), this->m_Seperator.c_str(),
        filter->GetFalseNegativeError( label ), this->m_Seperator.c_str(),
        filter->GetFalsePositiveError( label ) );
    }
    fclose ( pFile );

  } // end Run()

}; // end class ITKToolsComputeOverlapSummary

#endif // end #ifndef __computeoverlapsummary_h_
