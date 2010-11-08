# This project is intended to be built outside the Insight source tree
PROJECT(creategridimage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcreategridimage creategridimage.cxx)

TARGET_LINK_LIBRARIES(pxcreategridimage pxcommon ITKIO ITKCommon)

INSTALL( TARGETS pxcreategridimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

