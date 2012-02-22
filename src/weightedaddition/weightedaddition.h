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
#ifndef __weightedaddition_h_
#define __weightedaddition_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNaryAddImageFilter.h"
#include "itkMultiplyImageFilter.h"


/** \class ITKToolsWeightedAdditionBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsWeightedAdditionBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsWeightedAdditionBase()
  {
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsWeightedAdditionBase(){};

  /** Input member parameters. */
  std::vector<std::string> m_InputFileNames;
  std::vector<std::string> m_WeightFileNames;
  std::string m_OutputFileName;

}; // end class ITKToolsWeightedAdditionBase


/** \class ITKToolsWeightedAddition
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsWeightedAddition : public ITKToolsWeightedAdditionBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsWeightedAddition Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsWeightedAddition(){};
  ~ITKToolsWeightedAddition(){};

  /** Run function. */
  void Run( void )
  {
    /** TYPEDEF's. */
    typedef itk::Image< TComponentType, VDimension >      InputImageType;
    typedef itk::ImageFileReader< InputImageType >        ReaderType;
    typedef itk::MultiplyImageFilter<
      InputImageType, InputImageType, InputImageType>     MultiplierType;
    typedef itk::NaryAddImageFilter<
      InputImageType, InputImageType >                    AdderType;
    typedef itk::ImageFileWriter< InputImageType >        WriterType;
    typedef typename InputImageType::PixelType            PixelType;
    typedef typename ReaderType::Pointer                  ReaderPointer;
    typedef typename MultiplierType::Pointer              MultiplierPointer;
    typedef typename AdderType::Pointer                   AdderPointer;
    typedef typename WriterType::Pointer                  WriterPointer;

    /** DECLARATION'S. */
    unsigned int nrInputs = this->m_InputFileNames.size();
    if( this->m_WeightFileNames.size() != nrInputs )
    {
      itkGenericExceptionMacro( << "ERROR: Number of weight images does not equal number of input images!" );
    }

    std::vector< ReaderPointer > inReaders( nrInputs );
    std::vector< ReaderPointer > wReaders( nrInputs );
    std::vector< MultiplierPointer > multipliers( nrInputs );
    AdderPointer adder = AdderType::New();
    WriterPointer writer = WriterType::New();

    for( unsigned int i = 0; i < nrInputs; ++i )
    {
      inReaders[ i ] = ReaderType::New();
      inReaders[ i ]->SetFileName( this->m_InputFileNames[ i ].c_str() );
      wReaders[ i ] = ReaderType::New();
      wReaders[ i ]->SetFileName( this->m_WeightFileNames[ i ].c_str() );
      multipliers[ i ] = MultiplierType::New();
      multipliers[ i ]->SetInput(0, inReaders[ i ]->GetOutput() );
      multipliers[ i ]->SetInput(1, wReaders[ i ]->GetOutput() );
      multipliers[ i ]->InPlaceOn();
      adder->SetInput(i, multipliers[ i ]->GetOutput() );
    }

    /** Write the output image. */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( adder->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsWeightedAddition


#endif // end #ifndef __weightedaddition_h_
