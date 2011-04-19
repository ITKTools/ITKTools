# This project is intended to be built outside the Insight source tree
PROJECT( ttest )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxttest ttest.cxx )

TARGET_LINK_LIBRARIES( pxttest pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxttest
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

