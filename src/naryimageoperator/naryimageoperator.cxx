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
 \brief N-ary image operator.
 
 \verbinclude naryimageoperator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsBase.h"
#include "ITKToolsHelpers.h"

#include "NaryImageOperatorMainHelper.h"
#include "NaryImageOperatorHelper.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Performs n-ary operations on multiple (n) images." << std::endl
  << "Usage:\npxnaryimageoperator" << std::endl
  << "  -in      inputFilenames, at least 2" << std::endl
  << "  -out     outputFilename" << std::endl
  << "  -ops     n-ary operator of the following form:" << std::endl
  << "           {+,-,*,/,^,%}" << std::endl
  << "           notation:" << std::endl
  << "             {ADDITION, MINUS, TIMES, DIVIDE," << std::endl
  << "             MEAN," << std::endl
  << "             MAXIMUM, MINIMUM, ABSOLUTEDIFFERENCE," << std::endl
  << "             NARYMAGNITUDE }" << std::endl
  << "           notation examples:" << std::endl
  << "             MINUS = I_0 - I_1 - ... - I_n " << std::endl
  << "             ABSDIFF = |I_0 - I_1 - ... - I_n|" << std::endl
  << "             MIN = min( I_0, ..., I_n )" << std::endl
  << "             MAGNITUDE = sqrt( I_0 * I_0 + ... + I_n * I_n )" << std::endl
//   std::cout << "  [-arg]   argument, necessary for some ops\n"
//             << "             WEIGHTEDADDITION: 0.0 < weight alpha < 1.0\n"
//             << "             MASK[NEG]: background value, e.g. 0." << std::endl;
  << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
  << "  [-s]     number of streams, default equals number of inputs." << std::endl
  << "  [-opct]  output component type, by default the largest of the two input images" << std::endl
  << "             choose one of: {[unsigned_]{char,short,int,long},float,double}" << std::endl
  << "Supported: 2D, 3D, (unsigned) char, (unsigned) short, (unsigned) int, (unsigned) long, float, double." << std::endl;

  return ss.str();
} // end GetHelpString()


/** ITKToolsNaryImageOperator */

class ITKToolsNaryImageOperatorBase : public itktools::ITKToolsBase
{
public:
  ITKToolsNaryImageOperatorBase()
  {
    //std::vector<std::string> m_InputFileNames;
    m_OutputFileName = "";
    m_NaryOperatorName = "";
    m_UseCompression = false;
    m_NumberOfStreams = 0;
    m_Arg = "";
  };
  ~ITKToolsNaryImageOperatorBase(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  std::string m_NaryOperatorName;
  bool m_UseCompression;
  unsigned int m_NumberOfStreams;
  std::string m_Arg;

}; // end ITKToolsNaryImageOperatorBase


template< class TComponentTypeIn, class TComponentTypeOut, unsigned int VDimension >
class ITKToolsNaryImageOperator : public ITKToolsNaryImageOperatorBase
{
public:
  typedef ITKToolsNaryImageOperator Self;

  ITKToolsNaryImageOperator(){};
  ~ITKToolsNaryImageOperator(){};

