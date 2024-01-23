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
 \brief Create a simple box image.

 \verbinclude createsimplebox.help
 */

/** Setup Mevislab DicomTiff IO support */
#include "itkUseMevisDicomTiff.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "createsimplebox.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "This program creates an image containing a white box, defined by point A and B.\n"
    << "Usage:\n"
    << "pxcreatesimplebox\n"
    << "[-in]  InputImageFileName\n"
    << "Size, origin, and spacing for the output image will be taken\n"
    << "from this image. NB: not the dimension and the pixeltype;\n"
    << "you must set them anyway!\n"
    << "-out   OutputImageFileName\n"
    << "-pt    PixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>\n"
    << "Currently only char, uchar and short are supported.\n"
    << "-id    ImageDimension <2,3>\n"
    << "[-d0]  Size of dimension 0\n"
    << "[-d1]  Size of dimension 1\n"
    << "[-d2]  Size of dimension 2\n"
    << "-pA0  Index 0 of pointA\n"
    << "-pA1  Index 1 of pointA\n"
    << "[-pA2]Index 2 of pointA\n"
    << "-pB0  Index 0 of pointB\n"
    << "-pB1  Index 1 of pointB\n"
    << "[-pB2]Index 2 of pointB";

  return ss.str();

} // end GetHelpString()

//-------------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  RegisterMevisDicomTiff();

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "Output filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Read the dimension. */
  unsigned int dim = 0;
  parser->GetCommandLineArgument( "-id", dim );

  if( dim == 0 )
  {
    std::cerr << "ERROR: Image dimension cannot be 0" << std::endl;
    return EXIT_FAILURE;
  }

  std::string componentTypeAsString = "";
  bool retopct = parser->GetCommandLineArgument( "-opct", componentTypeAsString );

  std::string outputFileName;
  parser->GetCommandLineArgument( "-out", outputFileName );

  std::string inputFileName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputFileName );

  /** read point A and B from the commandline.*/
  std::vector<unsigned int> indexA;
  parser->GetCommandLineArgument( "-pA", indexA );

  std::vector<unsigned int> indexB;
  parser->GetCommandLineArgument( "-pB", indexB );

  std::vector<unsigned int> boxSize;
  parser->GetCommandLineArgument( "-d", boxSize );

  itk::ImageIOBase::IOComponentType componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  if( retin ) // if an input file was specified
  {
    itktools::GetImageDimension( inputFileName, dim );
    componentType = itktools::GetImageComponentType( inputFileName );
  }

  /** Let the user overrule. */
  if( retopct )
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  }

  /** Class that does the work. */
  ITKToolsCreateSimpleBoxBase * filter = nullptr;

  try
  {
    // now call all possible template combinations.
    if( !filter) filter = ITKToolsCreateSimpleBox< 2, short >::New( dim, componentType );
    if( !filter) filter = ITKToolsCreateSimpleBox< 2, char >::New( dim, componentType );
    if( !filter) filter = ITKToolsCreateSimpleBox< 2, unsigned char >::New( dim, componentType );

#ifdef ITKTOOLS_3D_SUPPORT
    if( !filter) filter = ITKToolsCreateSimpleBox< 3, short >::New( dim, componentType );
    if( !filter) filter = ITKToolsCreateSimpleBox< 3, char >::New( dim, componentType );
    if( !filter) filter = ITKToolsCreateSimpleBox< 3, unsigned char >::New( dim, componentType );
#endif
    /** Check if filter was instantiated. */
    bool supported = itktools::IsFilterSupportedCheck( filter, dim, componentType );
    if( !supported ) return EXIT_FAILURE;

    /** Set the filter arguments. */
    filter->m_InputFileName = inputFileName;
    filter->m_OutputFileName = outputFileName;
    filter->m_BoxSize = boxSize;
    filter->m_IndexA = indexA;
    filter->m_IndexB = indexB;

    filter->Run();

    delete filter;
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    delete filter;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

} // end function main
