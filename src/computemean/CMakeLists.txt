# This project is intended to be built outside the Insight source tree
PROJECT( computemean )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcomputemean computemean.cxx )

TARGET_LINK_LIBRARIES( pxcomputemean pxcommon )

INSTALL( TARGETS pxcomputemean
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

