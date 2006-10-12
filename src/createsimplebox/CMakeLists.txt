# This project is intended to be built outside the Insight source tree
PROJECT(createsimplebox)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcreatesimplebox createsimplebox.cxx createsimplebox.h itkSimpleBoxSpatialFunction.txx itkSimpleBoxSpatialFunction.h)

INSTALL_TARGETS(/. pxcreatesimplebox)

TARGET_LINK_LIBRARIES(pxcreatesimplebox ITKBasicFilters ITKIO ITKCommon)

