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
#ifndef __castconverthelpers_h__
#define __castconverthelpers_h__

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
#include "itkImageToVectorImageFilter.h"
//#include "itkShiftScaleImageFilter.h"
#include "itkVectorCastImageFilter.h"

/**
 * \class ITKToolsCastConvertBase
 * \brief Base class for all castconverters.
 */

class ITKToolsCastConvertBase : public itktools::ITKToolsBase
{
public:
  ITKToolsCastConvertBase()
  {
    m_InputFileName = "";
    m_OutputFileName = "";
    m_UseCompression = false;

    m_InputDirectoryName = "";
    m_DICOMSeriesUID = "";
  };
  ~ITKToolsCastConvertBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  bool m_UseCompression;

  /** DICOM specific input parameters. */
  std::string m_InputDirectoryName;
  std::string m_DICOMSeriesUID;
  std::vector<std::string> m_DICOMSeriesRestrictions;

}; // end class ITKToolsCastConvertBase


/**
 * \class ITKToolsCastConvert
 * \brief Class that castconverts "normal" images.
 */

template< class TOutputComponentType, unsigned int VDimension >
class ITKToolsCastConvert : public ITKToolsCastConvertBase
{
public:
  typedef ITKToolsCastConvert Self;

  ITKToolsCastConvert(){};
  ~ITKToolsCastConvert(){};

  static Self * New( itktools::ComponentType outputComponentType, unsigned int dim )
  {
    if ( itktools::IsType<TOutputComponentType>( outputComponentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    typedef itk::VectorImage<double, VDimension>                InputVectorImageType;
    typedef itk::VectorImage<TOutputComponentType, VDimension>  OutputVectorImageType;

    typedef itk::Image<double, VDimension>                      InputScalarImageType;
    typedef itk::Image<TOutputComponentType, VDimension>        OutputScalarImageType;

    typedef typename itk::ImageFileReader< InputVectorImageType >     ImageReaderType;
    typedef typename itk::ImageFileWriter< OutputVectorImageType >    ImageWriterType;

    /** Create and setup the reader. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    // Create the disassembler
    typedef itk::VectorIndexSelectionCastImageFilter<InputVectorImageType, InputScalarImageType> IndexSelectionType;
    typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
    indexSelectionFilter->SetInput( reader->GetOutput() );

    // Create the re-assembler
    typedef itk::ImageToVectorImageFilter<OutputScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    typedef itk::CastImageFilter<InputVectorImageType, OutputVectorImageType> CastImageFilterType;
    typename CastImageFilterType::Pointer castImageFilter = CastImageFilterType::New();
    
    castImageFilter->SetInput(reader->GetOutput());
    castImageFilter->Update();

    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->SetInput( castImageFilter->GetOutput() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCastConvert


/**
 * \class ITKToolsCastConvertDICOM
 * \brief Class that castconverts DICOM images.
 */

template< class TOutputComponentType, unsigned int VDimension >
class ITKToolsCastConvertDICOM : public ITKToolsCastConvertBase
{
public:
  typedef ITKToolsCastConvertDICOM Self;

  ITKToolsCastConvertDICOM(){};
  ~ITKToolsCastConvertDICOM(){};

  static Self * New( itktools::ComponentType outputComponentType, unsigned int dim )
  {
    if ( itktools::IsType<TOutputComponentType>( outputComponentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  virtual void Run( void )
  {
    /** Typedef the correct reader, caster and writer. */
    typedef itk::Image< double, VDimension >                      InputScalarImageType;
    typedef itk::Image< TOutputComponentType, VDimension >        OutputScalarImageType;

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
    for ( unsigned int i = 0; i < this->m_DICOMSeriesRestrictions.size(); ++i )
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

    /**  Print  information. */
    //PrintInfo( seriesReader, writer );

  } // end Run()

}; // end class ITKToolsCastConvertDICOM



///----------------------------------------




// BELOW IS NOT USED




/** Print image information from the reader and the writer. */
template< class ReaderType, class WriterType >
void
PrintInfo( ReaderType reader, WriterType writer )
{
  /** Typedef's. */
  typedef itk::ImageIOBase                        ImageIOBaseType;
  typedef itk::ImageIORegion                      ImageIORegionType;
  typedef typename ImageIORegionType::SizeType    SizeType;

  /** Get IOBase of the reader and extract information. */
  ImageIOBaseType::Pointer imageIOBaseIn = reader->GetImageIO();
  ImageIORegionType iORegionIn = imageIOBaseIn->GetIORegion();

  const char * fileNameIn = imageIOBaseIn->GetFileName();
  std::string pixelTypeIn = imageIOBaseIn->GetPixelTypeAsString(
    imageIOBaseIn->GetPixelType() );
  unsigned int nocIn = imageIOBaseIn->GetNumberOfComponents();
  std::string componentTypeIn = imageIOBaseIn->GetComponentTypeAsString(
    imageIOBaseIn->GetComponentType() );
  unsigned int dimensionIn = imageIOBaseIn->GetNumberOfDimensions();
  SizeType sizeIn = iORegionIn.GetSize();

  /** Get IOBase of the writer and extract information. */
  ImageIOBaseType::Pointer imageIOBaseOut = writer->GetImageIO();
  ImageIORegionType iORegionOut = imageIOBaseOut->GetIORegion();

  const char * fileNameOut = imageIOBaseOut->GetFileName();
  std::string pixelTypeOut = imageIOBaseOut->GetPixelTypeAsString(
    imageIOBaseOut->GetPixelType() );
  unsigned int nocOut = imageIOBaseOut->GetNumberOfComponents();
  std::string componentTypeOut = imageIOBaseOut->GetComponentTypeAsString(
    imageIOBaseOut->GetComponentType() );
  unsigned int dimensionOut = imageIOBaseOut->GetNumberOfDimensions();
  SizeType sizeOut = iORegionOut.GetSize();
  std::string useCompression = "false";
  if ( imageIOBaseOut->GetUseCompression() )
  {
    useCompression = "true";
  }

  /** Print information. */
  std::cout << "Information about the input image \"" << fileNameIn << "\":\n";
  std::cout << "\tdimension:\t\t" << dimensionIn << "\n";
  std::cout << "\tpixel type:\t\t" << pixelTypeIn << "\n";
  std::cout << "\tnumber of components:\t" << nocIn << "\n";
  std::cout << "\tcomponent type:\t\t" << componentTypeIn << "\n";
  std::cout << "\tsize:\t\t\t";
  for ( unsigned int i = 0; i < dimensionIn; i++ ) std::cout << sizeIn[ i ] << " ";
  std::cout << std::endl;

  /** Print information. */
  std::cout << std::endl;
  std::cout << "Information about the output image \"" << fileNameOut << "\":\n";
  std::cout << "\tdimension:\t\t" << dimensionOut << "\n";
  std::cout << "\tpixel type:\t\t" << pixelTypeOut << "\n";
  std::cout << "\tnumber of components:\t" << nocOut << "\n";
  std::cout << "\tcomponent type:\t\t" << componentTypeOut << "\n";
  std::cout << "\tuse compression:\t" << useCompression << "\n";
  std::cout << "\tsize:\t\t\t";
  for ( unsigned int i = 0; i < dimensionOut; i++ ) std::cout << sizeOut[ i ] << " ";
  std::cout << std::endl;

}  // end PrintInfo()


#endif //__castconverthelpers_h__
