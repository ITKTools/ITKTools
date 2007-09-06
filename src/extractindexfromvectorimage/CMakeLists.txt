# This project is intended to be built outside the Insight source tree
PROJECT(extractindexfromvectorimage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxextractindexfromvectorimage extractindexfromvectorimage.cxx)

TARGET_LINK_LIBRARIES(pxextractindexfromvectorimage pxcommon ITKIO ITKCommon)

INSTALL_TARGETS(/. pxextractindexfromvectorimage)

