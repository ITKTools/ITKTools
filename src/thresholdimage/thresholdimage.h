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
#ifndef __thresholdimage_h_
#define __thresholdimage_h_

#include "ITKToolsBase.h"
#include "itkImage.h"
#include <string>


/** \class ITKToolsThresholdImageBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsThresholdImageBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsThresholdImageBase()
  {
    this->m_Bins = 0;
    this->m_InputFileName = "";
    this->m_Inside = 0.0f;
    this->m_Iterations = 0;
    this->m_MaskFileName = "";
    this->m_MaskValue = 0;
    this->m_Method = "";
    this->m_MixtureType = 0;
    this->m_NumThresholds = 0;
    this->m_OutputFileName = "";
    this->m_Outside = 0.0f;
    this->m_Pow = 0.0f;
    this->m_Sigma = 0.0f;
    this->m_Supported = false;
    this->m_Threshold1 = 0.0f;
    this->m_Threshold2 = 0.0f;
    this->m_UseCompression = false;
  };
  /** Destructor. */
  ~ITKToolsThresholdImageBase(){};

  /** Input member parameters. */
  std::string   m_InputFileName;
  std::string   m_OutputFileName;
  std::string   m_MaskFileName;

  std::string m_Method;

  unsigned int m_NumThresholds;
  double m_Threshold1;
  double m_Threshold2;
  double m_Inside;
  double m_Outside;

  unsigned int  m_Bins;
  unsigned int  m_Iterations;
  unsigned int  m_MaskValue;
  unsigned int  m_MixtureType;
  
  double        m_Pow;
  double        m_Sigma;
  bool          m_Supported;
  bool          m_UseCompression;

}; // end class ITKToolsThresholdImageBase


/** \class ITKToolsThresholdImage
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsThresholdImage : public ITKToolsThresholdImageBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsThresholdImage Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsThresholdImage(){};
  ~ITKToolsThresholdImage(){};

  /** Typedef. */
  typedef itk::Image< TComponentType, VDimension > InputImageType;

  /** Run function. */
  void Run( void )
  {
    if( this->m_Method == "Threshold" )
    {
      this->ThresholdImage(
        this->m_InputFileName, this->m_OutputFileName,
        this->m_Inside, this->m_Outside,
        this->m_Threshold1, this->m_Threshold2,
        this->m_UseCompression );
    }
    else if( this->m_Method == "OtsuThreshold" )
    {
      this->OtsuThresholdImage(
        this->m_InputFileName, this->m_OutputFileName, this->m_MaskFileName,
        this->m_Inside, this->m_Outside,
        this->m_Bins,
        this->m_UseCompression );
    }
    else if( this->m_Method == "OtsuMultipleThreshold" )
    {
      this->OtsuMultipleThresholdImage(
        this->m_InputFileName, this->m_OutputFileName, this->m_MaskFileName,
        this->m_Inside, this->m_Outside,
        this->m_Bins, this->m_NumThresholds,
        this->m_UseCompression );
    }
    else if( this->m_Method == "RobustAutomaticThreshold" )
    {
      this->RobustAutomaticThresholdImage(
        this->m_InputFileName, this->m_OutputFileName,
        this->m_Inside, this->m_Outside,
        this->m_Pow,
        this->m_UseCompression );
    }
    else if( this->m_Method == "KappaSigmaThreshold" )
    {
      this->KappaSigmaThresholdImage(
        this->m_InputFileName, this->m_OutputFileName, this->m_MaskFileName,
        this->m_Inside, this->m_Outside,
        this->m_MaskValue, this->m_Sigma, this->m_Iterations,
        this->m_UseCompression );
    }
    else if( this->m_Method == "MinErrorThreshold" )
    {
      this->MinErrorThresholdImage(
        this->m_InputFileName, this->m_OutputFileName,
        this->m_Inside, this->m_Outside,
        this->m_Bins, this->m_MixtureType,
        this->m_UseCompression );
    }
    else
    {
      std::cerr << "Not supported!" << std::endl;
      return;
    }
  } // end Run()

  /** Function to perform normal thresholding. */
  void ThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const double & inside, const double & outside,
    const double & threshold1, const double & threshold2,
    const bool & useCompression );

  /** Function to perform Otsu thresholding. */
  void OtsuThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const std::string & maskFileName,
    const double & inside, const double & outside,
    const unsigned int & bins,
    const bool & useCompression );

  /** Function to perform Otsu thresholding with multiple thresholds. */
  void OtsuMultipleThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const std::string & itkNotUsed( maskFileName ),
    const double & inside, const double & outside,
    const unsigned int & bins, const unsigned int & numThresholds,
    const bool & useCompression );

  /** Function to perform Otsu thresholding with an adaptive threshold. */
//   void AdaptiveOtsuThresholdImage(
//     const std::string & inputFileName, const std::string & outputFileName,
//     const unsigned int & radius, const unsigned int & bins,
//     const unsigned int & controlPoints, const unsigned int & levels,
//     const unsigned int & samples, const unsigned int & splineOrder );

  /** Function to perform thresholding using .. . */
  void RobustAutomaticThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const double & inside, const double & outside,
    const double & pow, const bool & useCompression );

  /** Function to perform thresholding using ... . */
  void KappaSigmaThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const std::string & maskFileName,
    const double & inside, const double & outside, const unsigned int & maskValue,
    const double & sigma, const unsigned int & iterations,
    const bool & useCompression );

  /** Function to perform thresholding using .. . */
  void MinErrorThresholdImage(
    const std::string & inputFileName, const std::string & outputFileName,
    const double & inside, const double & outside,
    const unsigned int & bins, const unsigned int & mixtureType,
    const bool & useCompression );

}; // end class ITKToolsThresholdImage

#include "thresholdimage.hxx"

#endif // end #ifndef __thresholdimage_h
