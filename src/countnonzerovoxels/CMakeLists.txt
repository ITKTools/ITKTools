# This project is intended to be built outside the Insight source tree
PROJECT( CountNonZeroVoxels )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcountnonzerovoxels countnonzerovoxels.cxx )

TARGET_LINK_LIBRARIES( pxcountnonzerovoxels ${ITK_LIBRARIES} ITKTools-Common )

INSTALL( TARGETS pxcountnonzerovoxels
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

