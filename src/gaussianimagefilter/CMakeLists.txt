# This project is intended to be built outside the Insight source tree
PROJECT(gaussianimagefilter)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxgaussianimagefilter
 gaussianimagefilter.cxx
 itkSmoothingRecursiveGaussianImageFilter2.h
 itkSmoothingRecursiveGaussianImageFilter2.txx
 itkHessianRecursiveGaussianImageFilter2.h
 itkHessianRecursiveGaussianImageFilter2.txx
)

TARGET_LINK_LIBRARIES(pxgaussianimagefilter pxcommon ITKIO ITKBasicFilters ITKCommon)

INSTALL_TARGETS(/. pxgaussianimagefilter)

