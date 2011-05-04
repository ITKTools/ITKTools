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

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

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

std::string PrintUsageString();

template <
  unsigned int NImageDimension,
  unsigned int NSpaceDimension,
  class ValueType >
ValueType run_avm(const char * inputFileName, const char * outputFileName = 0)
{
  /** Typedefs */
  const unsigned int ImageDimension = NImageDimension;
  const unsigned int SpaceDimension = NSpaceDimension;

  typedef ValueType     InputValueType;
  typedef itk::Vector<InputValueType, SpaceDimension> InputPixelType;
  typedef InputValueType    OutputPixelType;

  typedef itk::Image<InputPixelType, ImageDimension>  InputImageType;
  typedef itk::Image<OutputPixelType, ImageDimension>   OutputImageType;
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
  reader->SetFileName(inputFileName);
  filter->SetInput( reader->GetOutput() );
  magnitudeImage = filter->GetOutput();

  /** Only write to disk if an outputFileName is given */
  if (outputFileName)
  {
    if (std::string(outputFileName) != "")
    {
      writer = WriterType::New();
      writer->SetFileName(outputFileName);
      writer->SetInput( magnitudeImage );
    }
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
  ValueType averageVectorMagnitude = static_cast<ValueType>( sum / nrOfPixels );

  return averageVectorMagnitude;

} // end function run_avm



int main( int argc, char** argv )
{
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(PrintUsageString());

  std::string inputFileName("");
  std::string outputFileName(inputFileName + "AverageVectorMagnitude.mhd");
  std::string imageDimension("");
  std::string spaceDimension("");

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

  float averageVectorMagnitude = 0.0f;
  if (imageDimension.compare("2") == 0)
  {
    if (spaceDimension.compare("2") == 0)
    {
       averageVectorMagnitude = run_avm<2,2,float>(inputFileName.c_str(), outputFileName.c_str() );
    }
    else if (spaceDimension.compare("3") == 0)
    {
       averageVectorMagnitude = run_avm<2,3,float>(inputFileName.c_str(), outputFileName.c_str());
    }
  }
  else if (imageDimension.compare("3") == 0)
  {
    if (spaceDimension.compare("2") == 0)
    {
       averageVectorMagnitude = run_avm<3,2,float>(inputFileName.c_str(), outputFileName.c_str());
    }
    else if (spaceDimension.compare("3") == 0)
    {
       averageVectorMagnitude = run_avm<3,3,float>(inputFileName.c_str(), outputFileName.c_str());
    }
  }

  std::cout << "The average magnitude of the vectors in image \"" <<
    inputFileName << "\" is: " << averageVectorMagnitude << std::endl;

  if ( !outputFileName.empty() )
  {
    std::cout << "The magnitude image is written as \"" << outputFileName << "\"" << std::endl;
  }
  return 0;

} // end function main

std::string PrintUsageString()
{
  std::string helpString =
    "Calculate the average magnitude of the vectors in a vector image.\n\n \
    Usage:\n \
    AverageVectorMagnitude\n \
    \t-in InputVectorImageFileName\n \
    \t[-out OutputImageFileName]\n \
    \t-id ImageDimension\n \
    \t-sd SpaceDimension (the dimension of the vectors)\n";

  return helpString;
}

#endif // #ifndef __avm_cxx

