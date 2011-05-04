#ifndef __LogicalImageOperatorHelper_h
#define __LogicalImageOperatorHelper_h

#include "itkBinaryFunctorImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkLogicalFunctors.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorImage.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include <map>
#include <utility>
#include <vector>
#include <itksys/SystemTools.hxx>


//-------------------------------------------------------------------------------------

/** run: A macro to call a function. */
#define run( function, type, dim ) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::VectorImage< type, dim > InputImageType; \
  function< InputImageType >( inputFileName1, inputFileName2, outputFileName, ops, useCompression, argument); \
  supported = true; \
}

//-------------------------------------------------------------------------------------

/** Macro for easily instantiating the correct logical functor
 * if, for example name is AND, the result is:
 * typedef itk::BinaryFunctorImageFilter<
 *   InputImageType,
 *   InputImageType,
 *   InputImageType,
 *   itk::Functor::AND<InputPixelType > ANDFilterType;
 * logicalFilter = (ANDFilterType::New()).GetPointer();
 *
 */
#define InstantiateUnaryLogicalFilter( name ) \
typedef itk::UnaryFunctorImageFilter< \
  ScalarImageType, ScalarImageType, \
  itk::Functor::localName##name<ScalarPixelType> >  name##FilterType; \
if ( logicalOperatorName == #name ) \
{ \
  typename name##FilterType::Pointer tempLogicalFilter = name##FilterType::New(); \
  tempLogicalFilter->GetFunctor().SetArgument( static_cast<ScalarPixelType>( argument ) ); \
  logicalFilter = tempLogicalFilter.GetPointer(); \
}

#define InstantiateBinaryLogicalFilter( name ) \
typedef itk::BinaryFunctorImageFilter< \
  ScalarImageType, ScalarImageType, ScalarImageType, \
  itk::Functor::localName##name<ScalarPixelType> >  name##FilterType; \
if ( logicalOperatorName == #name ) \
{ \
  logicalFilter = ( name##FilterType::New() ).GetPointer(); \
}


/**
 * ******************* LogicalOps *******************
 */

template< class InputImageType >
void LogicalImageOperator(
  const std::string & inputFileName1,
  const std::string & inputFileName2,
  const std::string & outputFileName,
  const std::string & ops,
  const bool useCompression,
  const double & argument)
{
  /** Typedefs. */
  typedef typename InputImageType::PixelType                  InputPixelType;
  typedef typename InputImageType::InternalPixelType          ScalarPixelType;
  typedef itk::Image<ScalarPixelType, InputImageType::ImageDimension> ScalarImageType;
  typedef itk::ImageToImageFilter<
    ScalarImageType, ScalarImageType>                  BaseFilterType;
  /** \todo: write a real dummy filter which does really nothing */
  typedef itk::CastImageFilter<
    ScalarImageType,ScalarImageType >                   DummyFilterType;
  typedef itk::ImageFileReader< InputImageType >      ReaderType;
  typedef itk::ImageFileWriter< InputImageType >      WriterType;

  /** A pair indicating which functor should be used for an operator,
   * and whether the arguments should be swapped.
   */
  typedef std::pair< std::string, bool >              SimpleOperatorType;
  typedef std::map<std::string, SimpleOperatorType>   SimplifyMapType;

  /** Declarations. */
  typename BaseFilterType::Pointer logicalFilter = 0;
  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = 0;
  typename WriterType::Pointer writer = WriterType::New();
  SimplifyMapType simpmap;

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

  simpmap["AND"]        = SimpleOperatorType("AND", false); // SimpleOperatorType
  simpmap["OR"]         = SimpleOperatorType("OR", false); // SimpleOperatorType
  simpmap["XOR"]        = SimpleOperatorType("XOR", false); // SimpleOperatorType
  simpmap["NOT"]        = SimpleOperatorType("NOT", false); // SimpleOperatorType
  simpmap["EQUAL"]      = SimpleOperatorType("EQUAL", false); // SimpleOperatorType

  simpmap["ANDNOT"]     = SimpleOperatorType("ANDNOT", false); // SimpleOperatorType
  simpmap["ORNOT"]      = SimpleOperatorType("ORNOT", false); // SimpleOperatorType
  simpmap["XORNOT"]     = SimpleOperatorType("NOT_XOR", false); // see example1

  simpmap["NOTAND"]     = SimpleOperatorType("ANDNOT", true); // see example2
  simpmap["NOTOR"]      = SimpleOperatorType("ORNOT", true);
  simpmap["NOTXOR"]     = SimpleOperatorType("NOT_XOR", false);

  simpmap["NOTANDNOT"]  = SimpleOperatorType("NOT_OR", false);
  simpmap["NOTORNOT"]   = SimpleOperatorType("NOT_AND", false);
  simpmap["NOTXORNOT"]  = SimpleOperatorType("XOR", false);

  simpmap["NOT_AND"]    = SimpleOperatorType("NOT_AND", false); // SimpleOperatorType
  simpmap["NOT_OR"]     = SimpleOperatorType("NOT_OR", false); // SimpleOperatorType
  simpmap["NOT_XOR"]    = SimpleOperatorType("NOT_XOR", false); // SimpleOperatorType
  simpmap["NOT_NOT"]    = SimpleOperatorType("DUMMY", false); // SimpleOperatorType

  simpmap["NOT_ANDNOT"] = SimpleOperatorType("ORNOT", true);
  simpmap["NOT_ORNOT"]  = SimpleOperatorType("ANDNOT", true);
  simpmap["NOT_XORNOT"] = SimpleOperatorType("XOR", false);

  simpmap["NOT_NOTAND"] = SimpleOperatorType("ORNOT", false);
  simpmap["NOT_NOTOR"]  = SimpleOperatorType("ANDNOT", false);
  simpmap["NOT_NOTXOR"] = SimpleOperatorType("XOR", false);

  simpmap["NOT_NOTANDNOT"] = SimpleOperatorType("OR", false);
  simpmap["NOT_NOTORNOT"]  = SimpleOperatorType("AND", false);
  simpmap["NOT_NOTXORNOT"] = SimpleOperatorType("NOT_XOR", false);

  /** Read the images. */
  reader1->SetFileName( inputFileName1.c_str() );
  std::cout << "Reading image1: " << inputFileName1 << std::endl;
  reader1->Update();
  std::cout << "Done reading image1." << std::endl;

  if ( inputFileName2 != "" )
  {
    reader2 = ReaderType::New();
    reader2->SetFileName( inputFileName2.c_str() );
    std::cout << "Reading image2: " << inputFileName2 << std::endl;
    reader2->Update();
    std::cout << "Done reading image2." << std::endl;
  }

  /** Set up the logicalFilter */
  SimpleOperatorType logicalOperator = SimpleOperatorType( "DUMMY", false );
  std::string logicalOperatorName = logicalOperator.first;
  bool swapArguments = logicalOperator.second;
  if ( simpmap.count( ops ) != 0 )
  {
    logicalOperator = simpmap[ ops ];
    logicalOperatorName = logicalOperator.first;
    swapArguments = logicalOperator.second;
    if ( ops != logicalOperatorName )
    {
      std::string withswapping = "";
      if ( swapArguments )
      {
        withswapping = " with swapped arguments";
      }
      std::cout
        << "The desired logical operation, "
        << ops
        << ", is simplified to the "
        << logicalOperator.first
        << " operation"
        << withswapping
        << "."
        << std::endl;
    }
  }
  else
  {
    std::cerr << "ERROR: The desired operator is unknown: " << ops << std::endl;
    std::cerr << "The output will just be a copy of " << inputFileName1 << "." << std::endl;
  }

  /** Assign logicalFilter the correct filter (as defined by the logicalOperatorName). */
  if ( logicalOperatorName == "DUMMY" )
  {
    logicalFilter = (DummyFilterType::New()).GetPointer();
  }

  typedef itk::VectorIndexSelectionCastImageFilter<InputImageType, ScalarImageType> ComponentExtractionType;
  
  InstantiateUnaryLogicalFilter( EQUAL );
  InstantiateUnaryLogicalFilter( NOT );

  InstantiateBinaryLogicalFilter( AND );
  InstantiateBinaryLogicalFilter( OR );
  InstantiateBinaryLogicalFilter( XOR );
  InstantiateBinaryLogicalFilter( NOT_AND );
  InstantiateBinaryLogicalFilter( NOT_OR );
  InstantiateBinaryLogicalFilter( NOT_XOR );
  InstantiateBinaryLogicalFilter( ANDNOT );
  InstantiateBinaryLogicalFilter( ORNOT );

  // Create the filter which will assemble the component into the output image
  typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
  typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
  
  std::cout
    << "Performing logical operation, "
    << logicalOperatorName
    << ", on input image(s)..."
    << std::endl;
    
  for(unsigned int component = 0; component < reader1->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
  {
    typename ComponentExtractionType::Pointer componentExtractor1 = ComponentExtractionType::New();
    componentExtractor1->SetIndex(component);
    componentExtractor1->SetInput(reader1->GetOutput());
    componentExtractor1->Update();
    
    typename ComponentExtractionType::Pointer componentExtractor2 = ComponentExtractionType::New();
    if ( reader2.IsNotNull() )
      {
      componentExtractor2->SetIndex(component);
      componentExtractor2->SetInput(reader2->GetOutput());
      componentExtractor2->Update();
      }

    if ( swapArguments )
    {
      /** swap the input files */
      // Is this safe? It seems like this only makes sense if reader2 is specified.
      logicalFilter->SetInput( 1, componentExtractor1->GetOutput() );
      logicalFilter->SetInput( 0, componentExtractor2->GetOutput() );
    }
    else
    {
      logicalFilter->SetInput( 0, componentExtractor1->GetOutput() );
      if ( reader2.IsNotNull() )
      {
        logicalFilter->SetInput( 1, componentExtractor2->GetOutput() );
      }
    }
    logicalFilter->Update();
    
    imageToVectorImageFilter->SetNthInput(component, logicalFilter->GetOutput());
  }//end component loop

  std::cout << "Done performing logical operation." << std::endl;

  /** Write the image to disk */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( imageToVectorImageFilter->GetOutput() );
  writer->SetUseCompression( useCompression );
  std::cout << "Writing output to disk as: " << outputFileName << std::endl;
  writer->Update();
  std::cout << "Done writing output to disk." << std::endl;

} // end LogicalOps()


/**
 * ******************* PrintHelp *******************
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


#endif // end __LogicalImageOperatorHelper_h
