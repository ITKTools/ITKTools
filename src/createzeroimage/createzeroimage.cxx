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
 \brief Create a blank image.
 
 \verbinclude createzeroimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileWriter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxcreatezeroimage" << std::endl
    << "[-in]    inputFilename" << std::endl
    << "-out     outputFilename" << std::endl
    << "-sz      size" << std::endl
    << "[-sp]    spacing" << std::endl
    << "[-o]     origin" << std::endl
    << "[-dim]   dimension, default 3" << std::endl
    << "[-pt]    pixelType, default short" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, float, double.";
  return ss.str();
} // end GetHelpString()


/** CreateZeroImage */

class ITKToolsCreateZeroImageBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsCreateZeroImageBase()
  {
    this->m_OutputFileName = "";
    //std::vector<unsigned int> this->m_Size;
    //std::vector<double> this->m_Spacing;
    //std::vector<double> this->m_Origin;
  };
  ~ITKToolsCreateZeroImageBase(){};

  /** Input parameters */
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Size;
  std::vector<double> m_Spacing;
  std::vector<double> m_Origin;
    
}; // end CreateZeroImageBase


template< class TComponentType, unsigned int VDimension >
class ITKToolsCreateZeroImage : public ITKToolsCreateZeroImageBase
{
public:
  typedef ITKToolsCreateZeroImage Self;

  ITKToolsCreateZeroImage(){};
  ~ITKToolsCreateZeroImage(){};

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
    /** Typedefs. */
    typedef itk::Image<TComponentType, VDimension>ImageType;
    typedef itk::ImageFileWriter< ImageType >     WriterType;
    typedef typename ImageType::PixelType         PixelType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::SpacingType       SpacingType;
    typedef typename ImageType::PointType         OriginType;

    /** Prepare stuff. */
    SizeType    imSize;
    SpacingType imSpacing;
    OriginType  imOrigin;
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      imSize[ i ] = this->m_Size[ i ];
      imSpacing[ i ] = this->m_Spacing[ i ];
      imOrigin[ i ] = this->m_Origin[ i ];
    }

    /** Create image. */
    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions( imSize );
    image->SetOrigin( imOrigin );
    image->SetSpacing( imSpacing );
    image->Allocate();
    image->FillBuffer( itk::NumericTraits<PixelType>::Zero );

    /** Write the image. */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();
  }

}; // end CreateZeroImage

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  
  std::vector<std::string> exactlyOneArguments;
  exactlyOneArguments.push_back("-sz");
  exactlyOneArguments.push_back("-in");
  
  parser->MarkExactlyOneOfArgumentsAsRequired(exactlyOneArguments);

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
  std::string fileNameIn = "";
  bool retin = parser->GetCommandLineArgument( "-in", fileNameIn );

  std::string fileName = "";
  parser->GetCommandLineArgument( "-out", fileName );

  unsigned int Dimension = 3;
  parser->GetCommandLineArgument( "-dim", Dimension );

  std::string PixelType = "short";
  parser->GetCommandLineArgument( "-pt", PixelType );

  std::vector<unsigned int> size( Dimension, 0 );
  bool retsz = parser->GetCommandLineArgument( "-sz", size );

  std::vector<double> spacing( Dimension, 1.0 );
  bool retsp = parser->GetCommandLineArgument( "-sp", spacing );

  std::vector<double> origin( Dimension, 0.0 );
  bool reto = parser->GetCommandLineArgument( "-o", origin );

  std::vector<double> direction( Dimension * Dimension, 0.0 );
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    direction[ i * ( Dimension + 1 ) ] = 1.0;
  }

  parser->GetCommandLineArgument( "-d", direction );

  if ( retin )
  {
    /** Determine image properties. */
    std::string dummyPixelType; //we don't use this
    unsigned int NumberOfComponents = 1;
    int retgip = itktools::GetImageProperties(
      fileNameIn,
      dummyPixelType, PixelType, Dimension, NumberOfComponents,
      size, spacing, origin, direction );
    if ( retgip != 0 )
    {
      return 1;
    }
  }

  /** Get rid of the possible "_" in PixelType. */
  itktools::ReplaceUnderscoreWithSpace( PixelType );

  /** Check size, spacing and origin. */
  if ( retsz )
  {
    if( size.size() != Dimension && size.size() != 1 )
    {
      std::cout << "ERROR: The number of sizes should be 1 or Dimension." << std::endl;
      return 1;
    }
  }
  if ( retsp )
  {
    if( spacing.size() != Dimension && spacing.size() != 1 )
    {
      std::cout << "ERROR: The number of spacings should be 1 or Dimension." << std::endl;
      return 1;
    }
  }
  if ( reto )
  {
    if( origin.size() != Dimension && origin.size() != 1 )
    {
      std::cout << "ERROR: The number of origins should be 1 or Dimension." << std::endl;
      return 1;
    }
  }

  /** Check size and spacing for nonpositive numbers. */
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    if ( size[ i ] < 1 )
    {
      std::cerr << "ERROR: For each dimension the size should be at least 1." << std::endl;
      return 1;
    }
    if ( spacing[ i ] < 0.00001 )
    {
      std::cerr << "ERROR: No negative numbers are allowed in the spacing." << std::endl;
      return 1;
    }
  }


  /** Class that does the work */
  ITKToolsCreateZeroImageBase * createZeroImage = 0; 

  /** Short alias */
  unsigned int dim = Dimension;

  itktools::ComponentType componentType = itk::ImageIOBase::GetComponentTypeFromString( PixelType );

  try
  {    
    // now call all possible template combinations.
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< unsigned char, 2 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< char, 2 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< unsigned short, 2 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< short, 2 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< float, 2 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< double, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< unsigned char, 3 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< char, 3 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< unsigned short, 3 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< short, 3 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< float, 3 >::New( componentType, dim );
    if (!createZeroImage) createZeroImage = ITKToolsCreateZeroImage< double, 3 >::New( componentType, dim );
#endif
    if (!createZeroImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    createZeroImage->m_OutputFileName = fileName;
    createZeroImage->m_Size = size;
    createZeroImage->m_Spacing = spacing;
    createZeroImage->m_Origin = origin;

    createZeroImage->Run();
    
    delete createZeroImage;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createZeroImage;
    return 1;
  }
  

  /** End program. */
  return 0;

} // end main

