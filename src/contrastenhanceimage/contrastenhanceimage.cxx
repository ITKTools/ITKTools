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
 \brief Enhance the contrast of an image.
 
 \verbinclude contrastenhanceimage.help
 */
#ifndef __contrastenhanceimage_cxx
#define __contrastenhanceimage_cxx

#include "contrastenhanceimage.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include <iostream>
#include <string>
#include <map>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

#include "itkCommandLineArgumentParser.h"

std::string GetHelpString(void)
{
  std::stringstream ss;
  ss << "This program enhances an image." << std::endl
    << "alpha and beta control the exact behaviour of the filter. See the" << std::endl
    << "ITK documentation of the AdaptiveHistogramEqualizationImageFilter" << std::endl
    << "Usage:" << std::endl
    << "pxcontrastenhanceimage" << std::endl
    << "-in    \tInputImageFileName" << std::endl
    << "-out   \tOutputImageFileName" << std::endl
    << "-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>" << std::endl
    << "Currently only char, uchar and short are supported." << std::endl
    << "-id    \tImageDimension <2,3>" << std::endl
    << "-alpha \t0.0 < alpha < 1.0" << std::endl
    << "-beta  \t0.0 < beta < 1.0" << std::endl
    << "-r0    \tInteger radius of window, dimension 0" << std::endl
    << "-r1    \tInteger radius of window, dimension 1" << std::endl
    << "[-r2]  \tInteger radius of window, dimension 2" << std::endl
    << "[-LUT] \tUse Lookup-table <true, false>;" << std::endl
    << "default = true; Faster, but requires more memory.";
  return ss.str();
} // end PrintUsageString


/** ContrastEnhanceImage */

class ITKToolsContrastEnhanceImageBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsContrastEnhanceImageBase()
  {
    m_Alpha = 0.0f;
    m_Beta = 0.0f;
    m_InputFileName = "";
    m_OutputFileName = "";
    m_LookUpTable = false;
    //m_Radius; // does this need to be initialized?
  };
  
  ~ITKToolsContrastEnhanceImageBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  float m_Alpha;
  float m_Beta;
  bool m_LookUpTable;
  std::vector<unsigned int> m_Radius;
}; // end ContrastEnhanceImageBase


template< class TComponentType, unsigned int VImageDimension >
class ITKToolsContrastEnhanceImage : public ITKToolsContrastEnhanceImageBase
{
public:
  typedef ITKToolsContrastEnhanceImage Self;

  ITKToolsContrastEnhanceImage(){};
  ~ITKToolsContrastEnhanceImage(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VImageDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    const unsigned int ImageDimension =           VImageDimension;
    typedef itk::Image<TComponentType, ImageDimension> ImageType;
    typedef typename ImageType::Pointer           ImagePointer;
    typedef typename ImageType::IndexType         IndexType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::RegionType        RegionType;
    typedef typename ImageType::PointType         PointType;
    typedef itk::ImageFileReader<ImageType>       ReaderType;
    typedef itk::ImageFileWriter<ImageType>       WriterType;
    typedef typename ReaderType::Pointer          ReaderPointer;
    typedef typename WriterType::Pointer          WriterPointer;
    typedef itk::AdaptiveHistogramEqualizationImageFilter<
      ImageType>                                  EnhancerType;
    typedef typename EnhancerType::Pointer        EnhancerPointer;
    typedef typename EnhancerType::ImageSizeType  RadiusType;

    /** vars */
    WriterPointer writer = WriterType::New();
    EnhancerPointer enhancer = EnhancerType::New();


    std::string inputImageFileName = "";
    std::string outputImageFileName = "";

    /** Try to read input image */
    ReaderPointer reader = ReaderType::New();
    reader->SetFileName( m_InputFileName.c_str() );
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

    enhancer->SetUseLookupTable(m_LookUpTable);
    enhancer->SetAlpha(m_Alpha);
    enhancer->SetBeta(m_Beta);
    
    itk::Size<VImageDimension> radiusSize;
    for(unsigned int i = 0; i < VImageDimension; ++i)
      {
      radiusSize[i] = m_Radius[i];
      }
    enhancer->SetRadius(radiusSize);
    enhancer->SetInput( reader->GetOutput() );
    writer->SetInput( enhancer->GetOutput() );
    writer->SetFileName(m_OutputFileName.c_str());

    /** do it. */
    std::cout
      << "Saving image to disk as \""
      << m_OutputFileName
      << "\""
      << std::endl;
    try
    {
      writer->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << err << std::endl;
    }

  }

}; // end ContrastEnhanceImage

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-alpha", "Alpha." );
  parser->MarkArgumentAsRequired( "-beta", "Beta." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  // Get image properties
  
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );
  
  unsigned int imageDimension;
  itktools::GetImageDimension(inputFileName, imageDimension);
 
  itktools::ComponentType componentType;
  itktools::GetImageComponentType(inputFileName, componentType);
  
  // Parse other arguments
  std::string outputFileName;
  parser->GetCommandLineArgument( "-out", outputFileName);
  
  float alpha = 0.0f;
  parser->GetCommandLineArgument( "-alpha", alpha);
  
  float beta = 0.0f;
  parser->GetCommandLineArgument( "-beta", beta);
    
  bool lookUpTable = true;
  parser->GetCommandLineArgument( "-LUT", lookUpTable);

  std::vector<unsigned int> radius;
  parser->GetCommandLineArgument( "-r", radius);
  
  /** Class that does the work */
  ITKToolsContrastEnhanceImageBase * contrastEnhanceImage = NULL; 

  try
  {    
    // now call all possible template combinations.
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< short, 2 >::New( componentType, imageDimension );
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< char, 2 >::New( componentType, imageDimension );
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< unsigned char, 2 >::New( componentType, imageDimension );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< short, 3 >::New( componentType, imageDimension );
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< char, 3 >::New( componentType, imageDimension );
    if (!contrastEnhanceImage) contrastEnhanceImage = ITKToolsContrastEnhanceImage< unsigned char, 3 >::New( componentType, imageDimension );
#endif
    if (!contrastEnhanceImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << imageDimension
        << std::endl;
      return 1;
    }

    contrastEnhanceImage->m_InputFileName = inputFileName;
    contrastEnhanceImage->m_OutputFileName = outputFileName;
    contrastEnhanceImage->m_Alpha = alpha;
    contrastEnhanceImage->m_Beta = beta;
    contrastEnhanceImage->m_LookUpTable = lookUpTable;
    contrastEnhanceImage->m_Radius = radius;

    contrastEnhanceImage->Run();
    
    delete contrastEnhanceImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete contrastEnhanceImage;
    return 1;
  }
  
  return EXIT_SUCCESS;

} // end function main

#endif // #ifndef __contrastenhanceimage_cxx