  static Self * New( itktools::ComponentType componentTypeIn, itktools::ComponentType componentTypeOut, unsigned int dim )
  {
    if ( itktools::IsType<TComponentTypeIn>( componentTypeIn ) &&
         itktools::IsType<TComponentTypeOut>( componentTypeOut ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs. */
    typedef itk::Image<TComponentTypeIn, VDimension>    InputImageType;
    typedef itk::Image<TComponentTypeOut, VDimension>   OutputImageType;
    
    typedef typename InputImageType::PixelType          InputPixelType;
    typedef typename OutputImageType::PixelType         OutputPixelType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >     WriterType;

    /** Read the input images. */
    std::vector<typename ReaderType::Pointer> readers( m_InputFileNames.size() );
    for ( unsigned int i = 0; i < m_InputFileNames.size(); ++i )
    {
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( m_InputFileNames[ i ] );
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
    typename BaseFilterType::Pointer naryFilter = naryFilterFactory.GetFilter(naryOperatorMap[m_NaryOperatorName]);

    //InstantiateNaryFilterNoArg( POWER );
    //InstantiateNaryFilterNoArg( SQUAREDDIFFERENCE );
    //InstantiateNaryFilterNoArg( MODULO );
    //InstantiateNaryFilterNoArg( LOG );

    //InstantiateNaryFilterWithArg( WEIGHTEDADDITION );
    //InstantiateNaryFilterWithArg( MASK );
    //InstantiateNaryFilterWithArg( MASKNEGATED );

    /** Connect the pipeline. */
    for ( unsigned int i = 0; i < m_InputFileNames.size(); ++i )
    {
      naryFilter->SetInput( i, readers[ i ]->GetOutput() );
    }

    /** Write the image to disk */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( naryFilter->GetOutput() );
    writer->SetUseCompression( m_UseCompression );
    writer->SetNumberOfStreamDivisions( m_NumberOfStreams );
    writer->Update();
  }

}; // end ITKToolsNaryImageOperator
//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-ops", "Operation." );

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
  std::vector<std::string> inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string ops = "";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string argument = "0";
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  std::string opct = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", opct );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Support for streaming. */
  unsigned int numberOfStreams = inputFileNames.size();
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** You should specify at least two input files. */
  if ( inputFileNames.size() < 2 )
  {
    std::cerr << "ERROR: You should specify at least two input file names." << std::endl;
    return 1;
  }

  /** Determine image properties. */
  itktools::ComponentType componentTypeIn = itk::ImageIOBase::LONG;
  itktools::ComponentType componentTypeOut = itk::ImageIOBase::LONG;
  unsigned int inputDimension = 2;
  int retdip = DetermineImageProperties( inputFileNames,
    componentTypeIn, componentTypeOut, inputDimension );
  if ( retdip ) return 1;

  /** Let the user override the output component type. */
  if ( retopct )
  {
    componentTypeOut = itk::ImageIOBase::GetComponentTypeFromString( opct );
    if ( !itktools::ComponentTypeIsValid( componentTypeOut ) )
    {
      std::cerr << "ERROR: the you specified an invalid opct." << std::endl;
      return 1;
    }
    
    if ( !itktools::ComponentTypeIsInteger( componentTypeOut ) )
    {
      componentTypeIn = itk::ImageIOBase::DOUBLE;
    }
  }

  /** Check if a valid operator is given. */
  std::string opsCopy = ops;
  int retCO  = CheckOperator( ops );
  if ( retCO ) return retCO;

  /** For certain ops an argument is mandatory. */
  bool retCOA = CheckOperatorAndArgument( ops, argument, retarg );
  if ( !retCOA ) return 1;


  /** Class that does the work */
  ITKToolsNaryImageOperatorBase * naryImageOperator = NULL;

  unsigned int dim = 0;
  itktools::GetImageDimension(inputFileNames[0], dim);

  /** \todo some progs allow user to override the pixel type,
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileNames[0]);

  std::cout << "Detected component type: " <<
    componentType << std::endl;

  try
  {
    // now call all possible template combinations.
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, char, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned char, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, short, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned short, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, int, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned int, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, long, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned long, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< double, float, 2 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< double, double, 2 >::New( componentTypeIn, componentTypeOut, dim );

#ifdef ITKTOOLS_3D_SUPPORT
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, char, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned char, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, short, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned short, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, int, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned int, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, long, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< long, unsigned long, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< double, float, 3 >::New( componentTypeIn, componentTypeOut, dim );
    if (!naryImageOperator) naryImageOperator = ITKToolsNaryImageOperator< double, double, 3 >::New( componentTypeIn, componentTypeOut, dim );
#endif
    if (!naryImageOperator)
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentTypeIn // so here we also need a string - we don't need to convert to a string here right? just output the string that was input.
        << " ; dimension = " << dim
        << std::endl;
      return 1;
    }

    naryImageOperator->m_InputFileNames = inputFileNames;
    naryImageOperator->m_OutputFileName = outputFileName;
    naryImageOperator->m_NaryOperatorName = ops;
    naryImageOperator->m_UseCompression = useCompression;
    naryImageOperator->m_NumberOfStreams = numberOfStreams;
    naryImageOperator->m_Arg = argument;
  
    naryImageOperator->Run();

    delete naryImageOperator;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete naryImageOperator;
    return 1;
  }
  

  /** End program. */
  return 0;

} // end main
