# This project is intended to be built outside the Insight source tree
PROJECT(createellipsoid)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcreateellipsoid createellipsoid.cxx)

TARGET_LINK_LIBRARIES(pxcreateellipsoid pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxcreateellipsoid
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

