# This project is intended to be built outside the Insight source tree
PROJECT(intensitywindowing)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxintensitywindowing intensitywindowing.cxx)

TARGET_LINK_LIBRARIES(pxintensitywindowing pxcommon ${ITK_LIBRARIES})

INSTALL( TARGETS pxintensitywindowing
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

