# This project is intended to be built outside the Insight source tree
PROJECT( closestversor3Dtransform )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxclosestversor3Dtransform closestversor3Dtransform.cxx )

TARGET_LINK_LIBRARIES( pxclosestversor3Dtransform ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxclosestversor3Dtransform
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

