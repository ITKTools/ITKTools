#ifndef __LogicalImageOperatorHelper_h
#define __LogicalImageOperatorHelper_h

#include "itkBinaryFunctorImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorImage.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

//#include "itkLogicalFunctors.h"
#include "itkUnaryLogicalFunctors.h"
#include "itkBinaryLogicalFunctors.h"

#include <map>
#include <utility>
#include <vector>

#include <itksys/SystemTools.hxx>


//-------------------------------------------------------------------------------------
/**
 * ******************* PrintHelp *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "Logical operations on one or two images." << std::endl
     << "NOTE: The output of this filter is an image with pixels of values 0 and 1." << std::endl
  << "An appropriate scaling must be performed either manually (with pxrescaleintensityimagefilter)" << std::endl
  << "or with the application used to view the image." << std::endl << std::endl
  << "In the case of a vector image, this is a componentwise logical operator." << std::endl
  << "Usage:" << std::endl << "pxlogicalimageoperator" << std::endl
  << "  -in      inputFilename1 [inputFilename2]" << std::endl
  << "  [-out]   outputFilename, default in1 + <ops> + in2 + .mhd" << std::endl
  << "  -ops     LogicalOperator of the following form:" << std::endl
  << "             [!]( ([!] A) [{&,|,^} ([!] B])] )" << std::endl
  << "           notation:" << std::endl
  << "             [NOT_][NOT][{AND,OR,XOR}[NOT]]" << std::endl
  << "           notation examples:" << std::endl
  << "             ANDNOT = A & (!B)" << std::endl
  << "             NOTAND = (!A) & B" << std::endl
  << "             NOTANDNOT = (!A) & (!B)" << std::endl
  << "             NOT_NOTANDNOT = !( (!A) & (!B) )" << std::endl
  << "             NOT_AND = !(A & B)" << std::endl
  << "             OR = A | B" << std::endl
  << "             XOR = A ^ B" << std::endl
  << "             NOT = !A " << std::endl
  << "             NOT_NOT = A" << std::endl
  << "           Internally this expression is simplified." << std::endl
  << "  [-z]     compression flag; if provided, the output image is compressed" << std::endl
  << "  [-arg]   argument, necessary for some ops" << std::endl
  << "  [-dim]   dimension, default: automatically determined from inputimage1" << std::endl
  << "  [-pt]    pixelType, default: automatically determined from inputimage1" << std::endl
  << "Supported: 2D, 3D, (unsigned) short, (unsigned) char." << std::endl;

  return ss.str();
} // end GetHelpString()


#endif // end __LogicalImageOperatorHelper_h
