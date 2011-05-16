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
#ifndef __UnaryImageOperatorHelper_h
#define __UnaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkUnaryFunctorImageFilter.h"
//#include <map>
#include <vector>
#include <itksys/SystemTools.hxx>

#include "itkUnaryFunctors.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "ITKToolsBase.h"

//-------------------------------------------------------------------------------------


/** UnaryImageOperator */

class UnaryImageOperatorBase : public itktools::ITKToolsBase
{ 
public:
  UnaryImageOperatorBase(){};
  ~UnaryImageOperatorBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_Ops;
  std::string m_Arg;
    
}; // end UnaryImageOperatorBase


template< class TInputComponentType, class TOutputComponentType, unsigned int VDimension >
class UnaryImageOperator : public UnaryImageOperatorBase
{
public:
  typedef UnaryImageOperator Self;

  UnaryImageOperator(){};
  ~UnaryImageOperator(){};

  static Self * New( itktools::EnumComponentType inputComponentType,
		     itktools::EnumComponentType outputComponentType, unsigned int dim )
  {
    if ( itktools::IsType<TInputComponentType>( inputComponentType ) &&
         itktools::IsType<TOutputComponentType>( outputComponentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs. */
    typedef itk::Image<TInputComponentType, VDimension>       InputImageType;
    typedef itk::Image<TOutputComponentType, VDimension>      OutputImageType;
    typedef typename InputImageType::PixelType          InputPixelType;
    typedef typename OutputImageType::PixelType         OutputPixelType;
    
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >     WriterType;

    /** Read the image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( m_InputFileName.c_str() );

    /** Get the ArgumentType and argument. */
    std::string argumentType = "int";
    std::basic_string<char>::size_type pos = m_Arg.find( "." );
    const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
    if ( pos != npos )
    {
      argumentType = "double";
    }

    /** Get the unaryOperatorName. */
    std::string unaryOperatorName = m_Ops;

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

    UnaryFunctorFactory<InputImageType, OutputImageType> unaryFunctorFactory;
    typename itk::InPlaceImageFilter<InputImageType, OutputImageType>::Pointer unaryFilter = unaryFunctorFactory.GetFilter(stringToEnumMap[m_Ops], m_Arg);
    
    /** Connect the pipeline. */
    unaryFilter->SetInput( reader->GetOutput() );
    //unaryFilter->SetArgument( argument ); // !!! how to do this since the unaryFilter pointer is the type of the superclass?
    
    /** Write the image to disk */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( unaryFilter->GetOutput() );
    writer->Update();
  }

}; // end UnaryImageOperator

//-------------------------------------------------------------------------------------


#endif //#ifndef __UnaryImageOperatorHelper_h
