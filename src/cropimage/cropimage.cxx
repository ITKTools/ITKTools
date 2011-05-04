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
 \brief Crop an image.
 
 \verbinclude cropimage.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImage.h"
#include "itkCropImageFilter.h"
#include "itkConstantPadImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, input1, input2, option, force ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Declare CropImage. */
template< class InputImageType >
void CropImage(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<int> & input1,
  const std::vector<int> & input2,
  const unsigned int option,
  const bool force );

/** Declare other functions. */
std::string PrintHelp( void );

bool CheckWhichInputOption( const bool pAGiven, const bool pBGiven, const bool szGiven,
  const bool lbGiven, const bool ubGiven, unsigned int & arg );

bool ProcessArgument( std::vector<int> & arg, const unsigned int dimension, const bool positive );

void GetBox( std::vector<int> & pA, std::vector<int> & pB, const unsigned int dimension );

std::vector<int> GetLowerBoundary( const std::vector<int> & input1,
   const unsigned int dimension, const bool force, std::vector<unsigned long> & padLowerBound );

std::vector<int> GetUpperBoundary( const std::vector<int> & input1,
   const std::vector<int> & input2, const std::vector<int> & imageSize,
   const unsigned int dimension, const unsigned int option,
   const bool force, std::vector<unsigned long> & padUpperBound );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

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

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "CROPPED.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::vector<int> pA;
  bool retpA = parser->GetCommandLineArgument( "-pA", pA );

  std::vector<int> pB;
  bool retpB = parser->GetCommandLineArgument( "-pB", pB );

  std::vector<int> sz;
  bool retsz = parser->GetCommandLineArgument( "-sz", sz );

  std::vector<int> lowBound;
  bool retlb = parser->GetCommandLineArgument( "-lb", lowBound );

  std::vector<int> upBound;
  bool retub = parser->GetCommandLineArgument( "-ub", upBound );

  bool force = parser->ArgumentExists( "-force" );

  /** Determine image properties. */
  std::string ComponentTypeIn = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = GetImageProperties(
    inputFileName,
    PixelType,
    ComponentTypeIn,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for vector images. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check which input option is used:
   * 1: supply two points with -pA and -pB
   * 2: supply a points and a size with -pA and -sz
   * 3: supply a lower and an upper bound with -lb and -ub
   */
  unsigned int option = 0;
  if ( !CheckWhichInputOption( retpA, retpB, retsz, retlb, retub, option ) )
  {
    std::cerr << "ERROR: Check your commandline arguments." << std::endl;
    return 1;
  }

  /** Check argument pA. Point A should only be positive if not force. */
  if ( retpA )
  {
    if ( !ProcessArgument( pA, Dimension, force ) )
    {
      std::cout << "ERROR: Point A should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument pB. Point B should always be positive. */
  if ( retpB )
  {
    if ( !ProcessArgument( pB, Dimension, false ) )
    {
      std::cout << "ERROR: Point B should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument sz. Size should always be positive. */
  if ( retsz )
  {
    if ( !ProcessArgument( sz, Dimension, false ) )
    {
      std::cout << "ERROR: The size sz should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument lb. */
  if ( retlb )
  {
    if ( !ProcessArgument( lowBound, Dimension, force ) )
    {
      std::cout << "ERROR: The lowerbound lb should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Check argument ub. */
  if ( retub )
  {
    if ( !ProcessArgument( upBound, Dimension, force ) )
    {
      std::cout << "ERROR: The upperbound ub should consist of 1 or Dimension positive values." << std::endl;
      return 1;
    }
  }

  /** Get inputs. */
  std::vector<int> input1, input2;
  if ( option == 1 )
  {
    GetBox( pA, pB, Dimension );
    input1 = pA;
    input2 = pB;
  }
  else if ( option == 2 )
  {
    input1 = pA;
    input2 = sz;
  }
  else if ( option == 3 )
  {
    input1 = lowBound;
    input2 = upBound;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( CropImage, unsigned char, 2 );
    run( CropImage, char, 2 );
    run( CropImage, unsigned short, 2 );
    run( CropImage, short, 2 );
    run( CropImage, unsigned int, 2 );
    run( CropImage, int, 2 );
    run( CropImage, unsigned long, 2 );
    run( CropImage, long, 2 );
    run( CropImage, float, 2 );
    run( CropImage, double, 2 );

    run( CropImage, unsigned char, 3 );
    run( CropImage, char, 3 );
    run( CropImage, unsigned short, 3 );
    run( CropImage, short, 3 );
    run( CropImage, unsigned int, 3 );
    run( CropImage, int, 3 );
    run( CropImage, unsigned long, 3 );
    run( CropImage, long, 3 );
    run( CropImage, float, 3 );
    run( CropImage, double, 3 );
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  if ( !supported )
  {
    std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
    std::cerr
      << "pixel (component) type = " << ComponentTypeIn
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main()


  /**
   * ******************* CropImage *******************
   */

template< class InputImageType >
void CropImage( const std::string & inputFileName, const std::string & outputFileName,
  const std::vector<int> & input1, const std::vector<int> & input2,
  const unsigned int option, const bool force )
{
  /** Typedefs. */
  typedef itk::CropImageFilter< InputImageType, InputImageType >        CropImageFilterType;
  typedef itk::ConstantPadImageFilter< InputImageType, InputImageType > PadFilterType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< InputImageType >      WriterType;
  typedef typename InputImageType::SizeType           SizeType;

  const unsigned int Dimension = InputImageType::ImageDimension;

  /** Declarations. */
  typename CropImageFilterType::Pointer cropFilter = CropImageFilterType::New();
  typename PadFilterType::Pointer padFilter = PadFilterType::New();
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();

  /** Prepare stuff. */
  SizeType input1Size, input2Size;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    input1Size[ i ] = input1[ i ];
    input2Size[ i ] = input2[ i ];
  }

  /** Read the image. */
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();

  /** Get the size of input image. */
  SizeType imageSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  std::vector<int> imSize( Dimension );
  for ( unsigned int i = 0; i < Dimension; i++ ) imSize[ i ] = static_cast<int>( imageSize[ i ] );

  /** Get the lower and upper boundary. */
  std::vector<unsigned long> padLowerBound, padUpperBound;
  std::vector<int> down = GetLowerBoundary(
    input1, Dimension, force, padLowerBound );
  std::vector<int> up = GetUpperBoundary(
    input1, input2, imSize, Dimension, option, force, padUpperBound );
  SizeType downSize, upSize;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    downSize[ i ] = down[ i ];
    upSize[ i ] = up[ i ];
  }

  /** Set the boundaries for the cropping filter. */
  cropFilter->SetInput( reader->GetOutput() );
  cropFilter->SetLowerBoundaryCropSize( downSize );
  cropFilter->SetUpperBoundaryCropSize( upSize );

  /** In case the force option is set to true, we force the output image to be of the
   * desired size.
   */
  if ( force )
  {
    unsigned long uBound[ Dimension ];
    unsigned long lBound[ Dimension ];
    for ( unsigned int i = 0; i < Dimension; i++ )
    {
      lBound[ i ] = padLowerBound[ i ];
      uBound[ i ] = padUpperBound[ i ];
    }
    padFilter->SetPadLowerBound( lBound );
    padFilter->SetPadUpperBound( uBound );
    padFilter->SetInput( cropFilter->GetOutput() );
    writer->SetInput( padFilter->GetOutput() );
  }
  else
  {
    writer->SetInput( cropFilter->GetOutput() );
  }

  /** Setup and process the pipeline. */
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

} // end CropImage()


  /**
   * ******************* PrintHelp *******************
   */

std::string PrintHelp( void )
{
  std::string helpString = "Usage: \
  pxcropimage \
    -in      inputFilename \
    [-out]   outputFilename, default in + CROPPED.mhd \
    [-pA]    a point A \
    [-pB]    a point B \
    [-sz]    size \
    [-lb]    lower bound \
    [-ub]    upper bound \
    [-force] force to extract a region of size sz, pad if necessary \
  pxcropimage can be called in different ways: \
    1: supply two points with \"-pA\" and \"-pB\". \
    2: supply a points and a size with \"-pA\" and \"-sz\". \
    3: supply a lower and an upper bound with \"-lb\" and \"-ub\". \
  The points are supplied in index coordinates. \
  Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double.";
  return helpString;
} // end PrintHelp()


  /**
   * ******************* CheckWhichInputOption *******************
   *
   * 1: supply two points with -pA and -pB
   * 2: supply a points and a size with -pA and -sz
   * 3: supply a lower and an upper bound with -lb and -ub
   */
bool CheckWhichInputOption( const bool pAGiven, const bool pBGiven, const bool szGiven,
  const bool lbGiven, const bool ubGiven, unsigned int & arg )
{
  if ( pAGiven && pBGiven && !szGiven && !lbGiven && !ubGiven )
  {
    /** Two points given. */
    arg = 1;
    return true;
  }
  else if ( pAGiven && !pBGiven && szGiven && !lbGiven && !ubGiven )
  {
    /** A point and a size given. */
    arg = 2;
    return true;
  }
  else if ( !pAGiven && !pBGiven && !szGiven && lbGiven && ubGiven )
  {
    /** A lower and an upper bound given. */
    arg = 3;
    return true;
  }

  /** Return a value. */
  return false;

} // end CheckWhichInputOption()


  /**
   * ******************* ProcessArgument *******************
   */

bool ProcessArgument( std::vector<int> & arg, const unsigned int dimension, const bool positive )
{
  /** Check if arg is of the right size. */
  if ( arg.size() != dimension && arg.size() != 1 )
  {
    return false;
  }

  /** Create a vector arg2 of size dimension, with values:
   * - ( arg[0], ..., arg[0] ) if arg.size() == 1
   * - ( arg[0], ..., arg[dimension-1] ) if arg.size() == dimension
   */
  std::vector<int> arg2( dimension, arg[ 0 ] );
  if ( arg.size() == dimension )
  {
    for ( unsigned int i = 1; i < dimension; i++ )
    {
      arg2[ i ] = arg[ i ];
    }
  }

  /** Substitute arg2 for arg. */
  arg = arg2;

  /** Check for positive numbers. */
  if ( !positive )
  {
    for ( unsigned int i = 0; i < dimension; i++ )
    {
      if ( arg[ i ] < 0 ) return false;
    }
  }

  /** Return a value. */
  return true;

} // end ProcessArgument()


  /**
   * ******************* GetBox *******************
   */

void GetBox( std::vector<int> & pA, std::vector<int> & pB, unsigned int dimension )
{
  /** Get the outer points of the box. */
  std::vector<int> pa( dimension, 0 );
  std::vector<int> pb( dimension, 0 );
  for ( unsigned int i = 0; i < dimension; i++ )
  {
    pa[ i ] = vnl_math_min( pA[ i ], pB[ i ] );
    pb[ i ] = vnl_math_max( pA[ i ], pB[ i ] );
  }

  /** Copy to the input variables. */
  pA = pa; pB = pb;

} // end GetBox()


  /**
   * ******************* GetLowerBoundary *******************
   */

std::vector<int> GetLowerBoundary( const std::vector<int> & input1,
   const unsigned int dimension, const bool force, std::vector<unsigned long> & padLowerBound )
{
  /** Create output vector. */
  std::vector<int> lowerBoundary( input1 );
  padLowerBound.resize( dimension, 0 );
  if ( !force ) return lowerBoundary;

  /** Fill output vector. */
  for ( unsigned int i = 0; i < dimension; i++ )
  {
    if ( input1[ i ] < 0 )
    {
      lowerBoundary[ i ] = 0;
      padLowerBound[ i ] = -input1[ i ];
    }
  }

  /** Return output. */
  return lowerBoundary;

} // end GetLowerBoundary()


  /**
   * ******************* GetUpperBoundary *******************
   */

 std::vector<int> GetUpperBoundary( const std::vector<int> & input1,
   const std::vector<int> & input2, const std::vector<int> & imageSize,
   const unsigned int dimension, const unsigned int option,
   const bool force, std::vector<unsigned long> & padUpperBound )
{
  /** Create output vector. */
  std::vector<int> upperBoundary( dimension, 0 );
  padUpperBound.resize( dimension, 0 );

  /** Fill output vector. */
  if ( option == 1 )
  {
    for ( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = imageSize[ i ] - input2[ i ];
      if ( imageSize[ i ] < input2[ i ] )
      {
        if ( force )
        {
          upperBoundary[ i ] = 0;
          padUpperBound[ i ] = input2[ i ] - imageSize[ i ];
        }
        else
        {
          itkGenericExceptionMacro( << "out of bounds." );
        }
      }
      if ( input1[ i ] == input2[ i ] )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  }
  else if ( option == 2 )
  {
    for ( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = imageSize[ i ] - input1[ i ] - input2[ i ];
      if ( imageSize[ i ] < input1[ i ] + input2[ i ] )
      {
        if ( force )
        {
          upperBoundary[ i ] = 0;
          padUpperBound[ i ] = input1[ i ] + input2[ i ] - imageSize[ i ];
        }
        else
        {
          itkGenericExceptionMacro( << "out of bounds." );
        }
      }
      if ( input2[ i ] == 0 )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  }
  else if ( option == 3 )
  {
    for ( unsigned int i = 0; i < dimension; i++ )
    {
      upperBoundary[ i ] = input2[ i ];
      if ( input2[ i ] < 0 )
      {
        upperBoundary[ i ] = 0;
        padUpperBound[ i ] = -input2[ i ];
      }
      if ( imageSize[ i ] < input1[ i ] + input2[ i ] ) // crossing
      {
        itkGenericExceptionMacro( << "out of bounds." );
      }
      if ( input1[ i ] + input2[ i ] == imageSize[ i ] )
      {
        itkGenericExceptionMacro( << "size[" << i << "] = 0" );
      }
    }
  } // end if

  /** Return output. */
  return upperBoundary;

} // end GetUpperBoundary()
