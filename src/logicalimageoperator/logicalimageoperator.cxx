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
 \brief Perform a logical operation on an image.
 
 \verbinclude logicalimageoperator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "LogicalImageOperatorHelper.h"

/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Logical operations on one or two images." << std::endl
     << "NOTE: The output of this filter is an image with pixels of values 0 and 1." << std::endl
  << "An appropriate scaling must be performed either manually (with pxrescaleintensityimagefilter)" << std::endl
  << "or with the application used to view the image." << std::endl << std::endl
  << "In the case of a vector image, this is a componentwise logical operator." << std::endl
  << "Usage:" << std::endl << "pxlogicalimageoperator" << std::endl
  << "  -in      inputFilename1 [inputFilename2]" << std::endl
  << "  [-out]   outputFilename, default in1 + <ops> + in2 + .mhd" << std::endl
  << "  -ops     LogicalOperator of the following form:" << std::endl
  << "             [!]( ([!] A) [{&,|,^} ([!] B])] )" << std::endl
  << "           notation:" << std::endl
  << "             [NOT_][NOT][{AND,OR,XOR}[NOT]]" << std::endl
  << "           notation examples:" << std::endl
  << "             ANDNOT = A & (!B)" << std::endl
  << "             NOTAND = (!A) & B" << std::endl
  << "             NOTANDNOT = (!A) & (!B)" << std::endl
  << "             NOT_NOTANDNOT = !( (!A) & (!B) )" << std::endl
  << "             NOT_AND = !(A & B)" << std::endl
  << "             OR = A | B" << std::endl
  << "             XOR = A ^ B" << std::endl
  << "             NOT = !A " << std::endl
  << "             NOT_NOT = A" << std::endl
  << "           Internally this expression is simplified." << std::endl
  << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
  << "  [-arg]   argument, necessary for some ops" << std::endl
  << "  [-dim]   dimension, default: automatically determined from inputimage1" << std::endl
  << "  [-pt]    pixelType, default: automatically determined from inputimage1" << std::endl
  << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;

  return ss.str();
} // end GetHelpString()

/** LogicalImageOperator */

class LogicalImageOperatorBase : public itktools::ITKToolsBase
{ 
public:
  LogicalImageOperatorBase(){};
  ~LogicalImageOperatorBase(){};

  /** Input parameters */
  std::string m_InputFileName1;
  std::string m_InputFileName2;
  std::string m_OutputFileName;
  std::string m_Ops;
  bool m_UseCompression;
  double m_Argument;
  bool m_Unary; // is the operator to be performed unary? (else it is binary)
    
}; // end LogicalImageOperatorBase


template< unsigned int VImageDimension, class TComponentType >
class LogicalImageOperator : public LogicalImageOperatorBase
{
public:
  typedef LogicalImageOperator Self;

  LogicalImageOperator(){};
  ~LogicalImageOperator(){};

