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

#ifndef __averagevectormagnitude_cxx
#define __averagevectormagnitude_cxx

#include "ITKToolsBase.h"
#include "ITKToolsImageProperties.h"
#include "itkCommandLineArgumentParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkExceptionObject.h"

#include <map>
#include <iostream>
#include <string>

std::string GetHelpString()
{
  std::stringstream ss;
  ss << "Calculate the average magnitude of the vectors in a vector image." << std::endl
     << "Usage:" << std::endl
     << "AverageVectorMagnitude" << std::endl
     << "-in InputVectorImageFileName" << std::endl
     << "[-out OutputImageFileName]" << std::endl
     << "-id ImageDimension" << std::endl
     << "-sd SpaceDimension (the dimension of the vectors)" << std::endl;

  return ss.str();
}

class AverageVectorMagnitudeBase : public itktools::ITKToolsBase
{
public:
  AverageVectorMagnitudeBase()
  {
    m_AverageMagnitude = 0.0f;
  }
  
  ~AverageVectorMagnitudeBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

  float m_AverageMagnitude;

  virtual void Run(void) = 0;
}; // end AverageVectorMagnitudeBase


template< unsigned int VVectorDimension, unsigned int VImageDimension >
class AverageVectorMagnitude : public AverageVectorMagnitudeBase
{
public:
  typedef AverageVectorMagnitude Self;

  AverageVectorMagnitude(){};
  ~AverageVectorMagnitude(){};

  static Self * New( unsigned int vectorDimension, unsigned int imageDimension )
  {
    if ( VVectorDimension == vectorDimension && VImageDimension == imageDimension )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
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
    reader->SetFileName(m_InputFileName);
    filter->SetInput( reader->GetOutput() );
    magnitudeImage = filter->GetOutput();

    /** Only write to disk if an outputFileName is given */
    if (m_OutputFileName.size() > 0 && m_OutputFileName.compare("") != 0)
    {
      writer = WriterType::New();
      writer->SetFileName(m_OutputFileName);
      writer->SetInput( magnitudeImage );
    }

    try
    {
      if (writer)
      {
        writer->Update();
      }
      else
      {
        magnitudeImage->Update();
      }
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << err << std::endl;
      throw err;
    }

    /** Sum over the resulting image and divide by the number of pixels */
    IteratorType iterator(magnitudeImage, magnitudeImage->GetLargestPossibleRegion() );
    double sum = 0.0;
    unsigned long nrOfPixels = 0;

    for (iterator = iterator.Begin(); !iterator.IsAtEnd(); ++iterator)
    {
      sum += iterator.Value();
      ++nrOfPixels;
    }
    this->m_AverageMagnitude = static_cast<ValueType>( sum / nrOfPixels );
  }

}; // end AverageVectorMagnitude
//-------------------------------------------------------------------------------------



int main( int argc, char** argv )
{
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  std::string inputFileName("");
  std::string outputFileName(inputFileName + "AverageVectorMagnitude.mhd");
  unsigned int imageDimension = 2;
  unsigned int spaceDimension = 1;

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-id", "Image dimension." );
  parser->MarkArgumentAsRequired( "-sd", "Space dimension." );

  parser->GetCommandLineArgument( "-in", inputFileName );
  parser->GetCommandLineArgument( "-out", outputFileName );
  parser->GetCommandLineArgument( "-sd", spaceDimension );
  parser->GetCommandLineArgument( "-id", imageDimension );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Determine image properties. */

  unsigned int numberOfComponents = 0;
  itktools::GetImageNumberOfComponents(inputFileName, numberOfComponents);

  unsigned int dimension = 0;
  itktools::GetImageDimension(inputFileName, dimension);

  AverageVectorMagnitudeBase * averageVectorMagnitude = 0;

  float averageMagnitude = 0.0f; // Initialize output to zero
  try
  {
    // 2D
    if (!averageVectorMagnitude) averageVectorMagnitude = AverageVectorMagnitude< 2, 2 >::New( numberOfComponents, dimension );
    if (!averageVectorMagnitude) averageVectorMagnitude = AverageVectorMagnitude< 2, 3 >::New( numberOfComponents, dimension );
    if (!averageVectorMagnitude) averageVectorMagnitude = AverageVectorMagnitude< 3, 2 >::New( numberOfComponents, dimension );
    if (!averageVectorMagnitude) averageVectorMagnitude = AverageVectorMagnitude< 3, 3 >::New( numberOfComponents, dimension );

    if (!averageVectorMagnitude)
    {
      std::cerr << "ERROR: this combination of numberOfComponents and dimension is not supported!" << std::endl;
      std::cerr
        << "numberOfComponents = " << numberOfComponents
        << " ; dimension = " << dimension
        << std::endl;
      return 1;
    }

    averageVectorMagnitude->m_InputFileName = inputFileName;
    averageVectorMagnitude->m_OutputFileName = outputFileName;

    averageVectorMagnitude->Run();

    averageMagnitude = averageVectorMagnitude->m_AverageMagnitude;
    delete averageVectorMagnitude;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete averageVectorMagnitude;
    return 1;
  }

  std::cout << "The average magnitude of the vectors in image \"" <<
    inputFileName << "\" is: " << averageVectorMagnitude << std::endl;

  return 0;

} // end function main

#endif // #ifndef __avm_cxx
