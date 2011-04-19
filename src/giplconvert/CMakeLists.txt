# This project is intended to be built outside the Insight source tree
PROJECT( giplconvert )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxgiplconvert giplconvert.cxx )

TARGET_LINK_LIBRARIES( pxgiplconvert ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxgiplconvert
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

