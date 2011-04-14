#ifndef __histogramequalizeimage_cxx
#define __histogramequalizeimage_cxx

#include "histogramequalizeimage.h"


int main(int argc, char** argv)
{
  ArgMapType argmap;
  std::string imageDimension("0");
  unsigned int iDim = 0;

  /** Fill the argument map */
  for (unsigned int i = 1; i < static_cast<unsigned int>(argc); i+=2)
  {
    if ( (i+1) < static_cast<unsigned int>(argc))
    {
      argmap[ argv[i] ] = argv[i+1];
    }
    else
    {
      argmap[ argv[i] ] = "";
    }
  }

  /** Help needed? */
  if ( (argc == 1) || argmap.count("-h") || argmap.count("-help") || argmap.count("--help") )
  {
    PrintUsageString();
    return -1;
  }

  /** Read the dimension. */
  int returndummy = 0;
  returndummy |= ReadArgument(argmap, "-id", imageDimension, false);

  if ( returndummy !=0 )
  {
    return returndummy;
  }

  iDim = atoi( imageDimension.c_str() );
  if (iDim ==0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }

  /** Instantiate the pixeltypeselector */
  if (imageDimension == "2")
  {
    returndummy = ptswrap<2>::PixelTypeSelector( argmap );
  }
  else if (imageDimension == "3")
  {
     returndummy = ptswrap<3>::PixelTypeSelector( argmap );
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

#endif // #ifndef __histogramequalizeimage_cxx

