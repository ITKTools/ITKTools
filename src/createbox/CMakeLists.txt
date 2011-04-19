# This project is intended to be built outside the Insight source tree
PROJECT( createbox )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcreatebox createbox.cxx itkBoxSpatialFunction.txx itkBoxSpatialFunction.h )

TARGET_LINK_LIBRARIES( pxcreatebox ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcreatebox
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

