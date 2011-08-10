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
#ifndef __createbox_cxx
#define __createbox_cxx

#include "createsimplebox.h"

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

std::string GetHelpString(void)
{
  std::stringstream ss;
  ss << "This program creates an image containing a white box, defined by point A and B." << std::endl
    << "Usage:" << std::endl
    << "pxcreatesimplebox" << std::endl
    << "[-in]  InputImageFileName" << std::endl
    << "Size, origin, and spacing for the output image will be taken" << std::endl
    << "from this image. NB: not the dimension and the pixeltype;" << std::endl
    << "you must set them anyway!" << std::endl
    << "-out   OutputImageFileName" << std::endl
    << "-pt    PixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>" << std::endl
    << "Currently only char, uchar and short are supported." << std::endl
    << "-id    ImageDimension <2,3>" << std::endl
    << "[-d0]  Size of dimension 0" << std::endl
    << "[-d1]  Size of dimension 1" << std::endl
    << "[-d2]  Size of dimension 2" << std::endl
    << "-pA0  Index 0 of pointA" << std::endl
    << "-pA1  Index 1 of pointA" << std::endl
    << "[-pA2]Index 2 of pointA" << std::endl
    << "-pB0  Index 0 of pointB" << std::endl
    << "-pB1  Index 1 of pointB" << std::endl
    << "[-pB2]Index 2 of pointB";
  return ss.str();
} // end GetHelpString

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-out", "Output filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  unsigned int imageDimension = 0;

  /** Read the dimension. */
  parser->GetCommandLineArgument("-id", imageDimension);

  if (imageDimension == 0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }
  
  std::string pixelTypeString("");
  bool retpt = parser->GetCommandLineArgument( "-pt", pixelTypeString );
  
  std::string outputFileName;
  parser->GetCommandLineArgument("-out", outputFileName);
  
  std::string inputFileName("");
  bool retin = parser->GetCommandLineArgument("-in", inputFileName);

  /** read point A and B from the commandline.*/
  std::vector<unsigned int> indexA;
  parser->GetCommandLineArgument("-pA", indexA);
  
  std::vector<unsigned int> indexB;
  parser->GetCommandLineArgument("-pB", indexB);
  
  std::vector<unsigned int> boxSize;
  parser->GetCommandLineArgument("-d", boxSize);
  
  /** Class that does the work */
  CreateSimpleBoxBase * createSimpleBox = NULL;

  unsigned int dim = 0;
  itktools::ComponentType componentType = itk::ImageIOBase::UCHAR; // to prevent uninitialized variable warning
  if( retin ) // if an input file was specified
  {
    itktools::GetImageDimension( inputFileName, dim );
    componentType = itktools::GetImageComponentType(inputFileName);
  }

  if( retpt ) // if a pixel type was specified on the command line
  {
    componentType = itk::ImageIOBase::GetComponentTypeFromString( pixelTypeString );
  }

  try
  {    
    // now call all possible template combinations.
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< short, 2 >::New( componentType, dim );
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< char, 2 >::New( componentType, dim );
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< unsigned char, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< short, 3 >::New( componentType, dim );    
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< char, 3 >::New( componentType, dim );
    if (!createSimpleBox) createSimpleBox = CreateSimpleBox< unsigned char, 3 >::New( componentType, dim );
#endif
    if (!createSimpleBox) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << dim
        << std::endl;
      return 1;
    }

    createSimpleBox->m_InputFileName = inputFileName;
    createSimpleBox->m_OutputFileName = outputFileName;
    createSimpleBox->m_BoxSize = boxSize;
    createSimpleBox->m_IndexA = indexA;
    createSimpleBox->m_IndexB = indexB;

    createSimpleBox->Run();
    
    delete createSimpleBox;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createSimpleBox;
    return 1;
  }

  return 0;

} // end function main

#endif // #ifndef __createbox_cxx
