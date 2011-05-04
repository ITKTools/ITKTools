#ifndef __computeDifferenceImageHelpers_h__
#define __computeDifferenceImageHelpers_h__

/** Basic Image support. */
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageIORegion.h"

/** For the support of RGB voxels. */
//#include "itkRGBPixel.h"

/** Reading and writing images. */
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

// One of these is used to cast the image. *
#include "itkShiftScaleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

// Print image information from the reader and the writer. *
template< class ReaderType, class WriterType >
void PrintInfo( ReaderType reader, WriterType writer )
{
  //* Typedef's. *
  typedef itk::ImageIOBase                        ImageIOBaseType;
  typedef itk::ImageIORegion                      ImageIORegionType;
  typedef typename ImageIORegionType::SizeType    SizeType;

  //* Get IOBase of the reader and extract information. *
  ImageIOBaseType::Pointer imageIOBaseIn = reader->GetImageIO();
  ImageIORegionType iORegionIn = imageIOBaseIn->GetIORegion();

  const char * fileNameIn = imageIOBaseIn->GetFileName();
  std::string pixelTypeIn = imageIOBaseIn->GetPixelTypeAsString( imageIOBaseIn->GetPixelType() );
  unsigned int nocIn = imageIOBaseIn->GetNumberOfComponents();
  std::string componentTypeIn = imageIOBaseIn->GetComponentTypeAsString( imageIOBaseIn->GetComponentType() );
  unsigned int dimensionIn = imageIOBaseIn->GetNumberOfDimensions();
  SizeType sizeIn = iORegionIn.GetSize();

  //*  Get  IOBase of  the  writer and extract information.  *
  ImageIOBaseType::Pointer imageIOBaseOut = writer->GetImageIO();
  ImageIORegionType iORegionOut = imageIOBaseOut->GetIORegion();

  const char * fileNameOut = imageIOBaseOut->GetFileName();
  std::string pixelTypeOut = imageIOBaseOut->GetPixelTypeAsString( imageIOBaseOut->GetPixelType() );
  unsigned int nocOut = imageIOBaseOut->GetNumberOfComponents();
  std::string componentTypeOut = imageIOBaseOut->GetComponentTypeAsString( imageIOBaseOut->GetComponentType() );
  unsigned int dimensionOut = imageIOBaseOut->GetNumberOfDimensions();
  SizeType sizeOut = iORegionOut.GetSize();

  //* Print information. *
  std::cout << "Information about the input image \"" << fileNameIn << "\":" << std::endl;
  std::cout << "\tdimension:\t\t" << dimensionIn << std::endl;
  std::cout << "\tpixel type:\t\t" << pixelTypeIn << std::endl;
  std::cout << "\tnumber of components:\t" << nocIn << std::endl;
  std::cout << "\tcomponent type:\t\t" << componentTypeIn << std::endl;
  std::cout << "\tsize:\t\t\t";
  for ( unsigned int i = 0; i < dimensionIn; i++ ) std::cout << sizeIn[ i ] << " ";
  std::cout << std::endl;

  //* Print information. *
  std::cout << std::endl;
  std::cout << "Information about the output image \"" << fileNameOut << "\":" << std::endl;
  std::cout << "\tdimension:\t\t" << dimensionOut << std::endl;
  std::cout << "\tpixel type:\t\t" << pixelTypeOut << std::endl;
  std::cout << "\tnumber of components:\t" << nocOut << std::endl;
  std::cout << "\tcomponent type:\t\t" << componentTypeOut << std::endl;
  std::cout << "\tsize:\t\t\t";
  for ( unsigned int i = 0; i < dimensionOut; i++ ) std::cout << sizeOut[ i ] << " ";
  std::cout << std::endl;

}  // end PrintInfo


/* The function that reads the input images, calculates the difference
 * and writes the output image.
 * This function is templated over the image types. In the main function
 * we have to make sure to call the right instantiation.
 */
