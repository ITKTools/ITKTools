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
 \brief Print information about the input image to screen.
 
 \verbinclude getimageinformation.help
 */
#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageFileReader.h"
#include <iomanip>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxgetimageinformation" << std::endl
  << "  -in      inputFileName" << std::endl
  << "  [-dim]   dimension" << std::endl
  << "  [-pt]    pixelType" << std::endl
  << "  [-ct]    componentType" << std::endl
  << "  [-noc]   #components" << std::endl
  << "  [-sz]    size" << std::endl
  << "  [-sp]    spacing" << std::endl
  << "  [-vol]   voxel volume" << std::endl
  << "  [-o]     origin" << std::endl
  << "  [-dc]    direction cosines" << std::endl
  << "  [-all]   all of the above" << std::endl
  << "Image information about the inputFileName is printed to screen." << std::endl
  << "Only one option should be given, e.g. -sp, then the spacing is printed." << std::endl
  << "  [-i]     index, if this option is given only e.g." << std::endl
  << "spacing[index] is printed.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

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

  int index = -1;
  bool reti = parser->GetCommandLineArgument( "-i", index );

  bool exdim = parser->ArgumentExists( "-dim" );
  bool expt = parser->ArgumentExists( "-pt" );
  bool exct = parser->ArgumentExists( "-ct" );
  bool exnoc = parser->ArgumentExists( "-noc" );
  bool exsz = parser->ArgumentExists( "-sz" );
  bool exind = parser->ArgumentExists( "-ind" );
  bool exsp = parser->ArgumentExists( "-sp" );
  bool exdc = parser->ArgumentExists( "-dc" );
  bool exvol = parser->ArgumentExists( "-vol" );
  bool exo = parser->ArgumentExists( "-o" );
  bool exall = parser->ArgumentExists( "-all" );

  /** Typedef's. */
  const unsigned int Dimension = 3;
  typedef short      PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageIOBase                    ImageIOBaseType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;

  /** Create a testReader and generate all information. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( inputFileName.c_str() );
  try
  {
    testReader->GenerateOutputInformation();
  }
  catch( itk::ExceptionObject  &  err  )
  {
    std::cerr  << "ExceptionObject caught !"  << std::endl;
    std::cerr  << err <<  std::endl;
    return 1;
  }

  /** Extract the ImageIO from the testReader. */
  ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();
  unsigned int dim = testImageIOBase->GetNumberOfDimensions();

  /** Check the index. */
  if ( index > static_cast<int>( dim ) - 1 )
  {
    std::cerr << "ERROR: index out of bounds." << std::endl;
    return 1;
  }

  /**
   * ************************ Print image information ***************
   */

  if ( !exall )
  {
    /** Print image dimension. */
    if ( exdim )
    {
      std::cout << dim;
      return 0;
    }

    /** Print image pixel type. */
    if ( expt )
    {
      //ReplaceUnderscoreWithSpace( PixelType );
      std::cout << testImageIOBase->GetPixelTypeAsString(
        testImageIOBase->GetPixelType() );
      return 0;
    }

    /** Print image component type. */
    if ( exct )
    {
      std::cout << testImageIOBase->GetComponentTypeAsString(
        testImageIOBase->GetComponentType() );
      return 0;
    }

    /** Print image number of components. */
    if ( exnoc )
    {
      std::cout << testImageIOBase->GetNumberOfComponents();
      return 0;
    }

    /** Print image size. */
    if ( exsz )
    {
      if ( reti )
      {
        std::cout << testImageIOBase->GetDimensions( index );
      }
      else
      {
        for ( unsigned int i = 0; i < dim - 1; i++ )
        {
          std::cout << testImageIOBase->GetDimensions( i ) << " ";
        }
        std::cout << testImageIOBase->GetDimensions( dim - 1 );
      }
      return 0;
    }

    /** Print image index. */
    if ( exind )
    {
      if ( reti )
      {
        std::cout << (testImageIOBase->GetIORegion().GetIndex())[ index ];
      }
      else
      {
        for ( unsigned int i = 0; i < dim - 1; i++ )
        {
          std::cout << (testImageIOBase->GetIORegion().GetIndex())[ i ] << " ";
        }
        std::cout << (testImageIOBase->GetIORegion().GetIndex())[ dim - 1 ];
      }
      return 0;
    }

    /** Print image spacing. */
    std::cout << std::fixed;
    std::cout << std::setprecision( 6 );
    if ( exsp )
    {
      if ( reti )
      {
        std::cout << testImageIOBase->GetSpacing( index );
      }
      else
      {
        for ( unsigned int i = 0; i < dim - 1; i++ )
        {
          std::cout << testImageIOBase->GetSpacing( i ) << " ";
        }
        std::cout << testImageIOBase->GetSpacing( dim - 1 );
      }
      return 0;
    }

    /** Print image voxel volume. */
    std::cout << std::fixed;
    std::cout << std::setprecision( 6 );
    if ( exvol )
    {
      double volume = 1.0;
      for ( unsigned int i = 0; i < dim; i++ )
      {
        volume *= testImageIOBase->GetSpacing( i );
      }
      std::cout << volume;
      return 0;
    }

    /** Print image origin. */
    if ( exo )
    {
      if ( reti )
      {
        std::cout << testImageIOBase->GetOrigin( index );
      }
      else
      {
        for ( unsigned int i = 0; i < dim - 1; i++ )
        {
          std::cout << testImageIOBase->GetOrigin( i ) << " ";
        }
        std::cout << testImageIOBase->GetOrigin( dim - 1 );
      }
      return 0;
    }

    /** Print image direction. */
    if ( exdc )
    {
      if ( reti )
      {
        std::vector<double> dir = testImageIOBase->GetDirection( index );
        for ( unsigned int i = 0; i < dim - 1; i++ )
        {
          std::cout << dir[ i ] << " ";
        }
        std::cout << dir[ dim - 1 ];
      }
      else
      {
        for ( unsigned int j = 0; j < dim; ++j )
        {
          std::vector<double> dir = testImageIOBase->GetDirection( j );
          for ( unsigned int i = 0; i < dim; i++ )
          {
            std::cout << dir[ i ] << " ";
          }
        }
      }
      return 0;
    }

  } // end don't print all

  /** Print all image information, i.e. all of the above. */
  else
  {
    std::cout << inputFileName << ":\n";

    std::cout << "dimension:      " << dim << "\n";

    std::cout << "pixel type:     " << testImageIOBase
      ->GetPixelTypeAsString( testImageIOBase->GetPixelType() ) << "\n";

    std::cout << "component type: " << testImageIOBase
      ->GetComponentTypeAsString( testImageIOBase->GetComponentType() ) << "\n";

    std::cout << "# components:   " << testImageIOBase
      ->GetNumberOfComponents() << "\n";

    std::cout << "size:           (";
    for ( unsigned int i = 0; i < dim - 1; i++ )
    {
      std::cout << testImageIOBase->GetDimensions( i ) << ", ";
    }
    std::cout << testImageIOBase->GetDimensions( dim - 1 ) << ")\n";

    std::cout << "spacing:        (";
    for ( unsigned int i = 0; i < dim - 1; i++ )
    {
      std::cout << testImageIOBase->GetSpacing( i ) << ", ";
    }
    std::cout << testImageIOBase->GetSpacing( dim - 1 ) << ")\n";

    std::cout << "origin:         (";
    for ( unsigned int i = 0; i < dim - 1; i++ )
    {
      std::cout << testImageIOBase->GetOrigin( i ) << ", ";
    }
    std::cout << testImageIOBase->GetOrigin( dim - 1 ) << ")\n";

    std::cout << "direction:      (";
    for ( unsigned int j = 0; j < dim - 1; ++j )
    {
      std::vector<double> dir = testImageIOBase->GetDirection( j );
      for ( unsigned int i = 0; i < dim; i++ )
      {
        std::cout << dir[ i ] << ", ";
      }
    }
    std::vector<double> dir = testImageIOBase->GetDirection( dim - 1 );
    for ( unsigned int i = 0; i < dim - 1; i++ )
    {
      std::cout << dir[ i ] << ", ";
    }
    std::cout << dir[ dim - 1  ] << ")\n";

    return 0;

  } // end print all information

  /** End program. */
  return 1;

} // end main
