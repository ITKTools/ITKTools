# This project is intended to be built outside the Insight source tree
PROJECT(morphology)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxmorphology
  morphology.cxx
  mainhelper.h
  Erosion.h
  Dilation.h
)

INSTALL_TARGETS(/. pxmorphology)

TARGET_LINK_LIBRARIES(pxmorphology pxcommon ITKBasicFilters ITKIO ITKCommon)

