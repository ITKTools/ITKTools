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
#ifndef __meanstdimage_h_
#define __meanstdimage_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include <string>
#include <vector>


/** \class ITKToolsMeanStdImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsMeanStdImageBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsMeanStdImageBase()
  {
    this->m_InputFileNames = std::vector<std::string>();
    this->m_OutputFileNameMean = "";
    this->m_OutputFileNameStd = "";
    this->m_CalcMean = false;
    this->m_CalcStd = false;
  };
  /** Destructor. */
  ~ITKToolsMeanStdImageBase(){};

  /** Input member parameters. */
  std::vector<std::string> m_InputFileNames;
  std::string              m_OutputFileNameMean;
  std::string              m_OutputFileNameStd;
  bool                     m_CalcMean;
  bool                     m_CalcStd;

}; // end class ITKToolsMeanStdImageBase


/** \class ITKToolsMeanStdImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsMeanStdImage : public ITKToolsMeanStdImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsMeanStdImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsMeanStdImage(){};
  ~ITKToolsMeanStdImage(){};

  /** Typedef. */
  typedef itk::Image< TComponentType, VDimension >  InputImageType;
  typedef itk::Image< float, VDimension >           OutputImageType;

  /** Run function. */
  void Run( void )
  {
    this->MeanStdImage(
      this->m_InputFileNames,
      this->m_CalcMean,
      this->m_OutputFileNameMean,
      this->m_CalcStd,
      this->m_OutputFileNameStd );

  } // end Run()

  /** Function to perform normal thresholding. */
  void MeanStdImage(
    const std::vector<std::string> & inputFileNames,
    const bool calc_mean, const std::string & outputFileNameMean,
    const bool calc_std, const std::string & outputFileNameStd );

}; // end class MeanStdImage

#include "meanstdimage.hxx"

#endif // end #ifndef __meanstdimage_h_