// template< class InputImageType1, class InputImageType2, class OutputImageType >
// void ComputeScalarDifferenceImage( std::string image1FileName,
//   std::string image2FileName, std::string outputFileName )
// {
//   /**  Typedef the correct reader, iterators and writer. */
//   typedef typename itk::ImageFileReader< InputImageType1 >    Image1ReaderType;
//   typedef typename itk::ImageFileReader< InputImageType2 >    Image2ReaderType;
//   typedef itk::ImageRegionConstIterator< InputImageType1 >    ConstIteratorType1;
//   typedef itk::ImageRegionConstIterator< InputImageType2 >    ConstIteratorType2;
//   typedef itk::ImageRegionIterator< OutputImageType >         OutputIteratorType;
//   typedef typename itk::ImageFileWriter< OutputImageType >    ImageWriterType;
//   typedef typename OutputImageType::PixelType                 OutputPixelType;
//
//   /** Create and setup the readers. */
//   typename Image1ReaderType::Pointer reader1 = Image1ReaderType::New();
//   typename Image2ReaderType::Pointer reader2 = Image2ReaderType::New();
//   reader1->SetFileName( image1FileName.c_str() );
//   reader2->SetFileName( image2FileName.c_str() );
//
//   /** Read images. */
//   reader1->Update();
//   reader2->Update();
//
//   /** The sizes of the image1 and image2 must match. */
//   typename InputImageType1::SizeType size1 = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();
//   typename InputImageType2::SizeType size2 = reader2->GetOutput()->GetLargestPossibleRegion().GetSize();
//
//   if ( size1 != size2 )
//   {
//     std::cerr << "The size of the two images are "
//       << size1 << " and "
//       << size2 << "." << std::endl;
//     std::cerr << "They should match!" << std::endl;
//     /** Create and throw an exception. */
//     //itkException( "ERROR: sizes do not match!" );
//   }
//
//   /** Create an output image. */
//   typename OutputImageType::Pointer outputImage = OutputImageType::New();
//   outputImage->SetRegions( reader1->GetOutput()->GetLargestPossibleRegion() );
//   outputImage->SetSpacing( reader1->GetOutput()->GetSpacing() );
//   outputImage->SetOrigin( reader1->GetOutput()->GetOrigin() );
//   outputImage->Allocate();
//
//   /** Create iterators over the images. */
//   ConstIteratorType1 it1( reader1->GetOutput(),
//     reader1->GetOutput()->GetLargestPossibleRegion() );
//   ConstIteratorType2 it2( reader2->GetOutput(),
//     reader2->GetOutput()->GetLargestPossibleRegion() );
//   OutputIteratorType itout( outputImage, outputImage->GetLargestPossibleRegion() );
//   it1.GoToBegin();
//   it2.GoToBegin();
//   itout.GoToBegin();
//
//   /** Get the extrema of the pixel type. */
//   OutputPixelType minimum = itk::NumericTraits<OutputPixelType>::NonpositiveMin();
//   OutputPixelType maximum = itk::NumericTraits<OutputPixelType>::max();
//   double min = static_cast<double>( minimum );
//   double max = static_cast<double>( maximum );
//
//   /** Do the actual work. */
//   double diff = 0.0;
//   while ( !it1.IsAtEnd() )
//   {
//     diff = static_cast<double>( it1.Get() ) - static_cast<double>( it2.Get() );
//     if ( diff < min )
//     {
//       itout.Set( minimum );
//     }
//     else if ( diff > max )
//     {
//       itout.Set( maximum );
//     }
//     else
//     {
//       itout.Set( static_cast<OutputPixelType>( diff ) );
//     }
//     /** Increase iterators. */
//     ++it1;
//     ++it2;
//     ++itout;
//   } // end while
//
//   /** Connect the pipeline. */
//   typename ImageWriterType::Pointer writer = ImageWriterType::New();
//   writer->SetFileName( outputFileName.c_str() );
//   writer->SetInput( outputImage );
//
//   /** Do the actual conversion. */
//   writer->Update();
//
//   /** Print information. */
//   //PrintInfo( reader, writer );
//
// }  // end CreateScalarDifferenceImage


/* The function that reads the input images, calculates the difference
 * and writes the output image.
 * This function is templated over the image types. In the main function
 * we have to make sure to call the right instantiation.
 */
