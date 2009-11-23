#include <iostream>
#include <vector>
#include <string>


/**
 * ******************* PrintHelp *******************
 */

void PrintHelp( void )
{
  std::cout << "Usage:" << std::endl << "pxmorphology\n";
  std::cout << "  -in      inputFilename\n";
  std::cout << "  -op      operation, choose one of {erosion, dilation, opening, closing}\n";
  std::cout << "  -type    type, choose one of {grayscale, binary, parabolic}\n";
  std::cout << "  [-out]   outputFilename, default in_operation_type.extension\n";
  std::cout << "  -r       radius\n";
  std::cout << "  [-bc]    boundaryCondition (grayscale): the gray value outside the image\n";
  std::cout << "  [-bin]   foreground, background, erosion values\n";
  std::cout << "  [-opct]  pixelType, default: automatically determined from input image\n";
  std::cout << "For grayscale filters, supply the boundary condition.\n";
  std::cout << "  This value defaults to the maximum pixel value.\n";
  std::cout << "For binary filters, supply the foreground, background, and erode value.\n";
  std::cout << "  The foreground value refers to the object (default 1),\n";
  std::cout << "  the background value is by default 0,\n";
  std::cout << "  the erode value refers to the object (default 1).\n";
  std::cout << "  It is not only intended for binary images, but also for grayscale images.\n";
  std::cout << "  In this case the erode value selects which value to do the operation on.\n";
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

} // end PrintHelp()

