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
 \brief This program creates a signed distance transform.
 
 \verbinclude distancetransform.help
 */
#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

#include <string>

#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkMorphologicalSignedDistanceTransformImageFilter.h"
#include "itkMorphologicalDistanceTransformImageFilter.h"
//#include "itkOrderKDistanceTransformImageFilter.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "This program creates a signed distance transform." << std::endl
  << "Usage:" << std::endl
  << "pxdistancetransform" << std::endl
  << "  -in      inputFilename: the input image (a binary mask;" << std::endl
  << "threshold at 0 is performed if the image is not binary)." << std::endl
  << "  -out     outputFilename: the output of distance transform" << std::endl
  << "  [-s]     flag: if set, output squared distances instead of distances" << std::endl
  << "  [-m]     method, one of {Maurer, Danielsson, Morphological, MorphologicalSigned}, default Maurer" << std::endl
  << "Note: voxel spacing is taken into account. Voxels inside the" << std::endl
  << "object (=1) receive a negative distance." << std::endl
  << "Supported: 2D/3D. input: unsigned char, output: float";
  //std::cout << "  [-m]     method, one of {Maurer, Danielsson, OrderK}, default Maurer\n";
  //std::cout << "  [-K]     for method \"OrderK\", specify K, default 5\n";
  return ss.str();

} // end GetHelpString()

/* Declare DistanceTransform. */
template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames,
  bool outputSquaredDistance,
  const std::string & method,
  const unsigned int & K );

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
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
  
  /** Get the input segmentation file name (mandatory). */
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Get whether the squared distance should be returned instead of the
   * distance. Default: false, which is faster.
   */
  bool outputSquaredDistance = parser->ArgumentExists( "-s" );

  /** Get the outputFileName */
  std::vector<std::string> outputFileNames;
  parser->GetCommandLineArgument( "-out", outputFileNames );

  std::string method = "Maurer";
  parser->GetCommandLineArgument( "-m", method );

  unsigned int K = 5;
  parser->GetCommandLineArgument( "-k", K );

  /** Checks. */
  if ( method != "Maurer" && method != "Danielsson"
    && method != "Morphological" && method != "MorphologicalSigned" )
  {
    std::cerr << "ERROR: the method should be one of { Maurer, Danielsson, Morphological, MorphologicalSigned }!"
      << std::endl;
    return 1;
  }

  if ( method == "OrderK" && outputFileNames.size() != 3 )
  {
    std::cerr << "ERROR: the method OrderK requires three output file names!\n";
    std::cerr << "  You only specified " << outputFileNames.size() << "."
      << std::endl;
    return 1;
  }

  /** Determine image properties. */
  std::string ComponentType = "unsigned char";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip != 0 )
  {
    return 1;
  }

  /** Check for scalar image. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: vector images are not supported." << std::endl;
    return 1;
  }

  /** Check for dimension. */
  if ( Dimension != 2 && Dimension != 3 )
  {
    std::cerr
      << "ERROR: images of dimension "
      << Dimension
      << " are not supported!"
      << std::endl;
    return 1;
  }

  /** Run the program. */
  try
  {
    if ( Dimension == 2 )
    {
      DistanceTransform<2>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }
    if ( Dimension == 3 )
    {
      DistanceTransform<3>(
        inputFileName,
        outputFileNames,
        outputSquaredDistance,
        method, K );
    }

  }
  catch( itk::ExceptionObject & e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }
  catch( std::exception & e )
  {
    std::cerr << "Caught std::exception: " << e.what() << std::endl;
    return 1;
  }
  catch ( ... )
  {
    std::cerr << "Caught unknown exception" << std::endl;
    return 1;
  }

  /** End program. */
  return 0;

} // end main



/*
 * ******************* DistanceTransform ****************
 *
 */

