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
 \brief Create a grid image.
 
 \verbinclude creategridimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------



/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxcreategridimage" << std::endl
    << "[-in]    inputFilename, information about size, etc, is taken from it" << std::endl
    << "-out     outputFilename" << std::endl
    << "-sz      image size for each dimension" << std::endl
    << "[-sp]    image spacing, default 1.0" << std::endl
    << "-d       distance in pixels between two gridlines" << std::endl
    << "[-stack] for 3D images, create a stack of 2D images, default false" << std::endl
  << "Supported: 2D, 3D, short.";
  return ss.str();

} // end GetHelpString()


/** CreateGridImage */

class CreateGridImageBase : public itktools::ITKToolsBase
{ 
public:
  CreateGridImageBase(){};
  ~CreateGridImageBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;

  std::vector<unsigned int> m_ImageSize;
  std::vector<float> m_ImageSpacing;
  std::vector<unsigned int> m_Distance;
  bool m_Is2DStack;

    
}; // end CreateGridImageBase


template< unsigned int VDimension >
class CreateGridImage : public CreateGridImageBase
{
public:
  typedef CreateGridImage Self;

  CreateGridImage(){};
  ~CreateGridImage(){};

  static Self * New( unsigned int dim )
  {
    if ( VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedef's. */
    typedef unsigned char                                   PixelType;
    typedef itk::Image< PixelType, VDimension >              ImageType;
    typedef itk::ImageRegionIteratorWithIndex< ImageType >  IteratorType;
    typedef itk::ImageFileReader< ImageType >               ReaderType;
    typedef itk::ImageFileWriter< ImageType >               WriterType;

    typedef typename ImageType::SizeType    SizeType;
    typedef typename ImageType::IndexType   IndexType;
    typedef typename ImageType::SpacingType SpacingType;

    /* Create image and writer. */
    typename ImageType::Pointer  image  = ImageType::New();
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Get and set grid image information. */
    if ( m_InputFileName != "" )
    {
      reader->SetFileName( m_InputFileName.c_str() );
      reader->GenerateOutputInformation();

      SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
      image->SetRegions( size );
      image->SetSpacing( reader->GetOutput()->GetSpacing() );
      image->SetOrigin( reader->GetOutput()->GetOrigin() );
      image->SetDirection( reader->GetOutput()->GetDirection() );
    }
    else
    {
      SizeType    size;
      SpacingType spacing;
      for ( unsigned int i = 0; i < VDimension; ++i )
      {
	size[ i ] = m_ImageSize[ i ];
	spacing[ i ] = m_ImageSpacing[ i ];
      }
      image->SetRegions( size );
      image->SetSpacing( spacing );
    }

    /** Allocate image. */
    image->Allocate();

    /* Fill the image. */
    IteratorType  it( image, image->GetLargestPossibleRegion() );
    it.GoToBegin();
    IndexType ind;
    while ( !it.IsAtEnd() )
    {
      /** Check if on grid. */
      ind = it.GetIndex();
      bool onGrid = false;
      onGrid |= ind[ 0 ] % m_Distance[ 0 ] == 0;
      onGrid |= ind[ 1 ] % m_Distance[ 1 ] == 0;
      if ( VDimension == 3 && !m_Is2DStack )
      {
	if ( ind[ 2 ] % m_Distance[ 2 ] != 0 )
	{
	  onGrid = ind[ 0 ] % m_Distance[ 0 ] == 0;
	  onGrid &= ind[ 1 ] % m_Distance[ 1 ] == 0;
	}
      }
      /** Set the value and continue. */
      if ( onGrid ) it.Set( 1 );
      else it.Set( 0 );
      ++it;
    } // end while

    /* Write result to file. */
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( image );
    writer->Update();
  }

}; // end CreateGridImage

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "The output filename." );

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
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<unsigned int> imageSize;
  bool retsz = parser->GetCommandLineArgument( "-sz", imageSize );

  const bool is2DStack = parser->ArgumentExists( "-stack" );

  /** Check if the required arguments are given. */
  if ( ( !retin && !retsz ) || ( retin && retsz ) )
  {
    std::cerr << "ERROR: You should specify \"-in\" or \"-sz\"." << std::endl;
    return 1;
  }

  /** Check arguments: size. */
  if ( retsz )
  {
    for ( unsigned int i = 0; i < imageSize.size(); ++i )
    {
      if ( imageSize[ i ] == 0 )
      {
        std::cerr << "ERROR: image size[" << i << "] = 0." << std::endl;
        return 1;
      }
    }
  }

  /** Get desired grid image dimension. */
  unsigned int imageDimension = 3;
  if ( retsz )
  {
    imageDimension = imageSize.size();
  }
  else
  {
    /** Determine image properties. */
    std::string ComponentTypeIn = "short";
    std::string PixelType; //we don't use this
    unsigned int NumberOfComponents = 1;
    std::vector<unsigned int> imagesize( imageDimension, 0 );
    int retgip = itktools::GetImageProperties(
      inputFileName,
      PixelType,
      ComponentTypeIn,
      imageDimension,
      NumberOfComponents,
      imagesize );
    if ( retgip != 0 ) return 1;
  }

  /** Check arguments: dimensionality. */
  if ( imageDimension < 2 || imageDimension > 3 )
  {
    std::cerr << "ERROR: Only image dimensions of 2 or 3 are supported." << std::endl;
    return 1;
  }

  /** Get more arguments. */
  std::vector<float> imageSpacing( imageDimension, 1.0 );
  parser->GetCommandLineArgument( "-sp", imageSpacing );

  std::vector<unsigned int> distance( imageDimension, 1 );
  bool retd = parser->GetCommandLineArgument( "-d", distance );

  /** Check arguments: distance. */
  if ( !retd )
  {
    std::cerr << "ERROR: You should specify \"-d\"." << std::endl;
    return 1;
  }
  for ( unsigned int i = 0; i < distance.size(); ++i )
  {
    if ( distance[ i ] == 0 ) distance[ i ] = 1;
  }


  /** Class that does the work */
  CreateGridImageBase * createGridImage = NULL; 

  /** Short alias */
  unsigned int dim = imageDimension;
 
  try
  {    
    // now call all possible template combinations.
    if (!createGridImage) createGridImage = CreateGridImage< 2 >::New( dim );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createGridImage) createGridImage = CreateGridImage< 3 >::New( dim );    
#endif
    if (!createGridImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << " dimension = " << dim
        << std::endl;
      return 1;
    }

    createGridImage->m_InputFileName = inputFileName;
    createGridImage->m_OutputFileName = outputFileName;
    createGridImage->m_ImageSize = imageSize;
    createGridImage->m_ImageSpacing = imageSpacing;
    createGridImage->m_Distance = distance;
    createGridImage->m_Is2DStack = is2DStack;

    createGridImage->Run();
    
    delete createGridImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createGridImage;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main
