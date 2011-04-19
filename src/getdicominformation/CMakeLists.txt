# This project is intended to be built outside the Insight source tree
PROJECT( getdicominformation )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxgetdicominformation getdicominformation.cxx )

TARGET_LINK_LIBRARIES( pxgetdicominformation pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxgetdicominformation
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

