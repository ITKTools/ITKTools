# This project is intended to be built outside the Insight source tree
PROJECT(contrastenhanceimage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcontrastenhanceimage contrastenhanceimage.cxx contrastenhanceimage.h)

INSTALL_TARGETS(/. pxcontrastenhanceimage)

TARGET_LINK_LIBRARIES(pxcontrastenhanceimage ITKBasicFilters ITKIO ITKCommon)

