# This project is intended to be built outside the Insight source tree
PROJECT( cropimage )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcropimage cropimage.cxx )

TARGET_LINK_LIBRARIES( pxcropimage pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcropimage
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

