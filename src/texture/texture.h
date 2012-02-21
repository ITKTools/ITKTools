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
#ifndef __texture_h_
#define __texture_h_

#include "ITKToolsBase.h"

#include <itksys/SystemTools.hxx>

#include "itkTextureImageToImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMultiThreader.h"


/** \class ITKToolsTextureBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsTextureBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsTextureBase()
  {
    this->m_InputFileName = "";
    this->m_OutputDirectory = "";
    this->m_NeighborhoodRadius = 0;
    this->m_NumberOfBins = 0;
    this->m_NumberOfOutputs = 0;
  };
  /** Destructor. */
  ~ITKToolsTextureBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputDirectory;
  unsigned int m_NeighborhoodRadius;
  std::vector< unsigned int > m_OffsetScales;
  unsigned int m_NumberOfBins;
  unsigned int m_NumberOfOutputs;

}; // end class ITKToolsTextureBase


/** \class ITKToolsTexture
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TInputComponentType, class TOutputComponentType >
class ITKToolsTexture : public ITKToolsTextureBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsTexture Self;
  itktoolsTwoTypeNewMacro( Self );

  ITKToolsTexture(){};
  ~ITKToolsTexture(){};

  // Helper class to print the progress
  class ShowProgressObject
  {
  public:
    ShowProgressObject( itk::ProcessObject* o )
    {
      this->m_Process = o;
    }
    void ShowProgress()
    {
      std::cout << "\rProgress: "
        << static_cast<unsigned int>( 100.0 * this->m_Process->GetProgress() ) << "%";
    }
    itk::ProcessObject::Pointer m_Process;
  }; // end class ShowProgressObject

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TInputComponentType, VDimension>   InputImageType;
    typedef itk::Image<TOutputComponentType, VDimension>  OutputImageType;
    typedef itk::TextureImageToImageFilter<
      InputImageType, OutputImageType >                   TextureFilterType;
    typedef itk::ImageFileReader< InputImageType >        ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >       WriterType;

    /** Read the input. */
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );

    /** Setup the filter filter. */
    typename TextureFilterType::Pointer textureFilter = TextureFilterType::New();
    textureFilter->SetInput( reader->GetOutput() );
    textureFilter->SetNeighborhoodRadius( this->m_NeighborhoodRadius );
    textureFilter->SetOffsetScales( this->m_OffsetScales );
    textureFilter->SetNumberOfHistogramBins( this->m_NumberOfBins );
    textureFilter->SetNormalizeHistogram( false );
    textureFilter->SetNumberOfRequestedOutputs( this->m_NumberOfOutputs );

    /** Create and attach a progress observer. */
    ShowProgressObject progressWatch( textureFilter );
    typename itk::SimpleMemberCommand<ShowProgressObject>::Pointer progressCommand
      = itk::SimpleMemberCommand<ShowProgressObject>::New();
    progressCommand->SetCallbackFunction( &progressWatch, &ShowProgressObject::ShowProgress );
    textureFilter->AddObserver( itk::ProgressEvent(), progressCommand );

    /** Create the output file names. */
    std::vector< std::string > outputFileNames( 8, "" );
    outputFileNames[ 0 ] = this->m_OutputDirectory + "energy.mhd";
    outputFileNames[ 1 ] = this->m_OutputDirectory + "entropy.mhd";
    outputFileNames[ 2 ] = this->m_OutputDirectory + "correlation.mhd";
    outputFileNames[ 3 ] = this->m_OutputDirectory + "inverseDifferenceMoment.mhd";
    outputFileNames[ 4 ] = this->m_OutputDirectory + "inertia.mhd";
    outputFileNames[ 5 ] = this->m_OutputDirectory + "clusterShade.mhd";
    outputFileNames[ 6 ] = this->m_OutputDirectory + "clusterProminence.mhd";
    outputFileNames[ 7 ] = this->m_OutputDirectory + "HaralickCorrelation.mhd";

    /** Setup and process the pipeline. */
    for( unsigned int i = 0; i < this->m_NumberOfOutputs; ++i )
    {
      typename WriterType::Pointer writer = WriterType::New();
      writer->SetFileName( outputFileNames[ i ].c_str() );
      writer->SetInput( textureFilter->GetOutput( i ) );
      writer->Update();
    }
  } // end Run()

}; // end class ITKToolsTexture


#endif // end #ifndef __texture_h

