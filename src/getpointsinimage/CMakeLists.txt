# This project is intended to be built outside the Insight source tree
PROJECT(GetPointsInImage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxgetpointsinimage getpointsinimage.cxx)

TARGET_LINK_LIBRARIES(pxgetpointsinimage ITKIO ITKCommon)

INSTALL( TARGETS pxgetpointsinimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

