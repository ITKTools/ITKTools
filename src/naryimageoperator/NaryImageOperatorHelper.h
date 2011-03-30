#ifndef __NaryImageOperatorHelper_h
#define __NaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkNaryFunctors.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <vector>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------

/** Macros for easily instantiating the correct binary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::NaryFunctorImageFilter<
 *   InputImageType,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixelType,OutputPixelType> > PLUSFilterType;
 * naryFilter = (PLUSFilterType::New()).GetPointer();
 * 
 */
#define InstantiateNaryFilterNoArg( name ) \
  typedef itk::NaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::Nary##name<InputPixelType, OutputPixelType> > name##FilterType; \
  if ( naryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempNaryFilter = name##FilterType::New(); \
    tempNaryFilter->InPlaceOn(); \
    naryFilter = tempNaryFilter.GetPointer(); \
  }

#define InstantiateNaryFilterWithArg( name ) \
  typedef itk::NaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::Nary##name<InputPixelType, OutputPixelType> > name##FilterType; \
  if ( naryOperatorName == #name ) \
  {\
    typename name##FilterType::Pointer tempNaryFilter = name##FilterType::New(); \
    tempNaryFilter->InPlaceOn(); \
    tempNaryFilter->GetFunctor().SetArgument( argument ); \
    naryFilter = tempNaryFilter.GetPointer(); \
  }


/**
 * ******************* NaryImageOperator *******************
 */

template< class InputImageType, class OutputImageType >
void NaryImageOperator(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::string & ops,
  const bool & useCompression,
  const unsigned int & numberOfStreams,
  const std::string & arg )
{
  /** Typedefs. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef itk::ImageToImageFilter<InputImageType, OutputImageType> BaseFilterType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Read the input images. */
  std::vector<typename ReaderType::Pointer> readers( inputFileNames.size() );
  for ( unsigned int i = 0; i < inputFileNames.size(); ++i )
  {
    readers[ i ] = ReaderType::New();
    readers[ i ]->SetFileName( inputFileNames[ i ] );
  }

  /** Get the argument. */
  double argument = atof( arg.c_str() );

  /** Get the naryOperatorName. */
  std::string naryOperatorName = ops;

  /** Set up the binaryFilter. */
  typename BaseFilterType::Pointer naryFilter = 0;
  InstantiateNaryFilterNoArg( ADDITION );
  InstantiateNaryFilterNoArg( MEAN );
  InstantiateNaryFilterNoArg( MINUS );
  InstantiateNaryFilterNoArg( TIMES );
  InstantiateNaryFilterNoArg( DIVIDE );
  //InstantiateNaryFilterNoArg( POWER );
  InstantiateNaryFilterNoArg( MAXIMUM );
  InstantiateNaryFilterNoArg( MINIMUM );
  InstantiateNaryFilterNoArg( ABSOLUTEDIFFERENCE );
  //InstantiateNaryFilterNoArg( SQUAREDDIFFERENCE );
  InstantiateNaryFilterNoArg( NARYMAGNITUDE );
  //InstantiateNaryFilterNoArg( MODULO );
  //InstantiateNaryFilterNoArg( LOG );

  //InstantiateNaryFilterWithArg( WEIGHTEDADDITION );
  //InstantiateNaryFilterWithArg( MASK );
  //InstantiateNaryFilterWithArg( MASKNEGATED );

  /** Connect the pipeline. */
  for ( unsigned int i = 0; i < inputFileNames.size(); ++i )
  {
    naryFilter->SetInput( i, readers[ i ]->GetOutput() );
  }

  /** Write the image to disk */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( naryFilter->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->SetNumberOfStreamDivisions( numberOfStreams );
  writer->Update();

} // end NaryImageOperator()


#endif //#ifndef __NaryImageOperatorHelper_h
