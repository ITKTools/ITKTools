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
 \brief This program converts between deformations (displacement fields) and transformations, and computes the magnitude or Jacobian of a deformation field.
 
 \verbinclude deformationfieldoperator.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "deformationfieldoperator.h"
#include "itkExceptionObject.h"
#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
  << "pxdeformationfieldoperator" << std::endl
    << "This program converts between deformations (displacement fields)" << std::endl
  << "and transformations, and computes the magnitude or Jacobian of a" << std::endl
  << "deformation field." << std::endl
  << "  -in      inputFilename" << std::endl
  << "  [-out]   outputFilename; default: in + {operation}.mhd" << std::endl
  << "  [-ops]   operation; options: DEF2TRANS, TRANS2DEF," << std::endl
  << "MAGNITUDE, JACOBIAN, DEF2JAC, TRANS2JAC, INVERSE. default: MAGNITUDE" << std::endl
  << "           TRANS2JAC == JACOBIAN" << std::endl
  << "  [-s]     number of streams, default 1." << std::endl
  << "  [-it]    number of iterations, for the iterative inversion, default 1, increase to get better results." << std::endl
  << "  [-stop]  allowed error, default 0.0, increase to get faster convergence." << std::endl
  << "Supported: 2D, 3D, vector of floats or doubles, number of components" << std::endl
  << "must equal number of dimensions.";
  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

/** DeformationFieldOperator */

class ITKToolsDeformationFieldOperatorBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsDeformationFieldOperatorBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Ops = "";
    this->m_NumberOfStreams = 0;
    this->m_NumberOfIterations = 0;
    this->m_StopValue = 0.0f;
  };
  ~ITKToolsDeformationFieldOperatorBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::string m_Ops;
  unsigned int m_NumberOfStreams;
  unsigned int m_NumberOfIterations;
  double m_StopValue;
    
}; // end DeformationFieldOperatorBase

/**
 * **************** DeformationFieldOperator *******************
 *
 * converts between deformation fields and transformation 'fields',
 * and compute magnitudes/Jacobians.
 */
template< class TComponentType, unsigned int VDimension >
class ITKToolsDeformationFieldOperator : public ITKToolsDeformationFieldOperatorBase
{
public:
  typedef ITKToolsDeformationFieldOperator Self;

