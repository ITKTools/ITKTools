# This project is intended to be built outside the Insight source tree
PROJECT( createzeroimage )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcreatezeroimage createzeroimage.cxx )

TARGET_LINK_LIBRARIES( pxcreatezeroimage pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcreatezeroimage
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

