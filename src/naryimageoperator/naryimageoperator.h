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
#ifndef __naryimageoperator_h_
#define __naryimageoperator_h_

#include "itkImage.h"
#include "itkNaryFunctors.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "NaryFilterFactory.h"

#include <vector>
#include <itksys/SystemTools.hxx>


/** \class ITKToolsUnaryImageOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsNaryImageOperatorBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsNaryImageOperatorBase()
  {
    this->m_OutputFileName = "";
    this->m_NaryOperatorName = "";
    this->m_UseCompression = false;
    this->m_NumberOfStreams = 0;
    this->m_Arg = "";
  };
  /** Destructor. */
  ~ITKToolsNaryImageOperatorBase(){};

  /** Input member parameters. */
  std::vector<std::string> m_InputFileNames;
  std::string       m_OutputFileName;
  std::string       m_NaryOperatorName;
  bool              m_UseCompression;
  unsigned int      m_NumberOfStreams;
  std::string       m_Arg;

}; // end class ITKToolsNaryImageOperatorBase


/** \class ITKToolsNaryImageOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TInputComponentType, class TOutputComponentType >
class ITKToolsNaryImageOperator : public ITKToolsNaryImageOperatorBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsNaryImageOperator Self;
  itktoolsTwoTypeNewMacro( Self );

  ITKToolsNaryImageOperator(){};
  ~ITKToolsNaryImageOperator(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TInputComponentType, VDimension >   InputImageType;
    typedef itk::Image< TOutputComponentType, VDimension >  OutputImageType;
    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >         WriterType;

    /** Read the input images. */
    std::vector<typename ReaderType::Pointer> readers( this->m_InputFileNames.size() );
    for ( unsigned int i = 0; i < this->m_InputFileNames.size(); ++i )
    {
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( this->m_InputFileNames[ i ] );
    }

    std::map <std::string, NaryFilterEnum> naryOperatorMap;

    naryOperatorMap["ADDITION"] = ADDITION;
    naryOperatorMap["MEAN"] = MEAN;
    naryOperatorMap["MINUS"] = MINUS;
    naryOperatorMap["TIMES"] = TIMES;
    naryOperatorMap["DIVIDE"] = DIVIDE;
    naryOperatorMap["MAXIMUM"] = MAXIMUM;
    naryOperatorMap["MINIMUM"] = MINIMUM;
    naryOperatorMap["ABSOLUTEDIFFERENCE"] = ABSOLUTEDIFFERENCE;
    naryOperatorMap["NARYMAGNITUDE"] = NARYMAGNITUDE;

    /** Set up the binaryFilter. */
    NaryFilterFactory< InputImageType, OutputImageType > naryFilterFactory;
    typedef itk::InPlaceImageFilter< InputImageType, OutputImageType > BaseFilterType;
    typename BaseFilterType::Pointer naryFilter
      = naryFilterFactory.GetFilter( naryOperatorMap[ this->m_NaryOperatorName ] );

    //InstantiateNaryFilterNoArg( POWER );
    //InstantiateNaryFilterNoArg( SQUAREDDIFFERENCE );
    //InstantiateNaryFilterNoArg( MODULO );
    //InstantiateNaryFilterNoArg( LOG );

    //InstantiateNaryFilterWithArg( WEIGHTEDADDITION );
    //InstantiateNaryFilterWithArg( MASK );
    //InstantiateNaryFilterWithArg( MASKNEGATED );

    /** Connect the pipeline. */
    for( unsigned int i = 0; i < this->m_InputFileNames.size(); ++i )
    {
      naryFilter->SetInput( i, readers[ i ]->GetOutput() );
    }

    /** Write the image to disk */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( naryFilter->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->SetNumberOfStreamDivisions( this->m_NumberOfStreams );
    writer->Update();

  } // end Run()

}; // end class ITKToolsNaryImageOperator


#endif // end #ifndef __naryimageoperator_h_
