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
#ifndef __changeimageinformation_h_
#define __changeimageinformation_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkChangeInformationImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsChangeImageInformationBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsChangeImageInformationBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsChangeImageInformationBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_ReferenceFileName = "";
  };
  /** Destructor. */
  ~ITKToolsChangeImageInformationBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_ReferenceFileName;

}; // end ITKToolsChangeImageInformationBase


/** \class ITKToolsChangeImageInformation
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsChangeImageInformation : public ITKToolsChangeImageInformationBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsChangeImageInformation Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsChangeImageInformation(){};
  ~ITKToolsChangeImageInformation(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef TComponentType                                  PixelType;
    const unsigned int Dimension = VDimension;

    typedef itk::Image< PixelType, Dimension >              ImageType;
    typedef itk::ImageFileReader< ImageType >               ReaderType;
    typedef itk::ChangeInformationImageFilter< ImageType >  ChangeInfoFilterType;
    typedef itk::ImageFileWriter< ImageType >               WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer readerIn   = ReaderType::New();
    typename ReaderType::Pointer readerRef = ReaderType::New();
    typename ChangeInfoFilterType::Pointer changeFilter = ChangeInfoFilterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Setup and update readers. */
    readerIn->SetFileName( this->m_InputFileName );
    readerRef->SetFileName( this->m_ReferenceFileName );
    readerIn->Update();
    readerRef->Update();

    /** Setup filter. */
    changeFilter->SetInput( readerIn->GetOutput() );
    changeFilter->SetReferenceImage( readerRef->GetOutput() );
    changeFilter->SetUseReferenceImage( true );
    changeFilter->ChangeAll();

    /** Set up writer. */
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( changeFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end ITKToolsChangeImageInformation

#endif // end #ifndef __intensityreplace_h_
