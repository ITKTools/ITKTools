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
  parser->SetProgramHelpText(PrintUsageString());

  parser->MarkArgumentAsRequired( "-pt", "Pixel type." );
  parser->MarkArgumentAsRequired( "-in", "Input filename." );

  bool validateArguments = parser->CheckForRequiredArguments();

  if(!validateArguments)
  {
    return EXIT_FAILURE;
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

