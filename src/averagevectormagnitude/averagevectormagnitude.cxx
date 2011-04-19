#ifndef __avm_cxx
#define __avm_cxx

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

void PrintUsageString(void)
{
  std::cerr
    << "Calculate the average magnitude of the vectors in a vector image.\n\n"
    << "Usage:\n"
    << "AverageVectorMagnitude\n"
    << "\t-in InputVectorImageFileName\n"
    << "\t[-out OutputImageFileName]\n"
    << "\t-id ImageDimension\n"
    << "\t-sd SpaceDimension (the dimension of the vectors)\n"
    << std::endl;
}


int main( int argc, char** argv )
{
  typedef std::map<std::string, std::string> ArgMapType;
  typedef float ValueType;

  ArgMapType argmap;
  std::string inputFileName("");
  std::string outputFileName("");
  std::string imageDimension("");
  std::string spaceDimension("");

  /** Fill the argument map */
  for (unsigned int i = 1; i < static_cast<unsigned int>(argc); i+=2)
  {
    if ( (i+1) < static_cast<unsigned int>(argc))
    {
      argmap[ argv[i] ] = argv[i+1];
    }
    else
    {
      argmap[ argv[i] ] = "";
    }
  }

  /** Help needed? */
  if (argmap.count("-h") || argmap.count("-help") || argmap.count("--help") )
  {
    PrintUsageString();
    return -1;
  }

  if ( argmap.count("-in") )
  {
    inputFileName = argmap["-in"];
  }
  else
  {
    std::cerr << "Not enough arguments\n";
    PrintUsageString();
    return -1;
  }
  if ( argmap.count("-out") )
  {
    outputFileName = argmap["-out"];
  }
  if ( argmap.count("-id") )
  {
    imageDimension = argmap["-id"];
  }
  else
  {
    std::cerr << "Not enough arguments\n";
    PrintUsageString();
    return -1;
  }
  if ( argmap.count("-sd") )
  {
    spaceDimension = argmap["-sd"];
  }
  else
  {
    std::cerr << "Not enough arguments\n";
    PrintUsageString();
    return -1;
  }

  ValueType averageVectorMagnitude = 0.0f;
  if (imageDimension == "2")
  {
    if (spaceDimension == "2")
    {
       averageVectorMagnitude = run_avm<2,2,float>(inputFileName.c_str(), outputFileName.c_str() );
    }
    else if (spaceDimension == "3")
    {
       averageVectorMagnitude = run_avm<2,3,float>(inputFileName.c_str(), outputFileName.c_str());
    }
  }
  else if (imageDimension == "3")
  {
    if (spaceDimension == "2")
    {
       averageVectorMagnitude = run_avm<3,2,float>(inputFileName.c_str(), outputFileName.c_str());
    }
    else if (spaceDimension == "3")
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

#endif // #ifndef __avm_cxx

