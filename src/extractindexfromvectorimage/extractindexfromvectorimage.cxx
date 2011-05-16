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
 \brief This program extracts a user specified component from a vector image.
 
 \verbinclude extractindexfromvectorimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program extracts a user specified component from a vector image." << std::endl
  << "Usage:" << std::endl
  << "pxextractindexfromvectorimage" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + INDEXEXTRACTED.mhd" << std::endl
  << "  -ind     a valid index" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int," << std::endl
  << "long, float, double.";
  return ss.str();
} // end GetHelpString()


class ExtractIndexBase : public itktools::ITKToolsBase
{
public:
  ExtractIndexBase(){};
  ~ExtractIndexBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Indices;
  
}; // end ExtractIndexBase


template< class ComponentType, unsigned int Dimension >
class ExtractIndex : public ExtractIndexBase
{
public:
  typedef ExtractIndex Self;

  ExtractIndex(){};
  ~ExtractIndex(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<ComponentType>( componentType ) && Dimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Use vector image type that dynamically determines vector length: */
    typedef itk::VectorImage< ComponentType, Dimension >    VectorImageType;
    typedef itk::Image< ComponentType, Dimension >          ScalarImageType;
    typedef itk::ImageFileReader< VectorImageType >     ImageReaderType;
    typedef itk::VectorIndexSelectionCastImageFilter<
      VectorImageType, ScalarImageType >                      IndexExtractorType;
    typedef itk::ImageFileWriter< VectorImageType >           ImageWriterType;

    /** Read input image. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( m_InputFileName );
    reader->Update();

    /** Extract indices. */
    
    // Create the assembler
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    
    for(unsigned int i = 0; i < reader->GetOutput()->GetNumberOfComponentsPerPixel(); ++i)
    {
      typename IndexExtractorType::Pointer extractor = IndexExtractorType::New();
      extractor->SetInput( reader->GetOutput() );
      extractor->SetIndex( m_Indices[i] );
      extractor->Update();
      //extractor->DisconnectPipeline();
      
      imageToVectorImageFilter->SetNthInput(i, extractor->GetOutput());
    }

    imageToVectorImageFilter->Update();
  
    /** Write output image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( m_OutputFileName );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->Update();
  }

}; // end ReplaceVoxel
//-------------------------------------------------------------------------------------


/** Declare GetHelpString. */
std::string GetHelpString( void );

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-ind", "The index or indices to extract." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "INDEXEXTRACTED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  // We must get the number of components here so we can construct the indices vector appropriately
  unsigned int numberOfComponents = 1;
  itktools::GetImageNumberOfComponents(inputFileName, numberOfComponents);
  
  // Determine if the argument passed as -ind is a string containing "all" or not
  std::string indicesString;
  parser->GetCommandLineArgument( "-ind", indicesString );
  
  std::vector<unsigned int> indices;
  if(indicesString.compare("all") == 0)
  {
    // Add all channels to the list of channels to extract
    for(unsigned int i = 0; i < indices.size(); ++i)
    {
      indices.push_back(i);
    }
  }
  else
  {
    parser->GetCommandLineArgument( "-ind", indices );
  }
  
  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    numberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    std::cerr << "ERROR: error while getting image properties of the input image!" << std::endl;
    return 1;
  }

  /** Check for vector images. */
  if ( numberOfComponents == 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is 1!" << std::endl;
    std::cerr << "Cannot make extract index from a scalar image." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Sanity check. */
  for(unsigned int i = 0; i < indices.size(); ++i)
  {
    if ( indices[i] > numberOfComponents - 1 )
    {
      std::cerr << "ERROR: You selected index "
	<< indices[i] << ", where the input image only has "
	<< numberOfComponents << " components." << std::endl;
      return 1;
    }
  }

  /** Run the program. */
  
  ExtractIndexBase * extractIndex = 0;
  unsigned int dim = Dimension;
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName);
  try
  {
    // 2D
    if (!extractIndex) extractIndex = ExtractIndex< char, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned char, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< short, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned short, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< int, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned int, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< long, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned long, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< float, 2 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< double, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    // 3D
    if (!extractIndex) extractIndex = ExtractIndex< char, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned char, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< short, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned short, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< int, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned int, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< long, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< unsigned long, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< float, 3 >::New( componentType, dim );
    if (!extractIndex) extractIndex = ExtractIndex< double, 3 >::New( componentType, dim );
#endif
    if (!extractIndex)
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    extractIndex->m_InputFileName = inputFileName;
    extractIndex->m_OutputFileName = outputFileName;
    extractIndex->m_Indices = indices;

    extractIndex->Run();

    delete extractIndex;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete extractIndex;
    return 1;
  }

  /** End program. */
  return 0;

} // end main