  ITKToolsDeformationFieldOperator(){};
  ~ITKToolsDeformationFieldOperator(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** TYPEDEF's. */
    typedef TComponentType                               ComponentType;
    typedef ComponentType                                ScalarPixelType;
    typedef itk::Vector< ComponentType, VDimension >     VectorPixelType;

    typedef itk::Image< ScalarPixelType, VDimension >    ScalarImageType;
    typedef itk::Image< VectorPixelType, VDimension >    VectorImageType;
    typedef itk::ImageFileReader< VectorImageType >     ReaderType;

    /** DECLARATION'S. */
    typename VectorImageType::Pointer workingImage;
    typename ReaderType::Pointer reader = ReaderType::New();

    /** Read in the inputImage. */
    reader->SetFileName( this->m_InputFileName.c_str() );
    // temporarily: only streaming support for Jacobian case needed for EMPIRE10 challenge.
    if ( this->m_Ops != "DEF2JAC" && this->m_Ops != "TRANS2JAC" && this->m_Ops != "JACOBIAN" )
    {
      std::cout << "Reading input image: " << this->m_InputFileName << std::endl;
      reader->Update();
      std::cout << "Input image read." << std::endl;
    }

    /** Change to Transformation or Deformation by adding/subtracting pixel coordinates */
    workingImage = reader->GetOutput();

    /** Do something with this image and save the result */
    if ( this->m_Ops == "DEF2TRANS" )
    {
      Deformation2Transformation<VectorImageType>(
	workingImage, this->m_OutputFileName, true );
    }
    else if ( this->m_Ops == "TRANS2DEF" )
    {
      Deformation2Transformation<VectorImageType>(
	workingImage, this->m_OutputFileName, false );
    }
    else if ( this->m_Ops == "MAGNITUDE" )
    {
      ComputeMagnitude<VectorImageType, ScalarImageType>(
	workingImage, this->m_OutputFileName );
    }
    else if ( this->m_Ops == "JACOBIAN" || this->m_Ops == "TRANS2JAC" )
    {
      ComputeJacobian<VectorImageType, ScalarImageType>(
	m_InputFileName, this->m_OutputFileName, this->m_NumberOfStreams, true );
    }
    else if ( this->m_Ops == "DEF2JAC" )
    {
      ComputeJacobian<VectorImageType, ScalarImageType>(
	m_InputFileName, this->m_OutputFileName, this->m_NumberOfStreams, false );
    }
    else if ( this->m_Ops == "INVERSE" )
    {
      ComputeInverse<VectorImageType>(
	m_InputFileName, this->m_OutputFileName, this->m_NumberOfStreams, this->m_NumberOfIterations, this->m_StopValue );
    }
    else
    {
      itkGenericExceptionMacro( << "<< invalid operator: " << this->m_Ops );
    }
  }

}; // end DeformationFieldOperator

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  std::string ops = "MAGNITUDE";
  parser->GetCommandLineArgument( "-ops", ops );

  std::string outputFileName = "";
  parser->GetCommandLineArgument( "-out", outputFileName );
  if ( outputFileName == "" )
  {
    std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension(inputFileName);
    std::string ext =
      itksys::SystemTools::GetFilenameLastExtension(inputFileName);
    outputFileName = part1 + ops + ext;
  }

  /** Support for streaming. */
  unsigned int numberOfStreams = 1;
  parser->GetCommandLineArgument( "-s", numberOfStreams );

  /** Parameters for the inversion. */
  unsigned int numberOfIterations = 1;
  parser->GetCommandLineArgument( "-it", numberOfIterations );

  double stopValue = 0.0;
  parser->GetCommandLineArgument( "-stop", stopValue );

  /** Determine image properties. */
  std::string ComponentType = "float";
  std::string PixelType = "VECTOR";
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = Dimension;
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

  /** Checks. */
  if ( NumberOfComponents != Dimension )
  {
    std::cerr << "ERROR: The NumberOfComponents must equal the Dimension!" << std::endl;
    return 1;
  }
  if ( NumberOfComponents == 1 )
  {
    std::cerr << "Scalar images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentType );

  /** Run the program. */
  
  /** Class that does the work */
  ITKToolsDeformationFieldOperatorBase * deformationFieldOperator = 0; 

  /** Short alias */
  unsigned int dim = Dimension;

  itktools::ComponentType componentType = itk::ImageIOBase::GetComponentTypeFromString( PixelType );
   
  try
  {    
    // now call all possible template combinations.
    if (!deformationFieldOperator) deformationFieldOperator = ITKToolsDeformationFieldOperator< float, 2 >::New( componentType, dim );
    if (!deformationFieldOperator) deformationFieldOperator = ITKToolsDeformationFieldOperator< double, 2 >::New( componentType, dim );
#ifdef ITKTOOLS_3D_SUPPORT
    if (!deformationFieldOperator) deformationFieldOperator = ITKToolsDeformationFieldOperator< float, 3 >::New( componentType, dim );
    if (!deformationFieldOperator) deformationFieldOperator = ITKToolsDeformationFieldOperator< double, 3 >::New( componentType, dim );
#endif
    if (!deformationFieldOperator) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    deformationFieldOperator->m_InputFileName = inputFileName;
    deformationFieldOperator->m_OutputFileName = outputFileName;
    deformationFieldOperator->m_Ops = ops;
    deformationFieldOperator->m_NumberOfStreams = numberOfStreams;
    deformationFieldOperator->m_NumberOfIterations = numberOfIterations;
    deformationFieldOperator->m_StopValue = stopValue;
  
    deformationFieldOperator->Run();
    
    delete deformationFieldOperator;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete deformationFieldOperator;
    return 1;
  }

  /** End program. */
  return 0;

} // end main

