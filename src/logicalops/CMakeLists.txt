# This project is intended to be built outside the Insight source tree
PROJECT(logicalops)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxlogicalops logicalops.cxx itkLogicalFunctors.h)

INSTALL_TARGETS(/. pxlogicalops)

TARGET_LINK_LIBRARIES(pxlogicalops pxcommon ITKBasicFilters ITKIO ITKCommon)

