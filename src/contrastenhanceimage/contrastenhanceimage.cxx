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
 \brief Enhance the contrast of an image.
 
 \verbinclude contrastenhanceimage.help
 */
#ifndef __contrastenhanceimage_cxx
#define __contrastenhanceimage_cxx

#include "contrastenhanceimage.h"

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-id", "The image dimension." );
  parser->MarkArgumentAsRequired( "-pt", "The pixel type." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  std::string imageDimension("0");
  unsigned int iDim = 0;

  /** Read the dimension. */
  parser->GetCommandLineArgument( "-id", imageDimension);

  iDim = atoi( imageDimension.c_str() );
  if (iDim ==0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }

  /** Instantiate the pixeltypeselector */
  bool returndummy;
  if (imageDimension == "2")
  {
    returndummy = ptswrap<2>::PixelTypeSelector(parser);
  }
  else if (imageDimension == "3")
  {
     returndummy = ptswrap<3>::PixelTypeSelector(parser);
  }
  else
  {
    std::cerr << "ERROR: This imageDimension is not supported" << std::endl;
    return 1;
  }

  if (returndummy)
  {
    std::cerr << "Errors occured." << std::endl;
  }

  return returndummy;

} // end function main

#endif // #ifndef __contrastenhanceimage_cxx

