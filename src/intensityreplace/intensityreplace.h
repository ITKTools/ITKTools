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
#ifndef __intensityreplace_h_
#define __intensityreplace_h_

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkChangeLabelImageFilter.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsIntensityReplaceBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsIntensityReplaceBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsIntensityReplaceBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsIntensityReplaceBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<std::string> m_InValues;
  std::vector<std::string> m_OutValues;

}; // end ITKToolsIntensityReplaceBase


/** \class ITKToolsIntensityReplace
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsIntensityReplace : public ITKToolsIntensityReplaceBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsIntensityReplace Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsIntensityReplace(){};
  ~ITKToolsIntensityReplace(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef TComponentType                                  OutputPixelType;
    const unsigned int Dimension = VDimension;

    typedef OutputPixelType                                 InputPixelType;

    typedef itk::Image< InputPixelType, Dimension >         InputImageType;
    typedef itk::Image< OutputPixelType, Dimension >        OutputImageType;

    typedef itk::ImageFileReader< InputImageType >          ReaderType;
    typedef itk::ChangeLabelImageFilter<
      InputImageType, OutputImageType >                     ReplaceFilterType;
    typedef itk::ImageFileWriter< OutputImageType >         WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename ReplaceFilterType::Pointer replaceFilter = ReplaceFilterType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Set up reader */
    reader->SetFileName( this->m_InputFileName );

    /** Setup the the input and the 'change map' of the replace filter. */
    replaceFilter->SetInput( reader->GetOutput() );
    if( itk::NumericTraits<OutputPixelType>::is_integer )
    {
      for( unsigned int i = 0; i < this->m_InValues.size(); ++i )
      {
        const InputPixelType inval = static_cast< InputPixelType >(
          atoi( this->m_InValues[ i ].c_str() )   );
        const OutputPixelType outval = static_cast< OutputPixelType >(
          atoi( this->m_OutValues[ i ].c_str() )   );
        replaceFilter->SetChange( inval, outval );
      }
    }
    else
    {
      for( unsigned int i = 0; i < this->m_InValues.size(); ++i )
      {
        const InputPixelType inval = static_cast< InputPixelType >(
          atof( this->m_InValues[ i ].c_str() )   );
        const OutputPixelType outval = static_cast< OutputPixelType >(
          atof( this->m_OutValues[ i ].c_str() )   );
        replaceFilter->SetChange( inval, outval );
      }
    }

    /** Set up writer. */
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( replaceFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end IntensityReplace

#endif // end #ifndef __intensityreplace_h_
