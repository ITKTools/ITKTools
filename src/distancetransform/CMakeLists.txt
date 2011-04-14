# This project is intended to be built outside the Insight source tree
PROJECT( distancetransform )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

INCLUDE_DIRECTORIES( ../morphology )

ADD_EXECUTABLE( pxdistancetransform
  distancetransform.cxx
)

TARGET_LINK_LIBRARIES( pxdistancetransform
  pxcommon ITKBasicFilters ${ITK_LIBRARIES})

INSTALL( TARGETS pxdistancetransform
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

