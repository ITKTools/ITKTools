#ifndef __UnaryImageOperatorHelper_h
#define __UnaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkUnaryFunctorImageFilter.h"
//#include <map>
#include <vector>
#include <itksys/SystemTools.hxx>

#include "itkUnaryFunctors.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run(function,typeIn,typeOut,dim) \
if ( ComponentTypeIn == #typeIn && ComponentTypeOut == #typeOut && inputDimension == dim ) \
{ \
  typedef itk::Image< typeIn, dim > InputImageType; \
  typedef itk::Image< typeOut, dim > OutputImageType; \
  function< InputImageType, OutputImageType >( inputFileName, outputFileName, ops, argument ); \
}

//-------------------------------------------------------------------------------------

/** Macro for easily instantiating the correct unary functor
 * if, for example name is PLUS, the result is:
 * typedef itk::UnaryFunctorImageFilter<
 *   InputImageType,
 *   OutputImageType,
 *   itk::Functor::PLUS<InputPixelType,ArgumentType,OutputPixelType > PLUSFilterType;
 * unaryFilter = (PLUSFilterType::New()).GetPointer();
 * 
 */
#define InstantiateUnaryFilter(name,typeArg) \
  typedef itk::UnaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::name<InputPixelType, typeArg, OutputPixelType> >  name##typeArg##FilterType; \
  if ( unaryOperatorName == #name && argumentType == #typeArg ) \
  {\
    typename name##typeArg##FilterType::Pointer tempUnaryFilter = name##typeArg##FilterType::New(); \
    tempUnaryFilter->GetFunctor().SetArgument( static_cast<typeArg>( argument ) ); \
    unaryFilter = tempUnaryFilter.GetPointer(); \
  }
  
#define InstantiateUnaryFilter2(name) \
  typedef itk::UnaryFunctorImageFilter< \
    InputImageType, OutputImageType, \
    itk::Functor::name<InputPixelType, double, OutputPixelType> > name##typeArg##FilterType; \
  if ( unaryOperatorName == #name ) \
  {\
    typename name##typeArg##FilterType::Pointer tempUnaryFilter = name##typeArg##FilterType::New(); \
    tempUnaryFilter->GetFunctor().SetArgument( static_cast<double>( argument ) ); \
    unaryFilter = tempUnaryFilter.GetPointer(); \
  }
 
  
  /**
   * ******************* UnaryImageOperator *******************
   */

template< class InputImageType, class OutputImageType >
void UnaryImageOperator( 
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::string & ops,
  const std::string & arg )
{
  /** Typedefs. */
  typedef typename InputImageType::PixelType          InputPixelType;
  typedef typename OutputImageType::PixelType         OutputPixelType;
  typedef itk::ImageToImageFilter<InputImageType, OutputImageType> BaseFilterType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >     WriterType;

  /** Read the image. */
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );

  /** Get the ArgumentType and argument. */
  std::string argumentType = "int";
  std::basic_string<char>::size_type pos = arg.find( "." );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos )
  {
    argumentType = "double";
  }
  float argument = atof( arg.c_str() );

  /** Get the unaryOperatorName. */
  std::string unaryOperatorName = ops;

  /** Set up the unaryFilter */
  typename BaseFilterType::Pointer unaryFilter = 0;
  InstantiateUnaryFilter( PLUS, int );
  InstantiateUnaryFilter( PLUS, double );
  InstantiateUnaryFilter( RMINUS, int );
  InstantiateUnaryFilter( RMINUS, double );
  InstantiateUnaryFilter( LMINUS, int );
  InstantiateUnaryFilter( LMINUS, double );
  InstantiateUnaryFilter( TIMES, int );
  InstantiateUnaryFilter( TIMES, double );
  InstantiateUnaryFilter( LDIVIDE, int );
  InstantiateUnaryFilter( LDIVIDE, double );
  InstantiateUnaryFilter( RDIVIDE, int );
  InstantiateUnaryFilter( RDIVIDE, double );
  InstantiateUnaryFilter( RPOWER, double );
  InstantiateUnaryFilter( LPOWER, double );
  InstantiateUnaryFilter( RMODINT, int );
  InstantiateUnaryFilter( RMODDOUBLE, double );
  InstantiateUnaryFilter( LMODINT, int );
  InstantiateUnaryFilter( LMODDOUBLE, double );
  InstantiateUnaryFilter( NLOG, double );

  InstantiateUnaryFilter( EQUAL, int );
  // arg is dummy
  InstantiateUnaryFilter2( NEG );
  InstantiateUnaryFilter2( SIGNINT );
  InstantiateUnaryFilter2( SIGNDOUBLE );
  InstantiateUnaryFilter2( ABSINT );
  InstantiateUnaryFilter2( ABSDOUBLE );
  InstantiateUnaryFilter2( FLOOR );
  InstantiateUnaryFilter2( CEIL );
  InstantiateUnaryFilter2( ROUND );
  InstantiateUnaryFilter2( LN );
  InstantiateUnaryFilter2( LOG10 );
  InstantiateUnaryFilter2( EXP );
  InstantiateUnaryFilter2( SIN );
  InstantiateUnaryFilter2( COS );
  InstantiateUnaryFilter2( TAN );
  InstantiateUnaryFilter2( ARCSIN );
  InstantiateUnaryFilter2( ARCCOS );
  InstantiateUnaryFilter2( ARCTAN );

  /** Connect the pipeline. */
  unaryFilter->SetInput( reader->GetOutput() );

  /** Write the image to disk */
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( unaryFilter->GetOutput() );
  writer->Update();

} // end UnaryImageOperator


#endif //#ifndef __UnaryImageOperatorHelper_h
