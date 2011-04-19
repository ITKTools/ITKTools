# This project is intended to be built outside the Insight source tree
PROJECT( detectgoldmarkers )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxdetectgoldmarkers detectgoldmarkers.cxx )

TARGET_LINK_LIBRARIES( pxdetectgoldmarkers ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxdetectgoldmarkers
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

