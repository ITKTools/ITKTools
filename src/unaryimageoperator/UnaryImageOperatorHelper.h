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
#ifndef __UnaryImageOperatorHelper_h_
#define __UnaryImageOperatorHelper_h_

#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkUnaryFunctorImageFilter.h"
#include "itkUnaryFunctors.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <map>
#include <vector>


/** \class ITKToolsUnaryImageOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsUnaryImageOperatorBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsUnaryImageOperatorBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_UnaryOperatorName = "";
    this->m_Arguments.resize( 1, "" );
    this->m_UseCompression = false;
  };
  /** Destructor. */
  ~ITKToolsUnaryImageOperatorBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_UnaryOperatorName;
  std::vector<std::string> m_Arguments;
  bool m_UseCompression;

}; // end class ITKToolsUnaryImageOperatorBase


/** \class ITKToolsUnaryImageOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TInputComponentType,
  class TOutputComponentType = TInputComponentType>
class ITKToolsUnaryImageOperator : public ITKToolsUnaryImageOperatorBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsUnaryImageOperator Self;
  itktoolsTwoTypeNewMacro( Self );

  ITKToolsUnaryImageOperator(){};
  ~ITKToolsUnaryImageOperator(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TInputComponentType, VDimension>   InputImageType;
    typedef itk::Image<TOutputComponentType, VDimension>  OutputImageType;
    typedef typename InputImageType::PixelType            InputPixelType;
    typedef typename OutputImageType::PixelType           OutputPixelType;
    typedef itk::ImageFileReader< InputImageType >        ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >       WriterType;

    /** Read the image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );

    /** Define a helper map. */
    std::map< std::string, UnaryFunctorEnum> stringToEnumMap;
    stringToEnumMap["PLUS"] = PLUS;
    stringToEnumMap["RMINUS"] = RMINUS;
    stringToEnumMap["LMINUS"] = LMINUS;
    stringToEnumMap["TIMES"] = TIMES;
    stringToEnumMap["LDIVIDE"] = LDIVIDE;
    stringToEnumMap["RDIVIDE"] = RDIVIDE;
    stringToEnumMap["RMODINT"] = RMODINT;
    stringToEnumMap["RMODDOUBLE"] = RMODDOUBLE;
    stringToEnumMap["LMODINT"] = LMODINT;
    stringToEnumMap["LMODDOUBLE"] = LMODDOUBLE;
    stringToEnumMap["NLOG"] = NLOG;
    stringToEnumMap["RPOWER"] = RPOWER;
    stringToEnumMap["LPOWER"] = LPOWER;
    stringToEnumMap["NEG"] = NEG;
    stringToEnumMap["SIGNINT"] = SIGNINT;
    stringToEnumMap["SIGNDOUBLE"] = SIGNDOUBLE;
    stringToEnumMap["ABSINT"] = ABSINT;
    stringToEnumMap["ABSDOUBLE"] = ABSDOUBLE;
    stringToEnumMap["FLOOR"] = FLOOR;
    stringToEnumMap["CEIL"] = CEIL;
    stringToEnumMap["ROUND"] = ROUND;
    stringToEnumMap["LN"] = LN;
    stringToEnumMap["LOG10"] = LOG10;
    stringToEnumMap["EXP"] = EXP;
    stringToEnumMap["SIN"] = SIN;
    stringToEnumMap["COS"] = COS;
    stringToEnumMap["TAN"] = TAN;
    stringToEnumMap["ARCSIN"] = ARCSIN;
    stringToEnumMap["ARCCOS"] = ARCCOS;
    stringToEnumMap["ARCTAN"] = ARCTAN;
    stringToEnumMap["LINEAR"] = LINEAR;

    /** Construct the unary filter. */
    UnaryFunctorFactory<InputImageType, OutputImageType, double> unaryFunctorFactory;
    typename itk::InPlaceImageFilter<InputImageType, OutputImageType>::Pointer unaryFilter
      = unaryFunctorFactory.GetFilter( stringToEnumMap[ this->m_UnaryOperatorName ], this->m_Arguments );

    /** Connect the pipeline. */
    unaryFilter->SetInput( reader->GetOutput() );

    /** Write the image to disk */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( unaryFilter->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();

  } // end Run()

}; // end UnaryImageOperator

#endif //#ifndef __UnaryImageOperatorHelper_h
