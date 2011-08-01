# This project is intended to be built outside the Insight source tree
PROJECT( binarythinning )

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxbinarythinning binarythinning.cxx )

TARGET_LINK_LIBRARIES( pxbinarythinning ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxbinarythinning
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

