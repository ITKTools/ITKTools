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

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetHelpString());

  parser->MarkArgumentAsRequired( "-pt", "Pixel type." );
  parser->MarkArgumentAsRequired( "-in", "Input filename." );

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

  /** Instantiate the pixeltypeselector */
  if (imageDimension == 2)
  {
    ptswrap<2>::PixelTypeSelector( parser );
  }
  else if (imageDimension == 3)
  {
     ptswrap<3>::PixelTypeSelector( parser );
  }
  else
  {
    std::cerr << "ERROR: This imageDimension is not supported" << std::endl;
    return 1;
  }

  return 0;

} // end function main

#endif // #ifndef __createbox_cxx
