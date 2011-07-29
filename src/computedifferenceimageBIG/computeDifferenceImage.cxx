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
 \brief Compute the difference image between two images.
 
 \verbinclude computedifferenceimage.help
 */

#include <iostream>
#include "itkImageFileReader.h"

/* In order to determine if argv[1] is a directory or a file,
 * so that we can distinguish between dicom and other files.
 */
#include <itksys/SystemTools.hxx>

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"


std::string GetHelpString()
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
     << "pxcomputedifferenceimage inputimage1filename inputimage2filename [outputimagefilename] [outputPixelComponentType]" << std::endl
     << "where outputPixelComponentType is one of:" << std::endl
     << "- unsigned_char" << std::endl
     << "- char" << std::endl
     << "- unsigned_short" << std::endl
     << "- short" << std::endl
     << "- unsigned_int" << std::endl
     << "- int" << std::endl
     << "- unsigned_long" << std::endl
     << "- long" << std::endl
     << "- float" << std::endl
     << "- double" << std::endl
     << "provided that the outputPixelComponentType is supported by the output file format." << std::endl
     << "By default the outputPixelComponentType is set to the inputPixelComponentType of image1.";
  return ss.str();
}

// extern int ComputeScalarDifferenceImage( const std::string &inputPixelComponentType1,
//   const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
//   const std::string &image1FileName, const std::string &image2FileName,
//   const std::string &outputFileName, int inputDimension);

extern int ComputeVectorDifferenceImage( const std::string &inputPixelComponentType1,
  const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
  const std::string &image1FileName, const std::string &image2FileName,
  const std::string &outputFileName, int inputDimension, int vectorDimension);

//-------------------------------------------------------------------------------------

