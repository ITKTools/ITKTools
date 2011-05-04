#include <iostream>
#include <vector>
#include <string>


/**
 * ******************* GetHelpString *******************
 */

void GetHelpString( void )
{
  std::cout << "Usage:" << std::endl << "pxmorphology\n";
  std::cout << "  -in      inputFilename\n";
  std::cout << "  -op      operation, choose one of {erosion, dilation, opening, closing, gradient}\n";
  std::cout << "  [-type]  type, choose one of {grayscale, binary, parabolic}, default grayscale\n";
  std::cout << "  [-out]   outputFilename, default in_operation_type.extension\n";
	std::cout << "  [-z]     compression flag; if provided, the output image is compressed\n";
  std::cout << "  -r       radius\n";
  std::cout << "  [-bc]    boundaryCondition (grayscale): the gray value outside the image\n";
  std::cout << "  [-bin]   foreground and background values\n";
  std::cout << "  [-a]     algorithm type for op=gradient\n";
  std::cout << "           BASIC = 0, HISTO = 1, ANCHOR = 2, VHGW = 3, default 0\n";
  std::cout << "           BASIC and HISTO have radius dependent performance, ANCHOR and VHGW not\n";
  std::cout << "  [-opct]  pixelType, default: automatically determined from input image\n";
  std::cout << "For grayscale filters, supply the boundary condition.\n";
  std::cout << "  This value defaults to the maximum pixel value.\n";
  std::cout << "For binary filters, supply the foreground and background value.\n";
  std::cout << "  The foreground value refers to the value of the object of interest (default 1),\n";
  std::cout << "  the background value is by default 0,\n";
  std::cout << "  It is not only intended for binary images, but also for grayscale images.\n";
  std::cout << "  In this case the foreground value selects which value to do the operation on.\n";
  std::cout << "Examples:\n";
  std::cout << "  1) Dilate a binary image (1 = foreground, 0 = background)\n";
  std::cout << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1\n";
  std::cout << "  2) Dilate a binary image (255 = foreground, 0 = background)\n";
  std::cout << "    pxmorphology -in input.mhd -op dilation -type binary -out output.mhd -r 1 -bin 255 0\n";
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

} // end GetHelpString()
