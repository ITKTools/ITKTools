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
 \brief Equalize the histogram of an image.
 
 \verbinclude histogramequalizeimage.help
 */
#ifndef __histogramequalizeimage_cxx
#define __histogramequalizeimage_cxx

#include "histogramequalizeimage.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsBase.h"
#include "ITKToolsHelpers.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss<< "This program applies histogram equalization to an image." << std::endl
    << "Works as described by Maintz, Introduction to Image Processing." << std::endl
    << "Usage:" << std::endl
    << "pxhistogramequalizeimage" << std::endl
    << "-in    \tInputImageFileName" << std::endl
    << "-out   \tOutputImageFileName" << std::endl
    << "-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>" << std::endl
    << "Currently only char, uchar, short, and ushort are supported." << std::endl
    << "-id    \tImageDimension <2,3>" << std::endl;

  return ss.str();

} // end GetHelpString()


/** HistogramEqualizeImage */

class ITKToolsHistogramEqualizeImageBase : public itktools::ITKToolsBase
{
public:
  ITKToolsHistogramEqualizeImageBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  ~ITKToolsHistogramEqualizeImageBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

}; // end ReplaceVoxelBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsHistogramEqualizeImage : public ITKToolsHistogramEqualizeImageBase
{
public:
  typedef ITKToolsHistogramEqualizeImage Self;

  ITKToolsHistogramEqualizeImage(){};
  ~ITKToolsHistogramEqualizeImage(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    typedef itk::Image<TComponentType, VDimension> ImageType;
    typedef typename ImageType::Pointer           ImagePointer;
    typedef typename ImageType::IndexType         IndexType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::RegionType        RegionType;
    typedef typename ImageType::PointType         PointType;
    typedef itk::ImageFileReader<ImageType>       ReaderType;
    typedef itk::ImageFileWriter<ImageType>       WriterType;
    typedef typename ReaderType::Pointer          ReaderPointer;
    typedef typename WriterType::Pointer          WriterPointer;
    typedef itk::HistogramEqualizationImageFilter<
      ImageType>                                  EnhancerType;
    typedef typename EnhancerType::Pointer        EnhancerPointer;

    WriterPointer writer = WriterType::New();
    EnhancerPointer enhancer = EnhancerType::New();

    /** Try to read input image */
    ReaderPointer reader = ReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Error while reading input image." << std::endl;
      std::cerr << err << std::endl;
      return;
    }

    /** Setup pipeline and configure its components */

    enhancer->SetInput( reader->GetOutput() );
    writer->SetInput( enhancer->GetOutput() );
    writer->SetFileName( this->m_OutputFileName.c_str());

    /** do it. */
    std::cout
      << "Saving image to disk as \""
      << this->m_OutputFileName
      << "\""
      << std::endl;
    try
    {
      writer->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << err << std::endl;
      return;
    }
  }

}; // end HistogramEqualizeImage

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-pt", "Pixel type." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Read the dimension. */
  unsigned int imageDimension = 0;
  parser->GetCommandLineArgument( "-id", imageDimension );

  if (imageDimension == 0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }

  std::string inputFileName("");
  parser->GetCommandLineArgument("-in", inputFileName);

  std::string outputFileName("");
  parser->GetCommandLineArgument("-out", outputFileName);
  
  std::string pixelTypeString("");
  bool retpt = parser->GetCommandLineArgument("-pt", pixelTypeString);
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  if(retpt)
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( pixelTypeString );
  }



  /** Class that does the work */
  ITKToolsHistogramEqualizeImageBase * histogramEqualizeImage = NULL;

  std::cout << "Detected component type: " <<
    componentType << std::endl;

  try
  {
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< short, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< unsigned short, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< char, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< unsigned char, 2 >::New( componentType, imageDimension );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< short, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< unsigned short, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< char, 2 >::New( componentType, imageDimension );
    if (!histogramEqualizeImage) histogramEqualizeImage = ITKToolsHistogramEqualizeImage< unsigned char, 2 >::New( componentType, imageDimension );
#endif
    if (!histogramEqualizeImage)
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << imageDimension
        << std::endl;
      return 1;
    }

    histogramEqualizeImage->m_InputFileName = inputFileName;
    histogramEqualizeImage->m_OutputFileName = outputFileName;

    histogramEqualizeImage->Run();

    delete histogramEqualizeImage;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete histogramEqualizeImage;
    return 1;
  }
  return EXIT_SUCCESS;

} // end function main

#endif // #ifndef __histogramequalizeimage_cxx

