# This project is intended to be built outside the Insight source tree
PROJECT(createbox)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcreatebox createbox.cxx createbox.h itkBoxSpatialFunction.txx itkBoxSpatialFunction.h)

INSTALL_TARGETS(/. pxcreatebox)

TARGET_LINK_LIBRARIES(pxcreatebox ITKBasicFilters ITKIO ITKCommon)

