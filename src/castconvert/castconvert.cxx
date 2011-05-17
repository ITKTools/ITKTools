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
 \brief This program converts and possibly casts images.
 
 \verbinclude castconvert.help
 */
/*
 * authors:       Marius Staring and Stefan Klein
 *
 * Thanks to Hans J. Johnson for a modification to this program. This
 * modification breaks down the program into smaller compilation units,
 * so that the compiler does not overflow.
 *
 * ENH: on 23-05-2006 we added multi-component support.
 * ENH: on 09-06-2006 we added support for extracting a specific DICOM serie.
 */

#include <iostream>

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsBase.h"
#include "ITKToolsHelpers.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkImage.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageToVectorImageFilter.h"

// Some non-standard IO Factories
#include "itkGE4ImageIOFactory.h"
#include "itkGE5ImageIOFactory.h"
#include "itkGEAdwImageIOFactory.h"
//#include "itkBrains2MaskImageIOFactory.h"
#include "itkPhilipsRECImageIOFactory.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Description:\n"
     << "This is done by reading in an image, possibly casting of the image,\n"
     << "and subsequently writing the image to some format.\n"
     << "With converting we mean changing the extension of the image,\n"
     << "such as bmp, mhd, etc. With casting we mean changing the component\n"
     << "type of a voxel, such as short, unsigned long, float.\n"
     << "Casting is currently done for scalar images using the ShiftScaleImageFilter,\n"
     << "where values are mapped to itself, leaving the intensity range\n"
     << "the same. NOTE that when casting to a component type with a\n"
     << "smaller dynamic range, information might get lost. In this case\n"
     << "we might use the RescaleIntensityImageFilter to linearly\n"
     << "rescale the image values. For multi-component images, such as vector\n"
     << "or RGB images, casting is done using the itk::VectorCastImageFilter.\n"
     << "Currently supported are the SCALAR pixel types and also multi-\n"
     << "component pixel types, such as vector and RGB pixels. For multi-\n"
     << "component pixel types, everything is read in as an itk::Vector with\n"
     << "the correct pixel component type and number of components. This is\n"
     << "also the case for the writer.\n"
     << "Input images can be in all file formats ITK supports and for which\n"
     << "the ImageFileReader works, and additionally 3D dicom series\n"
     << "using the ImageSeriesReader. It is also possible to extract a specific\n"
     << "DICOM series from a directory by supplying the seriesUID. The pixel\n"
     << "component type should of course be a component type supported by the\n"
     << "file format. Output images can be in all file formats ITK supports and\n"
     << "for which the ImageFileReader works, so no dicom output is\n"
     << "currently supported.\n" << std::endl
     << "Usage:\n"
     << "pxcastconvert\n"
     << "  -in      inputfilename\n"
     << "  -out     outputfilename\n"
     << "  [-opct]  outputPixelComponentType, default equal to input\n"
     << "  [-z]     compression flag; if provided, the output image is compressed\n"
     << "OR pxcastconvert\n"
     << "  -in      dicomDirectory\n"
     << "  -out     outputfilename\n"
     << "  [-opct]  outputPixelComponentType, default equal to input\n"
     << "  [-s]     seriesUID, default the first UID found\n"
     << "  [-r]     add restrictions to generate a unique seriesUID\n"
     << "           e.g. \"0020|0012\" to add a check for acquisition number.\n"
     << "  [-z]     compression flag; if provided, the output image is compressed\n\n"
     << "OutputPixelComponentType should be one of {[unsigned_]char, [unsigned_]short,\n"
     << "  [unsigned_]int, [unsigned_]long, float, double}.\n"
     << "OutputPixelComponentType should additionally be supported by the output file format.\n"
     << "The compression flag \"-z\" may be ignored by some output image formats." << std::endl;

  return ss.str();

} // end GetHelpString()


/** CastConvert */

