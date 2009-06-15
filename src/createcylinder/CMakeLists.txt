# This project is intended to be built outside the Insight source tree
PROJECT(createcylinder)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcreatecylinder createcylinder.cxx itkCylinderSpatialFunction.h itkCylinderSpatialFunction.txx)

INSTALL_TARGETS(/. pxcreatecylinder)

TARGET_LINK_LIBRARIES(pxcreatecylinder pxcommon ITKBasicFilters ITKIO ITKCommon)
