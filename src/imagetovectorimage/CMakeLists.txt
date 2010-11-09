# This project is intended to be built outside the Insight source tree
PROJECT(imagetovectorimage)

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pximagetovectorimage imagetovectorimage.cxx)

TARGET_LINK_LIBRARIES(pximagetovectorimage pxcommon ITKIO ITKCommon)

INSTALL( TARGETS pximagetovectorimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

