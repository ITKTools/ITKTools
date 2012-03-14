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
 \brief This program generates a deformation field (from fixed to moving image) based on some corresponding points.

 \verbinclude deformationfieldgenerator.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "deformationfieldgenerator.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program generates a deformation field (from fixed\n"
    << "to moving image) based on some corresponding points.\n"
    << "Usage:\n"
    << "pxdeformationfieldgenerator\n"
    << "  -in1     inputFilename1: the fixed image on which the\n"
    << "           deformaton field must be defined.\n"
    << "  [-in2]   inputFilename2: only needed to convert from\n"
    << "           indices to point if the second input point\n"
    << "           contains indices.\n"
    << "  -ipp1    inputPointFile1: a transformix style input point file\n"
    << "           with points in the fixed image.\n"
    << "  -ipp2    inputPointFile2: a transformix style input point file\n"
    << "           with the corresponding points in the moving image.\n"
    << "  [-s]     stiffness: a number that allows to vary between\n"
    << "           interpolating and approximating spline.\n"
    << "           0.0 = interpolating = default.\n"
    << "           Stiffness values are usually rather small,\n"
    << "           typically in the range of 0.001 to 0.1.\n"
    << "  [-k]     kernelType: the type of kernel transform that's used to\n"
    << "           generate the deformation field.\n"
    << "           TPS: thin plate spline (default)\n"
    << "           TPSR2LOGR: thin plate spline R2logR\n"
    << "           VS: volume spline\n"
    << "           EBS: elastic body spline\n"
    << "           EBSR: elastic body reciprocal spline\n"
    << "           See ITK documentation and the there cited paper\n"
    << "           for more information on these methods.\n"
    << "  -out     outputFilename: the name of the resulting deformation field,\n"
    << "           which is written as a vector<float/double,dim> image.\n"
    << "  [-opct]  output pixel component type, choose one of {float, double}, default float.\n"
    << "Supported: 2D, 3D, any scalar input pixeltype.";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in1", "The inputImage1 filename." );
  parser->MarkArgumentAsRequired( "-ipp1", "The inputPoints1 filename." );
  parser->MarkArgumentAsRequired( "-ipp2", "The inputPoints2 filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );

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
  std::string inputImage1FileName = "";
  parser->GetCommandLineArgument( "-in1", inputImage1FileName );

  std::string inputImage2FileName = "";
  parser->GetCommandLineArgument( "-in2", inputImage2FileName );

  std::string inputPoints1FileName = "";
  parser->GetCommandLineArgument( "-ipp1", inputPoints1FileName );

  std::string inputPoints2FileName = "";
  parser->GetCommandLineArgument( "-ipp2", inputPoints2FileName );

  std::string outputImageFileName = "";
  parser->GetCommandLineArgument( "-out", outputImageFileName );

  std::string kernelName = "TPS";
  parser->GetCommandLineArgument( "-k", kernelName );

  double stiffness = 0.0;
  parser->GetCommandLineArgument( "-s", stiffness );

  /** Determine image properties. */
  itk::ImageIOBase::IOPixelType pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  unsigned int dim = 0;
  unsigned int numberOfComponents = 0;
  bool retgip = itktools::GetImageProperties(
    inputImage1FileName, pixelType, componentType, dim, numberOfComponents );
  if( !retgip ) return EXIT_FAILURE;

  /** Check for vector images. */
  //bool retNOCCheck = itktools::NumberOfComponentsCheck( numberOfComponents );
  //if( !retNOCCheck ) return EXIT_FAILURE;

  /** Output vector type is float or double. */
  std::string componentTypeAsString = "float";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );
  componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );

  /** Class that does the work. */
  ITKToolsDeformationFieldGeneratorBase * filter = NULL;

  try
  {
    // now call all possible template combinations.
    if( !filter ) filter = ITKToolsDeformationFieldGenerator< 2, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsDeformationFieldGenerator< 2, double >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter ) filter = ITKToolsDeformationFieldGenerator< 3, float >::New( dim, componentType );
    if( !filter ) filter = ITKToolsDeformationFieldGenerator< 3, double >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputImage1FileName = inputImage1FileName;
    filter->m_InputImage2FileName = inputImage2FileName;
    filter->m_InputPoints1FileName = inputPoints1FileName;
    filter->m_InputPoints2FileName = inputPoints2FileName;
    filter->m_OutputImageFileName = outputImageFileName;
    filter->m_KernelName = kernelName;
    filter->m_Stiffness = stiffness;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  /** End program. */
  return EXIT_SUCCESS;

} // end main
