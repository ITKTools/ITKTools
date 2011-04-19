# This project is intended to be built outside the Insight source tree
PROJECT( extractslice )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxextractslice extractslice.cxx )

TARGET_LINK_LIBRARIES( pxextractslice pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxextractslice
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

