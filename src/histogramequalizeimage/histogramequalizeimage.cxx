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
 \brief Equalize the histogram of an image.
 
 \verbinclude histogramequalizeimage.help
 */
#ifndef __histogramequalizeimage_cxx
#define __histogramequalizeimage_cxx

#include "histogramequalizeimage.h"

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

std::string GetHelpString(void)
{
  std::stringstream ss;
  ss<< "This program applies histogram equalization to an image." << std::endl
    << "Works as described by Maintz, Introduction to Image Processing." << std::endl
    << "Usage:" << std::endl
    << "pxhistogramequalizeimage" << std::endl
    << "-in    \tInputImageFileName" << std::endl
    << "-out   \tOutputImageFileName" << std::endl
    << "-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>" << std::endl
    << "Currently only char, uchar, short, and ushort are supported." << std::endl
    << "-id    \tImageDimension <2,3>" << std::endl;

  return ss.str();
} // end GetHelpString


int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-out", "The output filename." );
  parser->MarkArgumentAsRequired( "-pt", "Pixel type." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Read the dimension. */
  unsigned int imageDimension = 0;
  parser->GetCommandLineArgument( "-id", imageDimension );

  if (imageDimension == 0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }

  /** Instantiate the pixeltypeselector */
  bool success = false;
  if (imageDimension == 2)
  {
    success = ptswrap<2>::PixelTypeSelector( parser );
  }
  else if (imageDimension == 3)
  {
    success = ptswrap<3>::PixelTypeSelector( parser );
  }
  else
  {
    std::cerr << "ERROR: This imageDimension is not supported" << std::endl;
    return 1;
  }

  return success;

} // end function main

#endif // #ifndef __histogramequalizeimage_cxx

