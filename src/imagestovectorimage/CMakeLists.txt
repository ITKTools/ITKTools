# This project is intended to be built outside the Insight source tree
PROJECT( imagestovectorimage )

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pximagestovectorimage imagestovectorimage.cxx )

TARGET_LINK_LIBRARIES( pximagestovectorimage ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pximagestovectorimage
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

