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
#ifndef __reshape_h_
#define __reshape_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkReshapeImageToImageFilter.h"
#include "itkImageFileWriter.h"
#include <itksys/SystemTools.hxx>


/** \class ITKToolsUnaryImageOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsReshapeBase : public itktools::ITKToolsBase
{ 
public:
  /** Constructor. */
  ITKToolsReshapeBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsReshapeBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned long> m_OutputSize;
    
}; // end class ITKToolsReshapeBase


/** \class ITKToolsReshape
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsReshape : public ITKToolsReshapeBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsReshape Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsReshape(){};
  ~ITKToolsReshape(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef itk::ImageFileReader< ImageType >           ReaderType;
    typedef itk::ReshapeImageToImageFilter< ImageType > ReshapeFilterType;
    typedef itk::ImageFileWriter< ImageType >           WriterType;
    typedef typename ReshapeFilterType::SizeType        SizeType;

    /** Translate vector to SizeType. */
    SizeType size;
    for( unsigned int i = 0; i < this->m_OutputSize.size(); ++i )
    {
      size[ i ] = this->m_OutputSize[ i ];
    }

    /** Reader. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );

    /** Reshaper. */
    typename ReshapeFilterType::Pointer reshaper = ReshapeFilterType::New();
    reshaper->SetInput( reader->GetOutput() );
    reshaper->SetOutputSize( size );
    reshaper->Update();

    /** Writer. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( reshaper->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsReshape

#endif // end #ifndef __reshape_h_
