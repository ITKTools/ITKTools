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

/** \file
 \brief Calculate the average magnitude of the vectors in a vector image.
 
 \verbinclude averagevectormagnitude.help
 */

#ifndef __averagevectormagnitude_h
#define __averagevectormagnitude_h

#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkExceptionObject.h"

// #include <map>
#include <iostream>
#include <string>


class ITKToolsAverageVectorMagnitudeBase : public itktools::ITKToolsBase
{
public:
  ITKToolsAverageVectorMagnitudeBase()
  {
    this->m_AverageMagnitude = 0.0f;
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  }
  
  ~ITKToolsAverageVectorMagnitudeBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  float m_AverageMagnitude;

  virtual void Run( void ) = 0;
}; // end AverageVectorMagnitudeBase


template< unsigned int VVectorDimension, unsigned int VImageDimension >
class ITKToolsAverageVectorMagnitude : public ITKToolsAverageVectorMagnitudeBase
{
public:
  typedef ITKToolsAverageVectorMagnitude Self;

  ITKToolsAverageVectorMagnitude(){};
  ~ITKToolsAverageVectorMagnitude(){};

  static Self * New( unsigned int vectorDimension, unsigned int imageDimension )
  {
    if ( VVectorDimension == vectorDimension && VImageDimension == imageDimension )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs */
    typedef float     ValueType;
    typedef itk::Vector<ValueType, VVectorDimension> InputPixelType;
    typedef ValueType    OutputPixelType;

    typedef itk::Image<InputPixelType, VImageDimension>  InputImageType;
    typedef itk::Image<OutputPixelType, VImageDimension>   OutputImageType;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef itk::ImageRegionConstIterator<OutputImageType>  IteratorType;

    typedef itk::ImageFileReader<InputImageType>  ReaderType;
    typedef itk::ImageFileWriter<OutputImageType>   WriterType;
    typedef typename ReaderType::Pointer ReaderPointer;
    typedef typename WriterType::Pointer WriterPointer;

    typedef itk::GradientToMagnitudeImageFilter<
      InputImageType, OutputImageType>  FilterType;
    typedef typename FilterType::Pointer    FilterPointer;

    /** Create variables */
    ReaderPointer reader = ReaderType::New();
    WriterPointer writer = 0;
    FilterPointer filter = FilterType::New();
    OutputImagePointer magnitudeImage = 0;

    /** Setup the pipeline */
    reader->SetFileName( this->m_InputFileName );
    filter->SetInput( reader->GetOutput() );
    magnitudeImage = filter->GetOutput();

    /** Only write to disk if an outputFileName is given */
    if( this->m_OutputFileName.size() > 0 && this->m_OutputFileName.compare("") != 0 )
    {
      writer = WriterType::New();
      writer->SetFileName( this->m_OutputFileName );
      writer->SetInput( magnitudeImage );
    }

    try
    {
      if ( writer )
      {
        writer->Update();
      }
      else
      {
        magnitudeImage->Update();
      }
    }
    catch( itk::ExceptionObject & err )
    {
      std::cerr << err << std::endl;
      throw err;
    }

    /** Sum over the resulting image and divide by the number of pixels */
    IteratorType iterator( magnitudeImage, magnitudeImage->GetLargestPossibleRegion() );
    double sum = 0.0;
    unsigned long nrOfPixels = 0;

    for ( iterator = iterator.Begin(); !iterator.IsAtEnd(); ++iterator )
    {
      sum += iterator.Value();
      ++nrOfPixels;
    }
    this->m_AverageMagnitude = static_cast<ValueType>( sum / nrOfPixels );

  } // end Run()

}; // end AverageVectorMagnitude

#endif // end #ifndef __averagevectormagnitude_h
