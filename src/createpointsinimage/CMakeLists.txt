# This project is intended to be built outside the Insight source tree
PROJECT(CreatePointsInImage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcreatepointsinimage createpointsinimage.cxx)

TARGET_LINK_LIBRARIES(pxcreatepointsinimage ${ITK_LIBRARIES})

INSTALL( TARGETS pxcreatepointsinimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

