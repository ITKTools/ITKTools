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
 \brief This program replaces the value of a user specified voxel.
 
 \verbinclude replacevoxel.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
//#include "itkChangeLabelImageFilter.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
  << "This program replaces the value of a user specified voxel." << std::endl
  << "Usage:" << std::endl
  << "pxreplacevoxel" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + VOXELREPLACED.mhd" << std::endl
  << "  -vox     input voxel index" << std::endl
  << "  -val     value that replaces the voxel" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int," << std::endl
  << "(unsigned) long, float, double.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

/** ReplaceVoxel */

class ITKToolsReplaceVoxelBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsReplaceVoxelBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    //std::vector<unsigned int> this->m_Voxel;
    this->m_Value = 0.0f;
  };
  ~ITKToolsReplaceVoxelBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Voxel;
  double m_Value;
    
}; // end ReplaceVoxelBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsReplaceVoxel : public ITKToolsReplaceVoxelBase
{
public:
  typedef ITKToolsReplaceVoxel Self;

  ITKToolsReplaceVoxel(){};
  ~ITKToolsReplaceVoxel(){};

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
    typedef TComponentType                         PixelType;
    typedef itk::Image< PixelType, VDimension >    ImageType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::IndexType         IndexType;
    typedef itk::ImageFileReader< ImageType >     ReaderType;
    typedef itk::ImageFileWriter< ImageType >     WriterType;

    /** Read in the input image. */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Read input image. */
    reader->SetFileName( this->m_InputFileName );
    reader->Update();
    typename ImageType::Pointer image = reader->GetOutput();

    /** Check size. */
    SizeType size = image->GetLargestPossibleRegion().GetSize();
    for ( unsigned int i = 0; i < VDimension; ++i )
    {
      if ( this->m_Voxel[ i ] < 0 || this->m_Voxel[ i ] > size[ i ] - 1 )
      {
        itkGenericExceptionMacro( << "ERROR: invalid voxel index." );
      }
    }

    /** Set the value to the voxel. */
    IndexType index;
    for ( unsigned int i = 0; i < VDimension; ++i )
    {
      index[ i ] = this->m_Voxel[ i ];
    }
    image->SetPixel( index, static_cast<PixelType>( this->m_Value ) );

    /** Write output image. */
    writer->SetFileName( this->m_OutputFileName );
    writer->SetInput( image );
    writer->Update();
  }

}; // end ReplaceVoxel

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-vox", "Voxel." );
  parser->MarkArgumentAsRequired( "-val", "Value." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "VOXELREPLACED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector< unsigned int > voxel;
  parser->GetCommandLineArgument( "-vox", voxel );

  double value = 0; // This is a required argument, but we must initialize it to prevent compiler warnings.
  parser->GetCommandLineArgument( "-val", value );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    std::cerr << "ERROR: error while getting image properties of the input image!" << std::endl;
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images not supported yet by this tool." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check if the specified voxel-size has Dimension number of components. */
  if ( voxel.size() != Dimension )
  {
    std::cerr << "ERROR: You should specify "
      << Dimension
      << " numbers with \"-vox\"." << std::endl;
    return 1;
  }

  /** Class that does the work */
  ITKToolsReplaceVoxelBase * replaceVoxel = 0; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType( inputFileName );
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!replaceVoxel) replaceVoxel = ITKToolsReplaceVoxel< short, 2 >::New( componentType, dim );
    if (!replaceVoxel) replaceVoxel = ITKToolsReplaceVoxel< float, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!replaceVoxel) replaceVoxel = ITKToolsReplaceVoxel< short, 3 >::New( componentType, dim );    
    if (!replaceVoxel) replaceVoxel = ITKToolsReplaceVoxel< float, 3 >::New( componentType, dim );
#endif
    if (!replaceVoxel) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    replaceVoxel->m_InputFileName = inputFileName;
    replaceVoxel->m_OutputFileName = outputFileName;
    replaceVoxel->m_Voxel = voxel;
    replaceVoxel->m_Value = value;

    replaceVoxel->Run();
    
    delete replaceVoxel;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete replaceVoxel;
    return 1;
  }

  /** End program. */
  return 0;

} // end main