class ITKToolsCastConvertBase : public itktools::ITKToolsBase
{
public:
  ITKToolsCastConvertBase()
  {
    m_InputFileName = "";
    m_OutputFileName = "";
    m_UseCompression = false;
  };
  ~ITKToolsCastConvertBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  bool m_UseCompression;

}; // end CastConvertBase


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

  void Run(void)
  {
    typedef itk::VectorImage<double, VDimension>                InputVectorImageType;
    typedef itk::VectorImage<TOutputComponentType, VDimension>  OutputVectorImageType;

    typedef itk::Image<double, VDimension>                      InputScalarImageType;
    typedef itk::Image<TOutputComponentType, VDimension>        OutputScalarImageType;
    
    typedef typename itk::ImageFileReader< InputVectorImageType >     ImageReaderType;
    typedef typename itk::ImageFileWriter< OutputVectorImageType >    ImageWriterType;
    
    //typedef typename itk::RescaleIntensityImageFilter<
      //InputImageType, OutputImageType >                         RescaleFilterType;
    typedef typename itk::ShiftScaleImageFilter<
      InputScalarImageType, OutputScalarImageType >                         ShiftScaleFilterType;
    
    /** Create and setup the reader. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( m_InputFileName.c_str() );
    reader->Update();

    // Create the disassembler
    typedef itk::VectorIndexSelectionCastImageFilter<InputVectorImageType, InputScalarImageType> IndexSelectionType;
    typename IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
    indexSelectionFilter->SetInput(reader->GetOutput());

    // Create the re-assembler
    typedef itk::ImageToVectorImageFilter<OutputScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    // Apply the filter to each channel
    for(unsigned int channel = 0; channel < reader->GetOutput()->GetNumberOfComponentsPerPixel(); channel++)
      {
      // Extract the current channel
      indexSelectionFilter->SetIndex(channel);
      indexSelectionFilter->Update();

      // Cast the image
      //typename RescaleFilterType::Pointer caster = RescaleFilterType::New();
      typename ShiftScaleFilterType::Pointer caster = ShiftScaleFilterType::New();
      caster->SetShift( 0.0 );
      caster->SetScale( 1.0 );
      caster->SetInput( indexSelectionFilter->GetOutput() );
      caster->Update();
      
      // Reassemble the current channel
      imageToVectorImageFilter->SetNthInput(channel, caster->GetOutput());
      }

  imageToVectorImageFilter->Update();

  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(m_OutputFileName);
  writer->SetInput(imageToVectorImageFilter->GetOutput());
  writer->Update();
  }

}; // end CastConvert
//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Register some non-standard IO Factories to make the tool more useful.
   * Copied from the Insight Applications.
   */
  //itk::Brains2MaskImageIOFactory::RegisterOneFactory();
  itk::GE4ImageIOFactory::RegisterOneFactory();
  itk::GE5ImageIOFactory::RegisterOneFactory();
  itk::GEAdwImageIOFactory::RegisterOneFactory();
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();

  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

  /** Get the command line arguments. */
  bool useCompression = false;

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  
  /** Class that does the work */
  ITKToolsCastConvertBase * castConvert = NULL;

  /** Short alias */
  unsigned int dim = 0;
  itktools::GetImageDimension(inputFileName, dim);

  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName);

  try
  {
    if (!castConvert) castConvert = ITKToolsCastConvert< short, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned short, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< char, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned char, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< int, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned int, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< long, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned long, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< float, 2 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< double, 2 >::New( componentType, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!castConvert) castConvert = ITKToolsCastConvert< short, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned short, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< char, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned char, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< int, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned int, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< long, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< unsigned long, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< float, 3 >::New( componentType, dim );
    if (!castConvert) castConvert = ITKToolsCastConvert< double, 3 >::New( componentType, dim );
#endif
    if (!castConvert)
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << dim
        << std::endl;
      return 1;
    }

    castConvert->m_InputFileName = inputFileName;
    castConvert->m_OutputFileName = outputFileName;
    castConvert->m_UseCompression = useCompression;

    castConvert->Run();

    delete castConvert;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete castConvert;
    return 1;
  }
  /** End  program. Return success. */
  return 0;

}  // end main
