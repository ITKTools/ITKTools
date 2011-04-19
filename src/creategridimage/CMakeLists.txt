# This project is intended to be built outside the Insight source tree
PROJECT( creategridimage )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcreategridimage creategridimage.cxx )

TARGET_LINK_LIBRARIES( pxcreategridimage pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcreategridimage
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

