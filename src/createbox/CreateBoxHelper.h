#ifndef __CreateBoxHelper_h
#define __CreateBoxHelper_h

#include <iostream>
#include "CommandLineArgumentHelper.h"

#include "itkBoxSpatialFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "vnl/vnl_math.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl 
    << "pxcreatebox" << std::endl
    << "  -out     outputFilename" << std::endl
    << "  Arguments to specify the output image:" << std::endl
    << "  [-in]    inputFilename, to copy image information from" << std::endl
    << "  [-sz]    image size (voxels)" << std::endl
    << "  [-sp]    image spacing (mm), default 1.0" << std::endl
    << "  [-io]    image origin, default 0.0" << std::endl
    << "  [-d]     image direction, default identity" << std::endl
    << "  [-dim]   dimension, default 3" << std::endl
    << "  [-pt]    pixelType, default short" << std::endl
    << "  Arguments to specify the box:" << std::endl
    << "  [-c]     center (mm)" << std::endl
    << "  [-r]     radii (mm)" << std::endl
    << "  [-cp1]   cornerpoint 1 (mm)" << std::endl
    << "  [-cp2]   cornerpoint 2 (mm)" << std::endl
    << "  [-ci1]   cornerindex 1" << std::endl
    << "  [-ci2]   cornerindex 2" << std::endl
    << "  [-o]     orientation of the box, default xyz" << std::endl
    << "- The user should EITHER specify the input filename OR the output image size." << std::endl
    << "- The user should EITHER specify the center and the radius," << std::endl
    << "    OR the positions of two opposite corner points." << std::endl
    << "    OR the positions of two opposite corner indices." << std::endl
    << "- The orientation is a vector with Euler angles (rad)." << std::endl
    << "- Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

  return ss.str();

} // end GetHelpString()


#endif // end #ifndef __CreateBoxHelper_h
