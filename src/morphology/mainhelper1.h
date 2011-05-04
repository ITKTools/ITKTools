#include <iostream>
#include <vector>
#include <string>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Usage:" << std::endl
     << "pxmorphology" << std::endl
     << "  -in      inputFilename" << std::endl
     << "  -op      operation, choose one of {erosion, dilation, opening, closing, gradient}" << std::endl
     << "  [-type]  type, choose one of {grayscale, binary, parabolic}, default grayscale" << std::endl
     << "  [-out]   outputFilename, default in_operation_type.extension" << std::endl
     << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
     << "  -r       radius" << std::endl
     << "  [-bc]    boundaryCondition (grayscale): the gray value outside the image" << std::endl
     << "  [-bin]   foreground and background values" << std::endl
     << "  [-a]     algorithm type for op=gradient" << std::endl
     << "           BASIC = 0, HISTO = 1, ANCHOR = 2, VHGW = 3, default 0" << std::endl
     << "           BASIC and HISTO have radius dependent performance, ANCHOR and VHGW not" << std::endl
     << "  [-opct]  pixelType, default: automatically determined from input image" << std::endl
     << "For grayscale filters, supply the boundary condition." << std::endl
     << "  This value defaults to the maximum pixel value." << std::endl
     << "For binary filters, supply the foreground and background value." << std::endl
     << "  The foreground value refers to the value of the object of interest (default 1)," << std::endl
     << "  the background value is by default 0," << std::endl
     << "  It is not only intended for binary images, but also for grayscale images." << std::endl
     << "  In this case the foreground value selects which value to do the operation on." << std::endl
     << "Examples:" << std::endl
     << "  1) Dilate a binary image (1 = foreground, 0 = background)" << std::endl
     << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1" << std::endl
     << "  2) Dilate a binary image (255 = foreground, 0 = background)" << std::endl
     << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1 -bin 255 0" << std::endl
     << "Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;
     
  return ss.str();

} // end GetHelpString()
