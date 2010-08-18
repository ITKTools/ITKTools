#ifndef __BinaryImageOperatorHelper_h
#define __BinaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkBinaryFunctors.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <vector>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------

/** Macros for easily instantiating the correct binary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::BinaryFunctorImageFilter<
 *   InputImage1Type,
 *   InputImage2Type,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixel1Type,InputPixel2Type,OutputPixelType> > PLUSFilterType;
 * binaryFilter = (PLUSFilterType::New()).GetPointer();
 * 
 */
#define InstantiateBinaryFilterNoArg( name ) \
  typedef itk::BinaryFunctorImageFilter< \
    InputImage1Type, InputImage2Type, OutputImageType, \
    itk::Functor::name<InputPixel1Type, InputPixel2Type, OutputPixelType> > name##FilterType; \
  if ( binaryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempBinaryFilter = name##FilterType::New(); \
    binaryFilter = tempBinaryFilter.GetPointer(); \
  }

#define InstantiateBinaryFilterWithArg( name ) \
  typedef itk::BinaryFunctorImageFilter< \
    InputImage1Type, InputImage2Type, OutputImageType, \
    itk::Functor::name<InputPixel1Type, InputPixel2Type, OutputPixelType> > name##FilterType; \
  if ( binaryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempBinaryFilter = name##FilterType::New(); \
    tempBinaryFilter->GetFunctor().SetArgument( argument ); \
    binaryFilter = tempBinaryFilter.GetPointer(); \
  }
 
  
  /**
   * ******************* BinaryImageOperator *******************
   */

template< class InputImage1Type, class InputImage2Type, class OutputImageType >
void BinaryImageOperator( 
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::string & ops,
  const bool & useCompression,
  const std::string & arg )
{
  /** Typedefs. */
  typedef typename InputImage1Type::PixelType         InputPixel1Type;
  typedef typename InputImage2Type::PixelType         InputPixel2Type;
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef itk::ImageToImageFilter<InputImage1Type, OutputImageType> BaseFilterType;
  typedef itk::ImageFileReader< InputImage1Type >     Reader1Type;
  typedef itk::ImageFileReader< InputImage2Type >     Reader2Type;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Read the input images. */
  typename Reader1Type::Pointer reader1 = Reader1Type::New();
  reader1->SetFileName( inputFileName1.c_str() );
  typename Reader2Type::Pointer reader2 = Reader2Type::New();
  reader2->SetFileName( inputFileName2.c_str() );

  /** Get the argument. */
  double argument = atof( arg.c_str() );

  /** Get the binaryOperatorName. */
  std::string binaryOperatorName = ops;

  /** Set up the binaryFilter. */
  typename BaseFilterType::Pointer binaryFilter = 0;
  InstantiateBinaryFilterNoArg( ADDITION );
  InstantiateBinaryFilterNoArg( MINUS );
  InstantiateBinaryFilterNoArg( TIMES );
  InstantiateBinaryFilterNoArg( DIVIDE );
  InstantiateBinaryFilterNoArg( POWER );
  InstantiateBinaryFilterNoArg( MAXIMUM );
  InstantiateBinaryFilterNoArg( MINIMUM );
  InstantiateBinaryFilterNoArg( ABSOLUTEDIFFERENCE );
  InstantiateBinaryFilterNoArg( SQUAREDDIFFERENCE );
  InstantiateBinaryFilterNoArg( BINARYMAGNITUDE );
  //InstantiateBinaryFilterNoArg( MODULO );
  InstantiateBinaryFilterNoArg( LOG );

  InstantiateBinaryFilterWithArg( WEIGHTEDADDITION );
  InstantiateBinaryFilterWithArg( MASK );
  InstantiateBinaryFilterWithArg( MASKNEGATED );

  /** Connect the pipeline. */
  binaryFilter->SetInput( 0, reader1->GetOutput() );
  binaryFilter->SetInput( 1, reader2->GetOutput() );

  /** Write the image to disk */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( binaryFilter->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end BinaryImageOperator


#endif //#ifndef __BinaryImageOperatorHelper_h
