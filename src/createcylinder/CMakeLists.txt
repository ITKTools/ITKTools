# This project is intended to be built outside the Insight source tree
PROJECT( createcylinder )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcreatecylinder createcylinder.cxx itkCylinderSpatialFunction.h itkCylinderSpatialFunction.txx )

TARGET_LINK_LIBRARIES( pxcreatecylinder pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcreatecylinder
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