int  main(  int  argc,  char *argv[] )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  parser->MarkArgumentAsRequired( "-in1", "The input1 filename." );
  parser->MarkArgumentAsRequired( "-in2", "The input2 filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get the image names. */
  std::string image1FileName;
  std::string image2FileName;
  std::string outputFileName = "";
  std::string outputPixelComponentType = "";

  parser->GetCommandLineArgument( "-in1", image1FileName );
  parser->GetCommandLineArgument( "-in2", image2FileName );
  parser->GetCommandLineArgument( "-out", outputFileName );
  parser->GetCommandLineArgument( "-pct", outputPixelComponentType );
  
  bool gotOutputFileName = parser->GetCommandLineArgument( "-out", outputFileName );
  
  if ( !gotOutputFileName )
  {
    std::string::size_type slash = image2FileName.find_last_of( "/" ) + 1;
    outputFileName = image1FileName.substr( 0, image1FileName.rfind( "." ) );
    outputFileName += "MINUS";
    outputFileName += image2FileName.substr( slash, image2FileName.rfind( "." ) - slash );
    outputFileName += ".mhd";
  }

  /** Check if image1FileName and image2FileName exist. */
  bool exists1 = itksys::SystemTools::FileExists( image1FileName.c_str() );
  bool exists2 = itksys::SystemTools::FileExists( image2FileName.c_str() );

  if ( !exists1 || !exists2 )
  {
    /** Something is wrong. */
    std::cerr << "ERROR: first input argument does not exist!" << std::endl;
    return 1;
  }

  /** Check outputPixelType. */
  if ( outputPixelComponentType != ""
    && outputPixelComponentType != "unsigned_char"
    && outputPixelComponentType != "char"
    && outputPixelComponentType != "unsigned_short"
    && outputPixelComponentType != "short"
    && outputPixelComponentType != "unsigned_int"
    && outputPixelComponentType != "int"
    && outputPixelComponentType != "unsigned_long"
    && outputPixelComponentType != "long"
    && outputPixelComponentType != "float"
    && outputPixelComponentType != "double" )
  {
    /** In this case an illegal outputPixelComponentType is given. */
    std::cerr << "The given outputPixelComponentType is \"" << outputPixelComponentType
      << "\", which is not supported." << std::endl;
    return 1;
  }

  /** TASK 2:
   * Typedefs and test reading to determine correct image types.
   * *******************************************************************
   */

  /** Initial image type. */
  const unsigned int    Dimension  =  3;
  typedef short         PixelType;

  /** Some typedef's. */
  typedef itk::Image< PixelType, Dimension >      ImageType;
  typedef itk::ImageFileReader< ImageType >       ReaderType;
  typedef itk::ImageIOBase                        ImageIOBaseType;

  /** Create testReaders. */
  ReaderType::Pointer testReader1 = ReaderType::New();
  ReaderType::Pointer testReader2 = ReaderType::New();

  /** Setup the testReaders. */
  testReader1->SetFileName( image1FileName.c_str() );
  testReader2->SetFileName( image2FileName.c_str() );

  /** Generate all information. */
  testReader1->GenerateOutputInformation();
  testReader2->GenerateOutputInformation();

  /** Extract the ImageIO from the testReaders. */
  ImageIOBaseType::Pointer testImageIOBase1 = testReader1->GetImageIO();
  ImageIOBaseType::Pointer testImageIOBase2 = testReader2->GetImageIO();

  /** Get the component type, number of components, dimension and pixel type of image1. */
  unsigned int inputDimension1 = testImageIOBase1->GetNumberOfDimensions();
  unsigned int numberOfComponents1 = testImageIOBase1->GetNumberOfComponents();
  std::string inputPixelComponentType1 = testImageIOBase1->GetComponentTypeAsString(
    testImageIOBase1->GetComponentType() );
  std::string pixelType1 = testImageIOBase1->GetPixelTypeAsString(
    testImageIOBase1->GetPixelType() );

  /** Get the component type, number of components, dimension and pixel type of image2. */
  unsigned int inputDimension2 = testImageIOBase2->GetNumberOfDimensions();
  unsigned int numberOfComponents2 = testImageIOBase2->GetNumberOfComponents();
  std::string inputPixelComponentType2 = testImageIOBase2->GetComponentTypeAsString(
    testImageIOBase2->GetComponentType() );
  std::string pixelType2 = testImageIOBase2->GetPixelTypeAsString(
    testImageIOBase2->GetPixelType() );

  /** TASK 3:
   * Do some preparations and checks.
   * *******************************************************************
   */

  /** Check dimension equality. */
  if ( inputDimension1 != inputDimension2 )
  {
    std::cerr << "The dimensions of the input images are "
      << inputDimension1 << " and "
      << inputDimension2 << "." << std::endl;
    std::cerr << "They should match!" << std::endl;
    return 1;
  }

  /** Check equality of the pixel type. */
  if ( pixelType1 != pixelType2 )
  {
    std::cerr << "The pixel type of the input images are "
      << pixelType1 << " and "
      << pixelType2 << "." << std::endl;
    std::cerr << "They should match!" << std::endl;
    return 1;
  }

  /** Check equality of the number of components. */
  if ( numberOfComponents1 != numberOfComponents2 )
  {
    std::cerr << "The number of components of the input images are "
      << numberOfComponents1 << " and "
      << numberOfComponents2 << "." << std::endl;
    std::cerr << "They should match!" << std::endl;
    return 1;
  }

  /** Check inputPixelComponentType1. */
  if ( inputPixelComponentType1 != "unsigned_char"
    && inputPixelComponentType1 != "char"
    && inputPixelComponentType1 != "unsigned_short"
    && inputPixelComponentType1 != "short"
    && inputPixelComponentType1 != "unsigned_int"
    && inputPixelComponentType1 != "int"
    && inputPixelComponentType1 != "unsigned_long"
    && inputPixelComponentType1 != "long"
    && inputPixelComponentType1 != "float"
    && inputPixelComponentType1 != "double" )
  {
    /** In this case an illegal inputPixelComponentType is found. */
    std::cerr << "The found inputPixelComponentType of image1 is \"" << inputPixelComponentType1
      << "\", which is not supported." << std::endl;
    return 1;
  }

  /** Check inputPixelType2. */
  if ( inputPixelComponentType2 != "unsigned_char"
    && inputPixelComponentType2 != "char"
    && inputPixelComponentType2 != "unsigned_short"
    && inputPixelComponentType2 != "short"
    && inputPixelComponentType2 != "unsigned_int"
    && inputPixelComponentType2 != "int"
    && inputPixelComponentType2 != "unsigned_long"
    && inputPixelComponentType2 != "long"
    && inputPixelComponentType2 != "float"
    && inputPixelComponentType2 != "double" )
  {
    /** In this case an illegal inputPixelComponentType is found. */
    std::cerr << "The found inputPixelComponentType of image2 is \"" << inputPixelComponentType2
      << "\", which is not supported." << std::endl;
    return 1;
  }

  /** Check outputPixelType. */
  if ( outputPixelComponentType == "" )
  {
    /** In this case this option is not given, and by default
    * we set it to the inputPixelComponentType.
    */
    outputPixelComponentType = inputPixelComponentType1;
  }

  /** Get rid of the "_" in inputPixelComponentTypes and outputPixelComponentType. */
  std::basic_string<char>::size_type pos = inputPixelComponentType1.find( "_" );
  static const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos )
  {
    inputPixelComponentType1.replace( pos, 1, " " );
  }
  pos = inputPixelComponentType2.find( "_" );
  if ( pos != npos )
  {
    inputPixelComponentType2.replace( pos, 1, " " );
  }
  pos = outputPixelComponentType.find( "_" );
  if ( pos != npos )
  {
    outputPixelComponentType.replace( pos, 1, " " );
  }

  /** TASK 4:
   * Now we are ready to check on image type and subsequently call the
   * correct ComputeDifference-function.
   * *******************************************************************
   */

  try
  {
    /**
     * ****************** Support for SCALAR pixel types. **********************************
     *
    if ( strcmp( pixelType1.c_str(), "scalar" ) == 0 && numberOfComponents1 == 1 )
    {
      const int ret_value = ComputeScalarDifferenceImage(
        inputPixelComponentType1, inputPixelComponentType2,
        outputPixelComponentType, image1FileName, image2FileName,
        outputFileName, inputDimension1 );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    } // end scalar support */
    /**
     * ****************** Support for VECTOR pixel types. **********************************
     */
    //else if ( strcmp( pixelType1.c_str(), "vector" ) == 0 )
    if ( numberOfComponents1 > 1 )
    {
      const int ret_value = ComputeVectorDifferenceImage(
        inputPixelComponentType1, inputPixelComponentType2,
        outputPixelComponentType, image1FileName, image2FileName,
        outputFileName, inputDimension1, numberOfComponents1 );
      if ( ret_value != 0 )
      {
        return ret_value;
      }
    } // end vector support
    else
    {
      std::cerr << "Pixel types are " << pixelType1
        << ", component types are " << inputPixelComponentType1
        << " and number of components equals " << numberOfComponents1 << "." << std::endl;
      std::cerr << "ERROR: This image type is not supported." << std::endl;
      return 1;
    }
  } // end try

  /** If any errors have occurred, catch and print the exception and return false. */
  catch( itk::ExceptionObject  &  err  )
  {
    std::cerr  << "ExceptionObject caught !"  << std::endl;
    std::cerr  << err <<  std::endl;
    return 1;
  }

  /** End  program. Return success. */
  return 0;

}  // end main
