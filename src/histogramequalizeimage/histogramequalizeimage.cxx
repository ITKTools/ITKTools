#ifndef __histogramequalizeimage_cxx
#define __histogramequalizeimage_cxx

#include "histogramequalizeimage.h"

#include "itkCommandLineArgumentParser.h"
#include "CommandLineArgumentHelper.h"

int main(int argc, char** argv)
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

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

