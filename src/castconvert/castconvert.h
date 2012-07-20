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
#ifndef __castconverthelpers_h_
#define __castconverthelpers_h_

#include "ITKToolsBase.h"
#include "ITKToolsHelpers.h"
#include <iostream>

/** Basic Image support. */
#include "itkImageIORegion.h"
#include "itkImage.h"
#include "itkVectorImage.h"

/** Reading and writing images. */
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

/** DICOM headers. */
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

/** One of these is used to cast the image. */
#include "itkCastImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkComposeImageFilter.h"
//#include "itkShiftScaleImageFilter.h"
#include "itkVectorCastImageFilter.h"


/** \class ITKToolsCastConvertBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCastConvertBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCastConvertBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_UseCompression = false;

    this->m_InputDirectoryName = "";
    this->m_DICOMSeriesUID = "";
  };
  /** Destructor. */
  ~ITKToolsCastConvertBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  bool m_UseCompression;

  /** DICOM specific input parameters. */
  std::string m_InputDirectoryName;
  std::string m_DICOMSeriesUID;
  std::vector<std::string> m_DICOMSeriesRestrictions;

}; // end class ITKToolsCastConvertBase


/** \class ITKToolsCastConvert
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCastConvert : public ITKToolsCastConvertBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCastConvert Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCastConvert(){};
  ~ITKToolsCastConvert(){};

  /** Run function. */
  void Run( void )
  {
    typedef itk::VectorImage< double, VDimension >              InputVectorImageType;
    typedef itk::VectorImage< TComponentType, VDimension >      OutputVectorImageType;
    typedef itk::Image< double, VDimension >                    InputScalarImageType;
    typedef itk::Image< TComponentType, VDimension >            OutputScalarImageType;
    typedef typename itk::ImageFileReader< InputVectorImageType >     ImageReaderType;
    typedef typename itk::ImageFileWriter< OutputVectorImageType >    ImageWriterType;

    /** Create and setup the reader. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    // Create the disassembler
    typedef itk::VectorIndexSelectionCastImageFilter<
      InputVectorImageType, InputScalarImageType >        IndexSelectionType;
    typename IndexSelectionType::Pointer indexSelectionFilter
      = IndexSelectionType::New();
    indexSelectionFilter->SetInput( reader->GetOutput() );

    // Create the re-assembler
    typedef itk::ComposeImageFilter< OutputScalarImageType > ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter
      = ImageToVectorImageFilterType::New();

    typedef itk::CastImageFilter< InputVectorImageType, OutputVectorImageType > CastImageFilterType;
    typename CastImageFilterType::Pointer castImageFilter = CastImageFilterType::New();

    castImageFilter->SetInput( reader->GetOutput() );
    castImageFilter->Update();

    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->SetInput( castImageFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCastConvert


/** \class ITKToolsCastConvertDICOM
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCastConvertDICOM : public ITKToolsCastConvertBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCastConvertDICOM Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCastConvertDICOM(){};
  ~ITKToolsCastConvertDICOM(){};

  /** Run function. */
  virtual void Run( void )
  {
    /** Typedef the correct reader, caster and writer. */
    typedef itk::Image< double, VDimension >                          InputScalarImageType;
    typedef itk::Image< TComponentType, VDimension >                  OutputScalarImageType;

    typedef typename itk::ImageSeriesReader< InputScalarImageType >   SeriesReaderType;
    typedef typename itk::CastImageFilter<
      InputScalarImageType, OutputScalarImageType >                   CastFilterType;
    typedef typename itk::ImageFileWriter< OutputScalarImageType >    ImageWriterType;

    /** Typedef DICOM stuff. */
    typedef itk::GDCMImageIO                  GDCMImageIOType;
    typedef itk::GDCMSeriesFileNames          GDCMNamesGeneratorType;
    typedef std::vector< std::string >        FileNamesContainerType;

    /** Create the DICOM ImageIO. */
    typename GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();

    /** Get a list of the filenames of the 2D input DICOM images. */
    GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    for( unsigned int i = 0; i < this->m_DICOMSeriesRestrictions.size(); ++i )
    {
      nameGenerator->AddSeriesRestriction( this->m_DICOMSeriesRestrictions[ i ] );
    }
    nameGenerator->SetInputDirectory( this->m_InputDirectoryName.c_str() );
    FileNamesContainerType fileNames = nameGenerator->GetFileNames( this->m_DICOMSeriesUID );

    /** Create and setup the seriesReader. */
    typename SeriesReaderType::Pointer seriesReader = SeriesReaderType::New();
    seriesReader->SetFileNames( fileNames );
    seriesReader->SetImageIO( dicomIO );

    /** Create and setup caster and writer. */
    typename CastFilterType::Pointer caster = CastFilterType::New();
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str()  );
    writer->SetUseCompression( this->m_UseCompression );

    /** Connect the pipeline. */
    caster->SetInput(  seriesReader->GetOutput()  );
    writer->SetInput(  caster->GetOutput()  );

    /**  Do the actual  conversion.  */
    writer->Update();

  } // end Run()

}; // end class ITKToolsCastConvertDICOM


#endif //__castconverthelpers_h_
