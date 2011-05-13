#ifndef __NaryImageOperatorHelper_h
#define __NaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkNaryFunctors.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "NaryFilterFactory.h"

#include <vector>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------

/**
 * ******************* NaryImageOperator *******************
 */

template< class InputImageType, class OutputImageType >
void NaryImageOperator(
  const std::vector<std::string> & inputFileNames,
  const std::string & outputFileName,
  const std::string & naryOperatorName,
  const bool & useCompression,
  const unsigned int & numberOfStreams,
  const std::string & arg )
{
  /** Typedefs. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Read the input images. */
  std::vector<typename ReaderType::Pointer> readers( inputFileNames.size() );
  for ( unsigned int i = 0; i < inputFileNames.size(); ++i )
  {
    readers[ i ] = ReaderType::New();
    readers[ i ]->SetFileName( inputFileNames[ i ] );
  }

  std::map <std::string, NaryFilterEnum> naryOperatorMap;
 
  naryOperatorMap["ADDITION"] = ADDITION;
  naryOperatorMap["MEAN"] = MEAN;
  naryOperatorMap["MINUS"] = MINUS;
  naryOperatorMap["TIMES"] = TIMES;
  naryOperatorMap["DIVIDE"] = DIVIDE;
  naryOperatorMap["MAXIMUM"] = MAXIMUM;
  naryOperatorMap["MINIMUM"] = MINIMUM;
  naryOperatorMap["ABSOLUTEDIFFERENCE"] = ABSOLUTEDIFFERENCE;
  naryOperatorMap["NARYMAGNITUDE"] = NARYMAGNITUDE;
  
  /** Set up the binaryFilter. */
  NaryFilterFactory<InputImageType, OutputImageType> naryFilterFactory;
  typedef itk::InPlaceImageFilter<InputImageType, OutputImageType> BaseFilterType;
  typename BaseFilterType::Pointer naryFilter = naryFilterFactory.GetFilter(naryOperatorMap[naryOperatorName]);
  
  //InstantiateNaryFilterNoArg( POWER );
  //InstantiateNaryFilterNoArg( SQUAREDDIFFERENCE );
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
