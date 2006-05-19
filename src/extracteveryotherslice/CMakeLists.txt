# This project is intended to be built outside the Insight source tree
PROJECT(extracteveryotherslice)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxextracteveryotherslice extracteveryotherslice.cxx)

INSTALL_TARGETS(/. pxextracteveryotherslice)

TARGET_LINK_LIBRARIES(pxextracteveryotherslice pxcommon ITKBasicFilters ITKIO ITKCommon)

