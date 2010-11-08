# This project is intended to be built outside the Insight source tree
PROJECT(resizeimage)

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxresizeimage resizeimage.cxx)

TARGET_LINK_LIBRARIES(pxresizeimage pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxresizeimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

