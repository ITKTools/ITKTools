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
#ifndef __computeoverlapsummary_h
#define __computeoverlapsummary_h

#include "ITKToolsBase.h"
#include "itkImage.h"
#include <string>
#include <vector>

/** ComputeOverlapSummary */

class ITKToolsComputeOverlapSummaryBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsComputeOverlapSummaryBase()
  {
    this->m_InputFileName1 = "";
    this->m_InputFileName2 = "";
    this->m_OutputFileName = "";
    this->m_Seperator      = "\t";
  };

  ~ITKToolsComputeOverlapSummaryBase(){};

  /** Input parameters */
  std::string m_InputFileName1;
  std::string m_InputFileName2;
  std::string m_OutputFileName;
  std::string m_Seperator;
}; // end class ComputeOverlapSummaryBase

class invalidfilexception: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Could not open output file stream for writing!";
	}
};

template< class TComponentType, unsigned int VDimension >
class ITKToolsComputeOverlapSummary : public ITKToolsComputeOverlapSummaryBase
{
public:
  typedef ITKToolsComputeOverlapSummary Self;

  ITKToolsComputeOverlapSummary(){};
  ~ITKToolsComputeOverlapSummary(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  /** Typedef. */
  typedef itk::Image< TComponentType, VDimension > InputImageType;

  /** Main function Run(). */
  void Run( void )
  {
    this->ComputeOverlapSummary(
      this->m_InputFileName1,
      this->m_InputFileName2,
      this->m_OutputFileName,
      this->m_Seperator);
  } // end Run()


  /** Function to perform normal thresholding. */
  void ComputeOverlapSummary(
    const std::string & inputImage1,
    const std::string & inputImage2,
    const std::string & outputFileName,
    const std::string & seperator);

}; // end class ComputeOverlapSummary

#include "computeoverlapsummary.hxx"

#endif // end #ifndef __computeoverlapsummary_h

