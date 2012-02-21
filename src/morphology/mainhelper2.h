#include <iostream>
#include <vector>
#include <string>

#include "erosion.h"
#include "dilation.h"
#include "opening.h"
#include "closing.h"
#include "gradient.h"


/** run: A macro to call a function. */
#define run( function, ctype, dim ) \
if( operation == #function ) \
{ \
  if( componentType == #ctype && Dimension == dim ) \
  { \
    typedef itk::Image< ctype, dim > ImageType; \
    if( type == "grayscale" ) \
    { \
      function##Grayscale< ImageType >( inputFileName, outputFileName, radius, boundaryCondition, useCompression ); \
      supported = true; \
    } \
    else if( type == "binary" ) \
    { \
      function##Binary< ImageType >( inputFileName, outputFileName, radius, bin, useCompression ); \
      supported = true; \
    } \
    else if( type == "parabolic" ) \
    { \
      function##Parabolic< ImageType >( inputFileName, outputFileName, radius, useCompression ); \
      supported = true; \
    } \
  } \
}

/** run2: A macro to call a function. */
#define run2( function, ctype, dim ) \
if( operation == #function ) \
{ \
  if( componentType == #ctype && Dimension == dim ) \
  { \
    typedef itk::Image< ctype, dim > ImageType; \
    function< ImageType >( inputFileName, outputFileName, radius, algorithm, useCompression ); \
    supported = true; \
  } \
}
