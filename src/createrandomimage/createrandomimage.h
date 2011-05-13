#ifndef __createrandomimage_h
#define __createrandomimage_h


#include "itkImageFileWriter.h"
#include "itkArray.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageRandomIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
//#include "itkDiscreteGaussianImageFilter.h"
//#include "itkRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkCompose3DVectorImageFilter.h"
#include "itkExceptionObject.h"
#include "itkNumericTraits.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include <iostream>
#include <string>
#include <math.h>
#include <map>


#endif // #ifndef __createrandomimage_h
