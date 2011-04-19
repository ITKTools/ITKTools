# This project is intended to be built outside the Insight source tree
PROJECT( computeboundingbox )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcomputeboundingbox computeboundingbox.cxx )

TARGET_LINK_LIBRARIES( pxcomputeboundingbox pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcomputeboundingbox
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

