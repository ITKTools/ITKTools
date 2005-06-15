# This project is intended to be built outside the Insight source tree
PROJECT(resize)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxresize resize.cxx)

INSTALL_TARGETS(/. pxresize)

TARGET_LINK_LIBRARIES(pxresize ITKBasicFilters ITKNumerics ITKIO ITKCommon)
