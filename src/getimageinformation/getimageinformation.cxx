#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageFileReader.h"
#include <iomanip>

/** Declare PrintHelp. */
void PrintHelp(void);

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 4 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

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

  /** Check if the required arguments are given. */
  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
    return 1;
  }

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



  /**
   * ******************* PrintHelp *******************
   */
void PrintHelp()
{
  std::cout << "Usage:\n" << "pxgetimageinformation\n";
  std::cout << "  -in      inputFileName\n";
  std::cout << "  [-dim]   dimension\n";
  std::cout << "  [-pt]    pixelType\n";
  std::cout << "  [-ct]    componentType\n";
  std::cout << "  [-noc]   #components\n";
  std::cout << "  [-sz]    size\n";
  std::cout << "  [-sp]    spacing\n";
  std::cout << "  [-vol]   voxel volume\n";
  std::cout << "  [-o]     origin\n";
  std::cout << "  [-dc]    direction cosines\n";
  std::cout << "  [-all]   all of the above\n";
  std::cout << "Image information about the inputFileName is printed to screen.\n";
  std::cout << "Only one option should be given, e.g. -sp, then the spacing is printed.\n";
  std::cout << "  [-i]     index, if this option is given only e.g. "
    << "spacing[index] is printed." << std::endl;

} // end PrintHelp()
