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
  parser->SetProgramHelpText(PrintUsageString());

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-id", "The image dimension." );
  parser->MarkArgumentAsRequired( "-pt", "The pixel type." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
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

