# This project is intended to be built outside the Insight source tree
PROJECT( averagevectormagnitude )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxaveragevectormagnitude averagevectormagnitude.cxx )

TARGET_LINK_LIBRARIES( pxaveragevectormagnitude ${ITK_LIBRARIES} ITKTools-Common )

INSTALL( TARGETS pxaveragevectormagnitude
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )
