

  /**
   * ******************* PrintHelp *******************
   */

void PrintHelp( void )
{
  std::cout << "Usage:" << std::endl << "pxmorphology" << std::endl;
  std::cout << "  -in      inputFilename" << std::endl;
  std::cout << "  -op      operation, choose one of {erosion, dilation, opening, closing}" << std::endl;
  std::cout << "  -type    type, choose one of {grayscale, binary}" << std::endl;
  std::cout << "  [-out]   outputFilename, default in_operation_type.extension" << std::endl;
  std::cout << "  -r       radius" << std::endl;
  std::cout << "  [-bc]    boundaryCondition (grayscale): the gray value outside the image" << std::endl;
  std::cout << "  [-bin]   foreground, background, erosion values." << std::endl;
  std::cout << "  [-opct]  pixelType, default: automatically determined from input image" << std::endl;
  std::cout << "For grayscale filters, supply the boundary condition.\n";
  std::cout << "  This value defaults to the maximum pixel value." << std::endl;
  std::cout << "For binary filters, supply the foreground, background, and erode value.\n";
  std::cout << "  The foreground value refers to the object (default 1),\n";
  std::cout << "  the background value is by default 0,\n";
  std::cout << "  the erode value refers to the object (default 1)." << std::endl;
  std::cout << "  It is not only intended for binary images, but also for grayscale images.\n";
  std::cout << "  In this case the erode value selects which value to do the operation on." << std::endl;
  std::cout << "Supported: 2D, 3D, (unsigned) char, (unsigned) short." << std::endl;

} // end PrintHelp()

//-------------------------------------------------------------------------------------

/** run: A macro to call a function. *
#define run(function,type,dim) \
if ( ComponentType == #type && Dimension == dim ) \
{ \
  typedef itk::Image< type, dim > InputImageType; \
  function< InputImageType >( inputFileName, outputFileName, Radius, boundaryCondition ); \
  supported = true; \
}

/** run: A macro to call a function. */
#define run( function, ctype, dim ) \
if ( operation == #function ) \
{ \
  if ( ComponentType == #ctype && Dimension == dim ) \
  { \
    typedef itk::Image< ctype, dim > ImageType; \
    if ( type == "grayscale" ) \
    { \
      function##Grayscale< ImageType >( inputFileName, outputFileName, Radius, boundaryCondition ); \
      supported = true; \
    } \
    else if ( type == "binary" ) \
    { \
      function##Binary< ImageType >( inputFileName, outputFileName, Radius, bin ); \
      supported = true; \
    } \
  } \
}