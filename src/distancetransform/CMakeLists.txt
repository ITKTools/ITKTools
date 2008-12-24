# This project is intended to be built outside the Insight source tree
PROJECT(distancetransform)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxdistancetransform
 distancetransform.cxx
)

INSTALL_TARGETS(/. pxdistancetransform)

TARGET_LINK_LIBRARIES(pxdistancetransform pxcommon ITKBasicFilters ITKIO ITKCommon ITKNumerics)

