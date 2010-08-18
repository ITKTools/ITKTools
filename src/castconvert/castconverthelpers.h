/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: castconverthelpers.h,v $
  Language:  C++
  Date:      $Date: 2007-06-01 13:00:06 $
  Version:   $Revision: 1.4 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __castconverthelpers_h__
#define __castconverthelpers_h__

/** Basic Image support. */
#include "itkImage.h"
#include "itkImageIORegion.h"

/** Reading and writing images. */
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

/** DICOM headers. */
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

/** One of these is used to cast the image. */
#include "itkShiftScaleImageFilter.h"
//#include "itkRescaleIntensityImageFilter.h"
#include "itkVectorCastImageFilter.h"

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


/** The function that reads the input dicom image and writes the output image.
 * This function is templated over the image types. In the main function
 * we have to make sure to call the right instantiation.
 */

template< class InputImageType, class OutputImageType >
void
ReadDicomSeriesCastWriteImage(
  const std::string & inputDirectoryName,
  const std::string & outputFileName,
  const std::string & seriesUID,
  const std::vector<std::string> & restrictions,
  const bool & useCompression )
{
  /** Typedef the correct reader, caster and writer. */
  typedef typename itk::ImageSeriesReader< InputImageType >     SeriesReaderType;
  //typedef typename itk::RescaleIntensityImageFilter<
    //InputImageType, OutputImageType >                           RescaleFilterType;
  typedef typename itk::ShiftScaleImageFilter<
    InputImageType, OutputImageType >                           ShiftScaleFilterType;
  typedef typename itk::ImageFileWriter< OutputImageType >      ImageWriterType;

  /** Typedef dicom stuff. */
  typedef itk::GDCMImageIO                  GDCMImageIOType;
  typedef itk::GDCMSeriesFileNames          GDCMNamesGeneratorType;
  typedef std::vector< std::string >        FileNamesContainerType;

  /** Create the dicom ImageIO. */
  typename GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();

  /** Get a list of the filenames of the 2D input dicom images. */
  GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails( true );
  for ( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );
  FileNamesContainerType fileNames = nameGenerator->GetFileNames( seriesUID );

  /** Create and setup the seriesReader. */
  typename SeriesReaderType::Pointer seriesReader = SeriesReaderType::New();
  seriesReader->SetFileNames( fileNames );
  seriesReader->SetImageIO( dicomIO );

  /** Create and setup caster and writer. */
  //typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
  typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
  typename ImageWriterType::Pointer  writer = ImageWriterType::New();
  caster->SetShift( 0.0 );
  caster->SetScale( 1.0 );
  writer->SetFileName( outputFileName.c_str()  );
  writer->SetUseCompression( useCompression );

  /** Connect the pipeline. */
  caster->SetInput(  seriesReader->GetOutput()  );
  writer->SetInput(  caster->GetOutput()  );

  /**  Do the actual  conversion.  */
  writer->Update();

  /**  Print  information. */
  PrintInfo( seriesReader, writer );

}  // end ReadDicomSeriesCastWriteImage()


/** The function that reads the input image and writes the output image.
 * This function is templated over the image types. In the main function
 * we have to make sure to call the right instantiation.
 */

template< class InputImageType, class OutputImageType >
void
ReadCastWriteImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const bool & useCompression )
{
  /**  Typedef the correct reader, caster and writer. */
  typedef typename itk::ImageFileReader< InputImageType >     ImageReaderType;
  //typedef typename itk::RescaleIntensityImageFilter<
    //InputImageType, OutputImageType >                         RescaleFilterType;
  typedef typename itk::ShiftScaleImageFilter<
    InputImageType, OutputImageType >                         ShiftScaleFilterType;
  typedef typename itk::ImageFileWriter< OutputImageType >    ImageWriterType;

  /** Create and setup the reader. */
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Create and setup caster and writer. */
  //typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
  typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
  typename ImageWriterType::Pointer  writer = ImageWriterType::New();
  caster->SetShift( 0.0 );
  caster->SetScale( 1.0 );
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );

  /** Connect the pipeline. */
  caster->SetInput( reader->GetOutput() );
  writer->SetInput( caster->GetOutput() );

  /** Do the actual conversion. */
  writer->Update();

  /** Print information. */
  PrintInfo( reader, writer );

}  // end ReadWriteImage()


/** The function that reads the input vector image and writes the vector output image.
 * This function is templated over the image types. In the main function
 * we have to make sure to call the right instantiation.
 */

template< class InputImageType, class OutputImageType >
void
ReadCastWriteVectorImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const bool & useCompression )
{
  /**  Typedef the correct reader, caster and writer. */
  typedef typename itk::ImageFileReader< InputImageType >     ImageReaderType;
  typedef typename itk::VectorCastImageFilter<
    InputImageType, OutputImageType >                         CasterType;
  typedef typename itk::ImageFileWriter< OutputImageType >    ImageWriterType;

  /** Create reader, caster and writer. */
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  typename CasterType::Pointer      caster = CasterType::New();
  typename ImageWriterType::Pointer writer = ImageWriterType::New();

  /** Setup reader, caster and writer. */
  reader->SetFileName( inputFileName.c_str() );
  //caster->InPlaceOn();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetUseCompression( useCompression );

  /** Connect the pipeline. */
  caster->SetInput( reader->GetOutput() );
  writer->SetInput( caster->GetOutput() );

  /** Do the actual conversion. */
  writer->Update();

  /** Print information. */
  PrintInfo( reader, writer );

}  // end ReadWriteImage()


/** Macros are used in order to make the code in main() look cleaner. */

/** callCorrectReadDicomWriterMacro:
 * A macro to call the dicom-conversion function.
 */
#define callCorrectReadDicomWriterMacro(typeIn,typeOut) \
    if ( inputPixelComponentType == #typeIn && outputPixelComponentType == #typeOut ) \
{ \
  typedef  itk::Image< typeIn, 3 >    InputImageType; \
  typedef  itk::Image< typeOut, 3 >  OutputImageType; \
  ReadDicomSeriesCastWriteImage< InputImageType, OutputImageType >( \
    inputDirectoryName, outputFileName, seriesUID, restrictions, useCompression ); \
}

/** callCorrectReadWriterMacro:
 * A macro to call the conversion function.
 */
#define callCorrectReadWriterMacro(typeIn,typeOut,dim) \
    if ( inputPixelComponentType == #typeIn \
    && outputPixelComponentType == #typeOut && inputDimension == dim ) \
{ \
  typedef  itk::Image< typeIn, dim >    InputImageType; \
  typedef  itk::Image< typeOut, dim >  OutputImageType; \
  ReadCastWriteImage< InputImageType, OutputImageType >( \
    inputFileName, outputFileName, useCompression ); \
}

/** callCorrectReadWriterMultiComponentMacro:
 * A macro to call the conversion function for multi-component images.
 */
#define callCorrectReadWriterMultiComponentMacro( typeIn, typeOut, vecDim, dim ) \
    if ( inputPixelComponentType == #typeIn \
      && outputPixelComponentType == #typeOut \
      && numberOfComponents == vecDim && inputDimension == dim ) \
{ \
  typedef itk::Vector< typeIn, vecDim >        InputVectorType; \
  typedef itk::Vector< typeOut, vecDim >       OutputVectorType; \
  typedef itk::Image< InputVectorType, dim >   InputImageType; \
  typedef itk::Image< OutputVectorType, dim >  OutputImageType; \
  ReadCastWriteVectorImage< InputImageType, OutputImageType >( \
    inputFileName, outputFileName, useCompression ); \
}


#endif //__castconverthelpers_h__
