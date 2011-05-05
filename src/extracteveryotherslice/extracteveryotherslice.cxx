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
 \brief Extract every other slice of an image.
 
 \verbinclude extracteveryotherslice.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,type,dim) \
if ( ComponentTypeIn == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, everyOther, offset, direction ); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/* Declare ExtractEveryOtherSlice. */
template< class InputImageType >
void ExtractEveryOtherSlice(
  const std::string & inputFileName,
  const std::string & outputFileName,
  unsigned int everyOther,
  unsigned int offset,
  unsigned int direction );

/** Declare GetHelpString. */
std::string GetHelpString( void );

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

  std::string outputFileName = inputFileName.substr( 0, inputFileName.rfind( "." ) );
  outputFileName += "EveryOtherKExtracted.mhd";
  parser->GetCommandLineArgument( "-out", outputFileName );

  unsigned int everyOther = 2;
  parser->GetCommandLineArgument( "-K", everyOther );

  unsigned int offset = 0;
  parser->GetCommandLineArgument( "-of", offset );
  offset = offset % everyOther;

  unsigned int direction = 2;
  parser->GetCommandLineArgument( "-d", direction );

  /** Check everyOther. */
  if ( everyOther < 2 )
  {
    std::cout << "ERROR: K should be larger than 1." << std::endl;
    return 1;
  }

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

  /** Check for dimension. */
  if ( Dimension != 3 )
  {
    std::cerr << "ERROR: The image dimension equals " << Dimension << "." << std::endl;
    std::cerr << "Only 3D images are supported." << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  ReplaceUnderscoreWithSpace( ComponentTypeIn );

  /** Check direction. */
  if ( direction + 1 > Dimension )
  {
    std::cout << "ERROR: direction should be 0, 1 or 2." << std::endl;
    return 1;
  }

  /** Run the program. */
  bool supported = false;
  try
  {
    run( ExtractEveryOtherSlice, unsigned char, 3 );
    run( ExtractEveryOtherSlice, char, 3 );
    run( ExtractEveryOtherSlice, unsigned short, 3 );
    run( ExtractEveryOtherSlice, short, 3 );
    run( ExtractEveryOtherSlice, float, 3 );
    run( ExtractEveryOtherSlice, double, 3 );
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
      << "pixel (component) type = " << PixelType
      << " ; dimension = " << Dimension
      << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main


  /*
   * ******************* ExtractEveryOtherSlice *******************
   *
   * The ExtractEveryOtherSlice function templated over the input pixel type.
   */

template< class InputImageType >
void ExtractEveryOtherSlice(
  const std::string & inputFileName,
  const std::string & outputFileName,
  unsigned int everyOther,
  unsigned int offset,
  unsigned int direction )
{
  /** Typedefs. */
  typedef itk::ImageSliceConstIteratorWithIndex<
    InputImageType >                                  SliceConstIteratorType;
  typedef itk::ImageSliceIteratorWithIndex<
    InputImageType >                                  SliceIteratorType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< InputImageType >      WriterType;
  typedef typename InputImageType::RegionType         RegionType;
  typedef typename RegionType::IndexType              IndexType;
  typedef typename InputImageType::SizeType           SizeType;

  /** Read in the inputImage. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();

  /** Define size of output image. */
  SizeType sizeIn = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  SizeType sizeOut = sizeIn;
  float newSize = vcl_ceil(
    ( static_cast<float>( sizeOut[ direction ] - offset ) )
    / static_cast<float>( everyOther ) );
  sizeOut[ direction ] = static_cast<unsigned int>( newSize );

  /** Define region of output image. */
  RegionType region;
  region.SetIndex( reader->GetOutput()->GetLargestPossibleRegion().GetIndex() );
  region.SetSize( sizeOut );

  /** Create output image. */
  typename InputImageType::Pointer outputImage = InputImageType::New();
  outputImage->SetSpacing( reader->GetOutput()->GetSpacing() );
  outputImage->SetOrigin( reader->GetOutput()->GetOrigin() );
  outputImage->SetRegions( region );
  outputImage->Allocate();

  /** Create iterators. */
  SliceConstIteratorType itIn( reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion() );
  SliceIteratorType itOut( outputImage, outputImage->GetLargestPossibleRegion() );

  /** Set direction, default slice = z. */
  if ( direction == 0 )
  {
    itIn.SetFirstDirection(1);
    itIn.SetSecondDirection(2);
    itOut.SetFirstDirection(1);
    itOut.SetSecondDirection(2);
  }
  else if ( direction == 1 )
  {
    itIn.SetFirstDirection(0);
    itIn.SetSecondDirection(2);
    itOut.SetFirstDirection(0);
    itOut.SetSecondDirection(2);
  }
  else if ( direction == 2 )
  {
    itIn.SetFirstDirection(0);
    itIn.SetSecondDirection(1);
    itOut.SetFirstDirection(0);
    itOut.SetSecondDirection(1);
  }

  /** Initialize iterators. */
  itIn.GoToBegin();
  itOut.GoToBegin();
  IndexType index= itIn.GetIndex();
  index[ direction ] += offset;
  itIn.SetIndex( index );

  /** Loop over images. */
  while( !itOut.IsAtEnd() )
  {
    while( !itOut.IsAtEndOfSlice() )
    {
      while( !itOut.IsAtEndOfLine() )
      {
        itOut.Set( itIn.Get() );
        ++itIn;
        ++itOut;
      }
      itIn.NextLine();
      itOut.NextLine();
    }
    itIn.NextSlice();
    itOut.NextSlice();

    /** Skip some slices in inputImage. */
    index = itIn.GetIndex();
    for ( unsigned int i = 1; i < everyOther; i++ )
    {
      index[ direction ]++;
    }
    itIn.SetIndex( index );
  }

  /** Write the output image. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( outputImage );
  writer->Update();

} // end ExtractEveryOtherSlice()


  /**
   * ******************* GetHelpString *******************
   */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxextracteveryotherslice" << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename, default in + EveryOtherKExtracted.mhd" << std::endl
  << "  [-K]     every other slice K, default 2" << std::endl
  << "  [-of]    offset, default 0" << std::endl
  << "  [-d]     direction, default is z-axes" << std::endl
  << "Supported: 3D, (unsigned) char, (unsigned) short, float, double.";
  return ss.str();

} // end GetHelpString()

