# This project is intended to be built outside the Insight source tree
PROJECT( getimageinformation )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxgetimageinformation getimageinformation.cxx )

TARGET_LINK_LIBRARIES( pxgetimageinformation ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxgetimageinformation
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

