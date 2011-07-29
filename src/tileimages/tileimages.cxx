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
 \brief Either tiles a stack of 2D images into a 3D image, or tiles nD images to form another nD image.

 This program tiles a stacks of 2D images into a 3D image.
 This is done by employing an itk::SeriesFileReader.
 \verbinclude tileimages.help
 */

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkTileImageFilter.h"
#include "itkImageFileWriter.h"

#include "TileImages.h"
#include "TileImages2D3D.h"

#include <vector>
#include <string>


/** Define GetHelpString. */
std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "pxtileimages EITHER tiles a stack of 2D images into a 3D image," << std::endl
  << "OR tiles nD images to form another nD image." << std::endl
  << "In the last case the way to tile is specified by a layout." << std::endl
  << "To stack a pile of 2D images an itk::SeriesFileReader is employed." << std::endl
  << "If no layout is specified with \"-ly\" 2D-3D tiling is done," << std::endl
  << "otherwise 2D-2D or 3D-3D tiling is performed." << std::endl
  << "Usage:  \npxtileimages" << std::endl
  << "  -in      input image filenames, at least 2" << std::endl
  << "  -out     output image filename" << std::endl
  << "  [-pt]    pixel type of input and output images" << std::endl
  << "           default: automatically determined from the first input image" << std::endl
  << "  [-sp]    spacing in z-direction for 2D-3D tiling [double];" << std::endl
  << "           if omitted, the origins of the 2d images are used to find the spacing;" << std::endl
  << "           if these are identical, a spacing of 1.0 is assumed" << std::endl
  << "  [-ly]    layout of the nD-nD tiling" << std::endl
  << "           example: in 2D for 4 images \"-ly 2 2\" results in" << std::endl
  << "             im1 im2" << std::endl
  << "             im3 im4" << std::endl
  << "           example: in 2D for 4 images \"-ly 4 1\" (or \"-ly 0 1\") results in" << std::endl
  << "             im1 im2 im3 im4" << std::endl
  << "  [-d]     default value, by default 0." << std::endl
  << "Supported pixel types: (unsigned) char, (unsigned) short, float.";

  return ss.str();

} // end GetHelpString


//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
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
  
  /** Get the input file names. */
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  if ( inputFileNames.size() < 2 )
  {
    std::cout << "ERROR: You should specify at least two input images." << std::endl;
    return 1;
  }

  /** Get the outputFileName. */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  /** Read the z-spacing. */
  double zspacing = -1.0;
  parser->GetCommandLineArgument( "-sp", zspacing );

  /** Get the layout. */
  std::vector< unsigned int > layout;
  bool retly = parser->GetCommandLineArgument( "-ly", layout );

  /** Get the layout. */
  double defaultvalue = 0.0;
  parser->GetCommandLineArgument( "-d", defaultvalue );

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileNames[ 0 ],
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );

  if ( retgip != 0 )
  {
    return 1;
  }

  std::cout << "The first input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** Let the user overrule this. */
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if ( retpt )
  {
    std::cout << "The user has overruled this by specifying -pt:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Run the program. */
  

  if ( !retly )
  {
    
    /** Class that does the work */
    TileImages2D3DBase * tileImages2D3D = NULL;

    itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);
    
    std::cout << "Detected component type: " << 
      componentType << std::endl;

    try
    {    
      // now call all possible template combinations.
      if (!tileImages2D3D) tileImages2D3D = TileImages2D3D< unsigned char >::New( componentType );
      if (!tileImages2D3D) tileImages2D3D = TileImages2D3D< char >::New( componentType );
      if (!tileImages2D3D) tileImages2D3D = TileImages2D3D< unsigned short >::New( componentType );
      if (!tileImages2D3D) tileImages2D3D = TileImages2D3D< short >::New( componentType );
      if (!tileImages2D3D) tileImages2D3D = TileImages2D3D< float >::New( componentType );

      if (!tileImages2D3D) 
      {
	std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
	std::cerr
	  << "pixel (component) type = " << componentType
	  << std::endl;
	return 1;
      }

      tileImages2D3D->m_InputFileNames = inputFileNames;
      tileImages2D3D->m_OutputFileName = outputFileName;
      tileImages2D3D->m_Zspacing = zspacing;
      
      tileImages2D3D->Run();
      
      delete tileImages2D3D;  
    }
    catch( itk::ExceptionObject &e )
    {
      std::cerr << "Caught ITK exception: " << e << std::endl;
      delete tileImages2D3D;
      return 1;
    }

  }
  else
  {
      
    /** Class that does the work */
    TileImagesBase * tileImages = NULL; 

    /** Short alias */
    unsigned int dim = Dimension;
  
    /** \todo some progs allow user to override the pixel type, 
    * so we need a method to convert string to EnumComponentType */
    itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);
    
    std::cout << "Detected component type: " << 
      componentType << std::endl;
    
    try
    {    
      // now call all possible template combinations.
      if (!tileImages) tileImages = TileImages< unsigned char, 2 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< char, 2 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< unsigned short, 2 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< short, 2 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< float, 2 >::New( componentType, dim );
      
  #ifdef ITKTOOLS_3D_SUPPORT
      if (!tileImages) tileImages = TileImages< unsigned char, 3 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< char, 3 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< unsigned short, 3 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< short, 3 >::New( componentType, dim );
      if (!tileImages) tileImages = TileImages< float, 3 >::New( componentType, dim );
  #endif
      if (!tileImages) 
      {
	std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
	std::cerr
	  << "pixel (component) type = " << componentType
	  << " ; dimension = " << Dimension
	  << std::endl;
	return 1;
      }

      tileImages->m_InputFileNames = inputFileNames;
      tileImages->m_OutputFileName = outputFileName;
      tileImages->m_Layout = layout;
      tileImages->m_Defaultvalue = defaultvalue;

      tileImages->Run();
      
      delete tileImages;  
    }
    catch( itk::ExceptionObject &e )
    {
      std::cerr << "Caught ITK exception: " << e << std::endl;
      delete tileImages;
      return 1;
    }

  }
  
  /** Return a value. */
  return 0;

} // end main


//-------------------------------------------------------------------------------------