  static Self * New( unsigned int imageDimension, itktools::ComponentType componentType )
  {
    if ( VImageDimension == imageDimension && itktools::IsType<TComponentType>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    if(m_Unary)
    {
      RunUnary();
    }
    else
    {
      RunBinary();
    }
  }
  
  void RunUnary(void)
  {
    /** Typedefs. */
    typedef itk::VectorImage<TComponentType, VImageDimension>         VectorImageType;

    typedef itk::Image<TComponentType, VImageDimension> ScalarImageType;

    typedef itk::ImageFileReader< VectorImageType >      ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >      WriterType;

    /** Declarations. */
    typename ReaderType::Pointer reader1 = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Read the images. */
    reader1->SetFileName( m_InputFileName1.c_str() );
    std::cout << "Reading image1: " << m_InputFileName1 << std::endl;
    reader1->Update();
    std::cout << "Done reading image1." << std::endl;

    UnaryFunctorEnum unaryOperation;
    if(m_Ops.compare("EQUAL"))
    {
      unaryOperation = EQUAL;
    }
    else if(m_Ops.compare("NOT"))
    {
      unaryOperation = NOT;
    }
    else
    {
      std::cerr << "Invalid operator: " << m_Ops << std::endl;
      return;
    }
    
    UnaryLogicalFunctorFactory<ScalarImageType> unaryFactory;
    typename itk::InPlaceImageFilter<ScalarImageType, ScalarImageType>::Pointer logicalFilter =
	unaryFactory.GetFilter(unaryOperation, static_cast<TComponentType>(m_Argument) );

    // Create the filter which will assemble the component into the output image
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    
    std::cout
      << "Performing logical operation, "
      << m_Ops
      << ", on input image(s)..."
      << std::endl;
      
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;
    
    for(unsigned int component = 0; component < reader1->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
    {
      typename ComponentExtractionType::Pointer componentExtractor1 = ComponentExtractionType::New();
      componentExtractor1->SetIndex(component);
      componentExtractor1->SetInput(reader1->GetOutput());
      componentExtractor1->Update();
      logicalFilter->SetInput(componentExtractor1->GetOutput() );

      logicalFilter->Update();
      
      imageToVectorImageFilter->SetNthInput(component, logicalFilter->GetOutput());
    }//end component loop

    std::cout << "Done performing logical operation." << std::endl;

    /** Write the image to disk */
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetUseCompression( m_UseCompression );
    std::cout << "Writing output to disk as: " << m_OutputFileName << std::endl;
    writer->Update();
    std::cout << "Done writing output to disk." << std::endl;
  }
  
  
  void RunBinary(void)
  {
    /** Typedefs. */
    typedef itk::VectorImage<TComponentType, VImageDimension>   VectorImageType;
    typedef itk::Image<TComponentType, VImageDimension> ScalarImageType;
    typedef itk::ImageFileReader< VectorImageType >      ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >      WriterType;

    /** A pair indicating which functor should be used for an operator,
    * and whether the arguments should be swapped.
    */
    typedef std::pair< BinaryFunctorEnum, bool >        BinaryOperatorType;
    typedef std::map<std::string, BinaryOperatorType>   BinaryOperatorMapType;

    /** Declarations. */
    typename ReaderType::Pointer reader1 = ReaderType::New();
    typename ReaderType::Pointer reader2 = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();
    

    /** Available SimpleOperatorTypes are defined in itkLogicalFunctors.h:
    * AND, OR, XOR, NOT_AND, NOT_OR, NOT_XOR, ANDNOT, ORNOT
    *
    * The Simplification map (simpmap) defines for every possible logical
    * operation of the form
    *   [not]( ([not] A) [{&,|,^} ([not] B])] )
    * a simplified version.
    *
    * example1: A ^ (!B) = XORNOT(A,B) = NOT_XOR(A,B) = ! (A ^ B)
    * example2: (!A) & B = NOTAND(A,B) = ANDNOT(B,A) = B & (!A)
    **/

    BinaryOperatorMapType binaryOperatorMap;
    binaryOperatorMap["AND"]        = BinaryOperatorType(AND, false);
    binaryOperatorMap["OR"]         = BinaryOperatorType(OR, false);
    binaryOperatorMap["XOR"]        = BinaryOperatorType(XOR, false);
    binaryOperatorMap["ANDNOT"]     = BinaryOperatorType(ANDNOT, false);
    binaryOperatorMap["ORNOT"]      = BinaryOperatorType(ORNOT, false);
    binaryOperatorMap["XORNOT"]     = BinaryOperatorType(NOT_XOR, false);

    binaryOperatorMap["NOTAND"]     = BinaryOperatorType(ANDNOT, true);
    binaryOperatorMap["NOTOR"]      = BinaryOperatorType(ORNOT, true);
    binaryOperatorMap["NOTXOR"]     = BinaryOperatorType(NOT_XOR, false);

    binaryOperatorMap["NOTANDNOT"]  = BinaryOperatorType(NOT_OR, false);
    binaryOperatorMap["NOTORNOT"]   = BinaryOperatorType(NOT_AND, false);
    binaryOperatorMap["NOTXORNOT"]  = BinaryOperatorType(XOR, false);

    binaryOperatorMap["NOT_AND"]    = BinaryOperatorType(NOT_AND, false);
    binaryOperatorMap["NOT_OR"]     = BinaryOperatorType(NOT_OR, false);
    binaryOperatorMap["NOT_XOR"]    = BinaryOperatorType(NOT_XOR, false);
    binaryOperatorMap["NOT_NOT"]    = BinaryOperatorType(DUMMY, false);

    binaryOperatorMap["NOT_ANDNOT"] = BinaryOperatorType(ORNOT, true);
    binaryOperatorMap["NOT_ORNOT"]  = BinaryOperatorType(ANDNOT, true);
    binaryOperatorMap["NOT_XORNOT"] = BinaryOperatorType(XOR, false);

    binaryOperatorMap["NOT_NOTAND"] = BinaryOperatorType(ORNOT, false);
    binaryOperatorMap["NOT_NOTOR"]  = BinaryOperatorType(ANDNOT, false);
    binaryOperatorMap["NOT_NOTXOR"] = BinaryOperatorType(XOR, false);

    binaryOperatorMap["NOT_NOTANDNOT"] = BinaryOperatorType(OR, false);
    binaryOperatorMap["NOT_NOTORNOT"]  = BinaryOperatorType(AND, false);
    binaryOperatorMap["NOT_NOTXORNOT"] = BinaryOperatorType(NOT_XOR, false);

    /** Read the images. */
    reader1->SetFileName( m_InputFileName1.c_str() );
    std::cout << "Reading image1: " << m_InputFileName1 << std::endl;
    reader1->Update();
    std::cout << "Done reading image1." << std::endl;

    reader2->SetFileName( m_InputFileName2.c_str() );
    std::cout << "Reading image2: " << m_InputFileName2 << std::endl;
    reader2->Update();
    std::cout << "Done reading image2." << std::endl;

    /** Set up the logicalFilter */
    if ( binaryOperatorMap.count( m_Ops ) == 0 )
    {
      std::cerr << "ERROR: The desired operator is unknown: " << m_Ops << std::endl;
      return;
    }

    BinaryOperatorType logicalOperator;
    logicalOperator = binaryOperatorMap[ m_Ops ];
    bool swapArguments = logicalOperator.second;
    std::string withswapping = "";
    if ( swapArguments )
    {
      withswapping = " with swapped arguments";
    }
    std::cout
      << "The desired logical operation, "
      << m_Ops
      << ", is simplified to the "
      << logicalOperator.first
      << " operation"
      << withswapping
      << "."
      << std::endl;

    BinaryLogicalFunctorFactory<ScalarImageType> binaryFactory;
    typename itk::InPlaceImageFilter<ScalarImageType, ScalarImageType>::Pointer logicalFilter = binaryFactory.GetFilter(logicalOperator.first);
    
    // Create the filter which will assemble the component into the output image
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    
    std::cout
      << "Performing logical operation, "
      << m_Ops
      << ", on input image(s)..."
      << std::endl;

    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;
    
    for(unsigned int component = 0; component < reader1->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
    {
      typename ComponentExtractionType::Pointer componentExtractor1 = ComponentExtractionType::New();
      componentExtractor1->SetIndex(component);
      componentExtractor1->SetInput(reader1->GetOutput());
      componentExtractor1->Update();
      
      typename ComponentExtractionType::Pointer componentExtractor2 = ComponentExtractionType::New();
      componentExtractor2->SetIndex(component);
      componentExtractor2->SetInput(reader2->GetOutput());
      componentExtractor2->Update();

      if ( swapArguments )
      {
	/** swap the input files */
	logicalFilter->SetInput( 1, componentExtractor1->GetOutput() );
	logicalFilter->SetInput( 0, componentExtractor2->GetOutput() );
      }
      else
      {
	logicalFilter->SetInput( 0, componentExtractor1->GetOutput() );
	logicalFilter->SetInput( 1, componentExtractor2->GetOutput() );
      }
      logicalFilter->Update();
      
      imageToVectorImageFilter->SetNthInput(component, logicalFilter->GetOutput());
    }//end component loop

    std::cout << "Done performing logical operation." << std::endl;

    /** Write the image to disk */
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetUseCompression( m_UseCompression );
    std::cout << "Writing output to disk as: " << m_OutputFileName << std::endl;
    writer->Update();
    std::cout << "Done writing output to disk." << std::endl;
  }

}; // end LogicalImageOperator


int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-ops", "The operation to perform." );

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
  std::vector< std::string >  inputFileNames;
  parser->GetCommandLineArgument( "-in", inputFileNames );

  std::string ops = "AND";
  parser->GetCommandLineArgument( "-ops", ops );

  double argument = 0.0;
  bool retarg = parser->GetCommandLineArgument( "-arg", argument );

  const bool useCompression = parser->ArgumentExists( "-z" );

  /** Check if the required arguments are given. */
  if ( inputFileNames.size() != 2 && ops != "NOT" && ops != "NOT_NOT" && ops != "EQUAL" )
  {
    std::cerr << "ERROR: You should specify two input images." << std::endl;
    return 1;
  }

  std::string inputFileName1 = inputFileNames[ 0 ];
  std::string inputFileName2 = "";
  if( (inputFileNames.size() == 2) & (ops != "NOT") )
  {
    inputFileName2 = inputFileNames[ 1 ];
  }
  if ( ops == "EQUAL" && inputFileNames.size() > 1 && !retarg )
  {
    std::cerr << "ERROR: The operator \"EQUAL\" expects 1 input image and a \"-arg\"." << std::endl;
    return 1;
  }
  
  bool unary = false;
  if ( ops.compare("EQUAL") == 0 || ops.compare("NOT") == 0 )
  {
    unary = true;
  }

  /** outputFileName */
  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
  {
    /** get file name without its last (shortest) extension  */
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName1);
    /** get file name of a full filename (i.e. file name without path) */
    std::string part2;
    if ( inputFileName2 != "" )
    {
      part2 = itksys::SystemTools::GetFilenameName(inputFileName2);
    }
    else
    {
      part2 = "";
      part2 += itksys::SystemTools::GetFilenameLastExtension(inputFileName1);
    }
    /** compose outputfilename */
    outputFileName = part1 + ops + part2;
  }


  /** Class that does the work */
  LogicalImageOperatorBase * logicalImageOperator = NULL; 

  unsigned int imageDimension = 0;
  itktools::GetImageDimension(inputFileName1, imageDimension);

  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName1);
    
  /** NB: do not add floating point support, since logical operators are
  * not defined on those types */
    
  try
  {    
    // now call all possible template combinations.
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 2, unsigned char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 2, char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 2, unsigned short >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 2, unsigned short >::New( imageDimension, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 3, unsigned char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 3, char >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 3, unsigned short >::New( imageDimension, componentType );
    if (!logicalImageOperator) logicalImageOperator = LogicalImageOperator< 3, unsigned short >::New( imageDimension, componentType );
#endif
    if (!logicalImageOperator) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << imageDimension << std::endl
        << " pixel type = " << componentType << std::endl
        << std::endl;
      return 1;
    }

    logicalImageOperator->m_OutputFileName = outputFileName;
    logicalImageOperator->m_InputFileName1 = inputFileName1;
    logicalImageOperator->m_InputFileName2 = inputFileName2;
    logicalImageOperator->m_Ops = ops;
    logicalImageOperator->m_UseCompression = useCompression;
    logicalImageOperator->m_Argument = argument;
    logicalImageOperator->m_Unary = unary;
    
    logicalImageOperator->Run();
    
    delete logicalImageOperator;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete logicalImageOperator;
    return 1;
  }
  
  /** End program. */
  return 0;

} // end main
