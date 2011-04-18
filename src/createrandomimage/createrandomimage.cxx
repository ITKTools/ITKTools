#ifndef __createrandomimage_cxx
#define __createrandomimage_cxx

#include "createrandomimage.h"


typedef std::map<std::string, std::string> ArgMapType;
ArgMapType argmap;



void PrintUsageString(void)
{
  std::cerr
    << "\nThis program creates a random image.\n\n"
    << "Usage:\n"
    << "pxcreaterandomimage\n"
    << "\t-out   \tOutputImageFileName\n"
    << "\t-pt    \tPixelType <SHORT, USHORT, INT, UINT, CHAR, UCHAR, FLOAT>\n"
    << "\t-id    \tImageDimension <2,3>\n"
    << "\t[-sd]  \tSpaceDimension (the number of channels) <1,2,3>\n"
    << "\t-d0    \tSize of dimension 0\n"
    << "\t-d1    \tSize of dimension 1\n"
    << "\t[-d2]  \tSize of dimension 2\n"
    //<< "\t[-d3]  \tSize of dimension 3\n"
    //<< "\t[-d4]  \tSize of dimension 4\n"
    << "\t[-r]   \tThe resolution of the random image <unsigned long>.\n"
    << "\t\t\tThis determines the number of voxels set to a random value before blurring.\n"
    << "\t\t\tIf set to 0, all voxels are set to a random value\n"
    << "\t[-sigma]\tThe standard deviation of the blurring filter\n"
    << "\t[-min] \tMinimum pixel value\n"
    << "\t[-max] \tMaximum pixel value\n"
    << "\t[-seed]\tThe random seed <int>\n"
    << std::endl;
} // end PrintUsageString



int ReadArgument(const std::string & key, std::string & value, bool optional)
{

  if ( argmap.count(key) )
  {
    value = argmap[key];
    return 0;
  }
  else
  {
    if (!optional)
    {
      std::cerr << "Not enough arguments\n";
      std::cerr << "Missing argument: " << key << std::endl;
      PrintUsageString();
      return 1;
    }
    else
    {
      return 0;
    }
  }

} // end ReadArgument


/**
 * ********************* main ***********************************
 */

int main(int argc, char** argv)
{

  std::string outputImageFileName("");
  std::string pixelType("");
  std::string imageDimension("0");
  std::string spaceDimension("1");
  std::string dimsize("0");
  std::string resolution("0");
  std::string sigma("-1");
  std::string minimum_value("0");
  std::string maximum_value("255");
  std::string randomseed("1");

  std::ostringstream makeString("");
  itk::Array<unsigned int> sizes;
  unsigned int iDim = 0;

  unsigned long res = 0;
  double sig = -1.0;
  double min_value = 0.0;
  double max_value = 0.0;
  int rand_seed = 0;

  unsigned long nrOfPixels = 1;

  /** Fill the argument map */
  for (unsigned int i = 1; i<static_cast<unsigned int>(argc); i+=2)
  {
    if ( (i+1) < argc)
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

  int returndummy = 0;
  returndummy |= ReadArgument("-out", outputImageFileName, 0);
  returndummy |= ReadArgument("-pt", pixelType, 0);
  returndummy |= ReadArgument("-id", imageDimension, 0);
  returndummy |= ReadArgument("-sd", spaceDimension, 1);
  returndummy |= ReadArgument("-r", resolution, 1);
  returndummy |= ReadArgument("-sigma", sigma, 1);
  returndummy |= ReadArgument("-min", minimum_value, 1);
  returndummy |= ReadArgument("-max", maximum_value, 1);
  returndummy |= ReadArgument("-seed", randomseed, 1);

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
  sizes.SetSize(iDim);
  for (unsigned int i=0; i< iDim ; i++)
  {
    makeString << "-d" << i;
    returndummy |= ReadArgument(makeString.str(), dimsize, 0);
    if (returndummy ==0)
    {
      sizes[i] = atoi( dimsize.c_str() );
      nrOfPixels *= sizes[i];
    }
    makeString.str("");
  }

  if ( returndummy !=0 )
  {
    return returndummy;
  }


  if ( argmap.count("-r") )
  {
    res = static_cast< unsigned long >(  atof( resolution.c_str() )  );
  }
  else
  {
    res = static_cast< unsigned long >(nrOfPixels/64 );
  }
  min_value = atof( minimum_value.c_str() );
  max_value = atof( maximum_value.c_str() );
  rand_seed = atoi( randomseed.c_str() );
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
         res, sig,
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
         res, sig,
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
         res, sig,
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
         res, sig,
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
         res, sig,
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
         res, sig,
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

  if (returndummy)
  {
    std::cerr << "Errors occured." << std::endl;
  }

  return returndummy;

} // end function main

#endif // #ifndef __createrandomimage_cxx