template <unsigned int NDimensions>
void DistanceTransform(
  const std::string & inputFileName,
  const std::vector<std::string> & outputFileNames,
  bool outputSquaredDistance,
  const std::string & method,
  const unsigned int & K )
{
  const unsigned int              Dimension = NDimensions;
  typedef unsigned char           InputComponentType;
  typedef InputComponentType      InputPixelType;
  typedef float                   OutputComponentType;
  typedef OutputComponentType     OutputPixelType;

  typedef itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::Image< float, Dimension >            FloatImageType;
  typedef itk::Image< unsigned long, Dimension >    ULImageType;

  typedef itk::SignedMaurerDistanceMapImageFilter<
    InputImageType, OutputImageType >               MaurerDistanceType;
  typedef itk::SignedDanielssonDistanceMapImageFilter<
    InputImageType, OutputImageType >               DanielssonDistanceType;
  typedef itk::MorphologicalSignedDistanceTransformImageFilter<
    InputImageType, OutputImageType >               MorphologicalSignedDistanceType;
  typedef itk::MorphologicalDistanceTransformImageFilter<
    InputImageType, OutputImageType >               MorphologicalDistanceType;
//   typedef itk::OrderKDistanceTransformImageFilter<
//     FloatImageType, ULImageType >                   OrderKDistanceType;
//
//   typedef typename OrderKDistanceType::OutputImageType    VoronoiMapType;
//   typedef typename OrderKDistanceType::KDistanceImageType KDistanceImageType;
//   typedef typename OrderKDistanceType::KIDImageType       KIDImageType;

  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename OutputImageType::Pointer         OutputImagePointer;

  typedef itk::ImageFileReader< InputImageType >    ReaderType;
  typedef itk::ImageFileReader< FloatImageType >    FloatReaderType;
  typedef itk::ImageFileWriter< OutputImageType >   WriterType;
//   typedef itk::ImageFileWriter< VoronoiMapType >    VoronoiWriterType;
//   typedef itk::ImageFileWriter< KDistanceImageType > KDistanceWriterType;
//   typedef itk::ImageFileWriter< KIDImageType >      KIDWriterType;

  /** Read the input images */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  typename FloatReaderType::Pointer freader = FloatReaderType::New();
  freader->SetFileName( inputFileName.c_str() );

  /** Setup the Maurer distance transform filter. */
  typename MaurerDistanceType::Pointer distance_Maurer
    = MaurerDistanceType::New();
  distance_Maurer->SetInput( reader->GetOutput() );
  distance_Maurer->SetUseImageSpacing( true );
  distance_Maurer->SetInsideIsPositive( false );
  distance_Maurer->SetSquaredDistance( outputSquaredDistance );
  distance_Maurer->SetBackgroundValue( 0 );

  /** Setup the Danielsson distance transform filter. */
  typename DanielssonDistanceType::Pointer distance_Danielsson
    = DanielssonDistanceType::New();
  distance_Danielsson->SetInput( reader->GetOutput() );
  distance_Danielsson->SetUseImageSpacing( true );
  distance_Danielsson->SetInsideIsPositive( false );
  distance_Danielsson->SetSquaredDistance( outputSquaredDistance );

  /** Setup the Morphological distance transform filter. */
  typename MorphologicalDistanceType::Pointer distance_Morphological
    = MorphologicalDistanceType::New();
  distance_Morphological->SetInput( reader->GetOutput() );
  distance_Morphological->SetUseImageSpacing( true );
  distance_Morphological->SetOutsideValue( 1 );
  distance_Morphological->SetSqrDist( outputSquaredDistance );

  /** Setup the Morphological signed distance transform filter. */
  typename MorphologicalSignedDistanceType::Pointer distance_MorphologicalSigned
    = MorphologicalSignedDistanceType::New();
  distance_MorphologicalSigned->SetInput( reader->GetOutput() );
  distance_MorphologicalSigned->SetUseImageSpacing( true );
  distance_MorphologicalSigned->SetInsideIsPositive( false );
  distance_MorphologicalSigned->SetOutsideValue( 0 );

  /** Setup the OrderK distance transform filter. */
//   typename OrderKDistanceType::Pointer distance_OrderK
//     = OrderKDistanceType::New();
//   distance_OrderK->SetInput( freader->GetOutput() );
//   distance_OrderK->SetUseImageSpacing( true );
//   distance_OrderK->SetInputIsBinary( false );
//   distance_OrderK->SetSquaredDistance( outputSquaredDistance );
//   distance_OrderK->SetK( K );

  /** Setup writer. */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileNames[ 0 ].c_str() );

//   typename VoronoiWriterType::Pointer voronoiWriter = VoronoiWriterType::New();
//   typename KDistanceWriterType::Pointer kDistanceWriter = KDistanceWriterType::New();
//   typename KIDWriterType::Pointer kIDWriter = KIDWriterType::New();
//   voronoiWriter->SetFileName( outputFileNames[ 0 ].c_str() );
//   kDistanceWriter->SetFileName( outputFileNames[ 1 ].c_str() );
//   kIDWriter->SetFileName( outputFileNames[ 2 ].c_str() );

  /** Run! */
  if ( method == "Maurer" )
  {
    distance_Maurer->Update();
    writer->SetInput( distance_Maurer->GetOutput() );
    writer->Update();
  }
  else if ( method == "Danielsson" )
  {
    distance_Danielsson->Update();
    writer->SetInput( distance_Danielsson->GetOutput() );
    writer->Update();
  }
  else if ( method == "Morphological" )
  {
    distance_Morphological->Update();
    writer->SetInput( distance_Morphological->GetOutput() );
    writer->Update();
  }
  else if ( method == "MorphologicalSigned" )
  {
    distance_MorphologicalSigned->Update();
    writer->SetInput( distance_MorphologicalSigned->GetOutput() );
    writer->Update();
  }

//   else if ( method == "OrderK" )
//   {
//     std::cerr << "to here";
//     freader->Update();
//     std::cerr << "to here";
//     distance_OrderK->Update();
//     std::cerr << "to here";
//     voronoiWriter->SetInput( distance_OrderK->GetVoronoiMap() );
//     kDistanceWriter->SetInput( distance_OrderK->GetKDistanceMap() );
//     kIDWriter->SetInput( distance_OrderK->GetKclosestIDMap() );
//
//     voronoiWriter->Update();
//     kDistanceWriter->Update();
//     kIDWriter->Update();
//   }

} // end DistanceTransform()