template< class InputImageType1, class InputImageType2, class OutputImageType >
void ComputeVectorDifferenceImage( std::string image1FileName,
  std::string image2FileName, std::string outputFileName )
{
  /**  Typedef the correct reader, iterators and writer. */
  typedef typename itk::ImageFileReader< InputImageType1 >    Image1ReaderType;
  typedef typename itk::ImageFileReader< InputImageType2 >    Image2ReaderType;
  typedef itk::ImageRegionConstIterator< InputImageType1 >    ConstIteratorType1;
  typedef itk::ImageRegionConstIterator< InputImageType2 >    ConstIteratorType2;
  typedef itk::ImageRegionIterator< OutputImageType >         OutputIteratorType;
  typedef typename itk::ImageFileWriter< OutputImageType >    ImageWriterType;

  /** Get the pixeltype (== vector), the valuetype (==short, float, etc.)
   * and the vector dimension.
   */
  typedef typename OutputImageType::PixelType       OutputPixelType;
  typedef typename OutputPixelType::ValueType       OutputValueType;
  const unsigned int vecDim = OutputPixelType::Dimension;
  typedef itk::Vector< double, vecDim >             DoubleVectorType;

  /** Create and setup the readers. */
  typename Image1ReaderType::Pointer reader1 = Image1ReaderType::New();
  typename Image2ReaderType::Pointer reader2 = Image2ReaderType::New();
  reader1->SetFileName( image1FileName.c_str() );
  reader2->SetFileName( image2FileName.c_str() );

  /** Read images. */
  reader1->Update();
  reader2->Update();

  /** The sizes of the image1 and image2 must match. */
  typename InputImageType1::SizeType size1 = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();
  typename InputImageType2::SizeType size2 = reader2->GetOutput()->GetLargestPossibleRegion().GetSize();

  if ( size1 != size2 )
  {
    std::cerr << "The size of the two images are "
      << size1 << " and "
      << size2 << "." << std::endl;
    std::cerr << "They should match!" << std::endl;
    /** Create and throw an exception. */
    //itkException( "ERROR: sizes do not match!" );
  }

  /** Create an output image. */
  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->SetRegions( reader1->GetOutput()->GetLargestPossibleRegion() );
  outputImage->SetSpacing( reader1->GetOutput()->GetSpacing() );
  outputImage->SetOrigin( reader1->GetOutput()->GetOrigin() );
  outputImage->Allocate();

  /** Create iterators over the images. */
  ConstIteratorType1 it1( reader1->GetOutput(),
    reader1->GetOutput()->GetLargestPossibleRegion() );
  ConstIteratorType2 it2( reader2->GetOutput(),
    reader2->GetOutput()->GetLargestPossibleRegion() );
  OutputIteratorType itout( outputImage, outputImage->GetLargestPossibleRegion() );
  it1.GoToBegin();
  it2.GoToBegin();
  itout.GoToBegin();

  /** Get the extrema of the pixel type. */
  OutputValueType minimum = itk::NumericTraits<OutputValueType>::NonpositiveMin();
  OutputValueType maximum = itk::NumericTraits<OutputValueType>::max();
  double min = static_cast<double>( minimum );
  double max = static_cast<double>( maximum );

  /** Do the actual work. */
  DoubleVectorType diff;
  OutputPixelType out;
  while ( !it1.IsAtEnd() )
  {
    diff = static_cast<DoubleVectorType>( it1.Get() ) - static_cast<DoubleVectorType>( it2.Get() );
    for ( unsigned int i = 0; i < vecDim; i++ )
    {
      if ( diff[ i ] < min )
      {
        out[ i ] = minimum;
      }
      else if ( diff[ i ] > max )
      {
        out[ i ] = maximum;
      }
      else
      {
        out[ i ] = static_cast<OutputValueType>( diff[ i ] );
      }
    } // end for loop
    /** Set output. */
    itout.Set( out );
    /** Increase iterators. */
    ++it1;
    ++it2;
    ++itout;
  } // end while

  /** Connect the pipeline. */
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( outputImage );

  /** Do the actual conversion. */
  writer->Update();

  /** Print information. */
  //PrintInfo( reader, writer );

}  // end CreateVectorDifferenceImage

/** Macros are used in order to make the code in main() look cleaner. */

/** callCorrectScalarDifferenceMacro:
 * A macro to call the conversion function.
 */
/*
#define callCorrectScalarDifferenceMacro(typeIn1,typeIn2,typeOut,dim) \
    if ( inputPixelComponentType1 == #typeIn1 && inputPixelComponentType2 == #typeIn2 && outputPixelComponentType == #typeOut && inputDimension == dim) \
{ \
    typedef itk::Image< typeIn1, dim >  InputImageType1; \
    typedef itk::Image< typeIn2, dim >  InputImageType2; \
    typedef itk::Image< typeOut, dim >  OutputImageType; \
    ComputeScalarDifferenceImage< InputImageType1, InputImageType2, OutputImageType >( image1FileName, image2FileName, outputFileName ); \
}
*/

/** callCorrectVectorDifferenceMacro:
 * A macro to call the conversion function.
 */

#define callCorrectVectorDifferenceMacro(typeIn1,typeIn2,typeOut,dim,vecDim) \
    if ( inputPixelComponentType1 == #typeIn1 && inputPixelComponentType2 == #typeIn2 \
    && outputPixelComponentType == #typeOut && inputDimension == dim && vectorDimension == vecDim ) \
{ \
    typedef itk::Vector< typeIn1, vecDim >  VectorType1; \
    typedef itk::Vector< typeIn2, vecDim >  VectorType2; \
    typedef itk::Vector< typeOut, vecDim >  OutputVectorType; \
    typedef itk::Image< VectorType1, dim >  InputImageType1; \
    typedef itk::Image< VectorType1, dim >  InputImageType2; \
    typedef itk::Image< OutputVectorType, dim >  OutputImageType; \
    ComputeVectorDifferenceImage< InputImageType1, InputImageType2, OutputImageType >( image1FileName, image2FileName, outputFileName ); \
}

#endif // endif __computeDifferenceImageHelpers_h__
