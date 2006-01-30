# This project is intended to be built outside the Insight source tree
PROJECT(CreatePointsInImage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcreatepointsinimage createpointsinimage.cxx)

INSTALL_TARGETS(/. pxcreatepointsinimage)

TARGET_LINK_LIBRARIES(pxcreatepointsinimage ITKIO ITKCommon)
