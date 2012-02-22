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
#ifndef __logicalimageoperator_h_
#define __logicalimageoperator_h_

#include "itkUnaryFunctorImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkUnaryLogicalFunctors.h"
#include "itkBinaryLogicalFunctors.h"

#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkImageFileWriter.h"

#include <map>
#include <utility>
#include <vector>
#include <itksys/SystemTools.hxx>


/** \class ITKToolsLogicalImageOperatorBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsLogicalImageOperatorBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsLogicalImageOperatorBase()
  {
    this->m_InputFileName1 = "";
    this->m_InputFileName2 = "";
    this->m_OutputFileName = "";
    this->m_Ops = "";
    this->m_UseCompression = false;
    this->m_Argument = 0.0f;
    this->m_Unary = false;
  };
  /** Destructor. */
  ~ITKToolsLogicalImageOperatorBase(){};

  /** Input member parameters. */
  std::string m_InputFileName1;
  std::string m_InputFileName2;
  std::string m_OutputFileName;
  std::string m_Ops;
  bool m_UseCompression;
  double m_Argument;
  bool m_Unary; // is the operator to be performed unary? (else it is binary)

}; // end class ITKToolsLogicalImageOperatorBase


/** \class ITKToolsLogicalImageOperator
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsLogicalImageOperator : public ITKToolsLogicalImageOperatorBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsLogicalImageOperator Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsLogicalImageOperator(){};
  ~ITKToolsLogicalImageOperator(){};

  /** Run function. */
  void Run( void )
  {
    if( this->m_Unary ) this->RunUnary();
    else this->RunBinary();

  } // end Run()

  /** RunUnary function. */
  void RunUnary( void )
  {
    /** Typedefs. */
    typedef itk::VectorImage<TComponentType, VDimension>  VectorImageType;
    typedef itk::Image<TComponentType, VDimension>        ScalarImageType;
    typedef itk::ImageFileReader< VectorImageType >       ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >       WriterType;

    /** Declarations. */
    typename ReaderType::Pointer reader1 = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    /** Read the images. */
    reader1->SetFileName( this->m_InputFileName1.c_str() );
    std::cout << "Reading image1: " << this->m_InputFileName1 << std::endl;
    reader1->Update();
    std::cout << "Done reading image1." << std::endl;

    UnaryFunctorEnum unaryOperation;
    if( this->m_Ops.compare( "EQUAL" ) )
    {
      unaryOperation = EQUAL;
    }
    else if( this->m_Ops.compare( "NOT" ) )
    {
      unaryOperation = NOT;
    }
    else
    {
      std::cerr << "Invalid operator: " << this->m_Ops << std::endl;
      return;
    }

    UnaryLogicalFunctorFactory<ScalarImageType> unaryFactory;
    typename itk::InPlaceImageFilter<ScalarImageType, ScalarImageType>::Pointer logicalFilter
      = unaryFactory.GetFilter( unaryOperation, static_cast<TComponentType>( this->m_Argument ) );

    // Create the filter which will assemble the component into the output image
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    std::cout
      << "Performing logical operation, "
      << this->m_Ops
      << ", on input image(s)..."
      << std::endl;

    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;

    for( unsigned int component = 0; component < reader1->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
    {
      typename ComponentExtractionType::Pointer componentExtractor1 = ComponentExtractionType::New();
      componentExtractor1->SetIndex(component);
      componentExtractor1->SetInput(reader1->GetOutput());
      componentExtractor1->Update();
      logicalFilter->SetInput( componentExtractor1->GetOutput() );

      logicalFilter->Update();

      imageToVectorImageFilter->SetNthInput( component, logicalFilter->GetOutput() );
    } // end component loop

    /** Write the image to disk */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();

  } // end RunUnary()

  /** RunBinary. */
  void RunBinary( void )
  {
    /** Typedefs. */
    typedef itk::VectorImage<TComponentType, VDimension>  VectorImageType;
    typedef itk::Image<TComponentType, VDimension>        ScalarImageType;
    typedef itk::ImageFileReader< VectorImageType >       ReaderType;
    typedef itk::ImageFileWriter< VectorImageType >       WriterType;

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
     */

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
    reader1->SetFileName( this->m_InputFileName1.c_str() );
    std::cout << "Reading image1: " << this->m_InputFileName1 << std::endl;
    reader1->Update();
    std::cout << "Done reading image1." << std::endl;

    reader2->SetFileName( this->m_InputFileName2.c_str() );
    std::cout << "Reading image2: " << this->m_InputFileName2 << std::endl;
    reader2->Update();
    std::cout << "Done reading image2." << std::endl;

    /** Set up the logicalFilter */
    if( binaryOperatorMap.count( this->m_Ops ) == 0 )
    {
      std::cerr << "ERROR: The desired operator is unknown: " << this->m_Ops << std::endl;
      return;
    }

    BinaryOperatorType logicalOperator;
    logicalOperator = binaryOperatorMap[ this->m_Ops ];
    bool swapArguments = logicalOperator.second;
    std::string withswapping = "";
    if( swapArguments )
    {
      withswapping = " with swapped arguments";
    }
    std::cout
      << "The desired logical operation, "
      << this->m_Ops
      << ", is simplified to the "
      << logicalOperator.first
      << " operation"
      << withswapping
      << "."
      << std::endl;

    BinaryLogicalFunctorFactory<ScalarImageType> binaryFactory;
    typename itk::InPlaceImageFilter<ScalarImageType, ScalarImageType>::Pointer logicalFilter
      = binaryFactory.GetFilter( logicalOperator.first );

    // Create the filter which will assemble the component into the output image
    typedef itk::ImageToVectorImageFilter<ScalarImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();

    std::cout
      << "Performing logical operation, "
      << this->m_Ops
      << ", on input image(s)..."
      << std::endl;

    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> ComponentExtractionType;

    for( unsigned int component = 0; component < reader1->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
    {
      typename ComponentExtractionType::Pointer componentExtractor1 = ComponentExtractionType::New();
      componentExtractor1->SetIndex(component);
      componentExtractor1->SetInput(reader1->GetOutput());
      componentExtractor1->Update();

      typename ComponentExtractionType::Pointer componentExtractor2 = ComponentExtractionType::New();
      componentExtractor2->SetIndex(component);
      componentExtractor2->SetInput(reader2->GetOutput());
      componentExtractor2->Update();

      if( swapArguments )
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
    } // end component loop

    /** Write the image to disk */
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( imageToVectorImageFilter->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();

  } // end RunBinary()

}; // end class LogicalImageOperator


#endif // end #ifndef __logicalimageoperator_h_

