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
 \brief Create a random image.
 
 \verbinclude createrandomimage.help
 */
#ifndef __createrandomimage_cxx
#define __createrandomimage_cxx

#include "createrandomimage.h"

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

typedef std::map<std::string, std::string> ArgMapType;
ArgMapType argmap;

std::string GetUsageString(void);

/**
 * ********************* main ***********************************
 */

int main(int argc, char** argv)
{

  std::string outputImageFileName("");
  std::string pixelType("");
  std::string imageDimension("0");
  std::string spaceDimension("1");
  std::string sigma("-1");

  std::ostringstream makeString("");
  itk::Array<unsigned int> sizes;
  unsigned int iDim = 0;

  double sig = -1.0;
  double min_value = 0.0;
  double max_value = 0.0;
  int rand_seed = 0;

  unsigned long nrOfPixels = 1;


  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText(GetUsageString());
  
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  parser->GetCommandLineArgument("-out", outputImageFileName);
  parser->GetCommandLineArgument("-pt", pixelType);
  parser->GetCommandLineArgument("-id", iDim);
  parser->GetCommandLineArgument("-sd", spaceDimension);
  parser->GetCommandLineArgument("-sigma", sigma);
  parser->GetCommandLineArgument("-min", min_value);
  parser->GetCommandLineArgument("-max", max_value);
  parser->GetCommandLineArgument("-seed", rand_seed);


  if (iDim ==0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }
  sizes.SetSize(iDim);
  for (unsigned int i=0; i< iDim ; i++)
  {
    makeString.str("");
    makeString << "-d" << i;
    unsigned int dimsize = 0;
    bool retdimsize = parser->GetCommandLineArgument(makeString.str(), dimsize);
    if (!retdimsize)
    {
      sizes[i] = dimsize;
      nrOfPixels *= sizes[i];
    }
  }

  unsigned long resolution = nrOfPixels/64;
  parser->GetCommandLineArgument("-r", resolution);

  sig = atof( sigma.c_str() ); //-1 if not entered.


  if (imageDimension == "2")
  {
    if (spaceDimension == "1")
    {
       returndummy = ptswrap<2,1>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else if (spaceDimension == "2")
    {
       returndummy = ptswrap<2,2>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else if (spaceDimension == "3")
    {
       returndummy = ptswrap<2,3>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else
    {
      std::cerr << "ERROR: This spaceDimension is not supported" << std::endl;
      return 1;
    }
  }
  else if (imageDimension == "3")
  {
    if (spaceDimension == "1")
    {
       returndummy = ptswrap<3,1>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else if (spaceDimension == "2")
    {
       returndummy = ptswrap<3,2>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else if (spaceDimension == "3")
    {
       returndummy = ptswrap<3,3>::PixelTypeSelector(
         pixelType.c_str(),
         outputImageFileName.c_str(),
         sizes,
         min_value,
         max_value,
         resolution, sig,
         rand_seed );
    }
    else
    {
      std::cerr << "ERROR: This spaceDimension is not supported" << std::endl;
      return 1;
    }

  }
  else
  {
    std::cerr << "ERROR: This imageDimension is not supported" << std::endl;
    return 1;
  }

  return 0;

} // end function main


std::string GetUsageString(void)
{
  std::string helpString = "\nThis program creates a random image.\n\n \
    Usage:\n \
    pxcreaterandomimage\n \
    \t-out   \tOutputImageFileName\n \
    \t-pt    \tPixelType <SHORT, USHORT, INT, UINT, CHAR, UCHAR, FLOAT>\n \
    \t-id    \tImageDimension <2,3>\n \
    \t[-sd]  \tSpaceDimension (the number of channels) <1,2,3>\n \
    \t-d0    \tSize of dimension 0\n \
    \t-d1    \tSize of dimension 1\n \
    \t[-d2]  \tSize of dimension 2\n \
    \t[-r]   \tThe resolution of the random image <unsigned long>.\n \
    \t\t\tThis determines the number of voxels set to a random value before blurring.\n \
    \t\t\tIf set to 0, all voxels are set to a random value\n \
    \t[-sigma]\tThe standard deviation of the blurring filter\n \
    \t[-min] \tMinimum pixel value\n \
    \t[-max] \tMaximum pixel value\n \
    \t[-seed]\tThe random seed <int>\n";
  //<< "\t[-d3]  \tSize of dimension 3\n"
  //<< "\t[-d4]  \tSize of dimension 4\n"
  return helpString;
} // end PrintUsageString

#endif // #ifndef __createrandomimage_cxx
