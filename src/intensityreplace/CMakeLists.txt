# This project is intended to be built outside the Insight source tree
PROJECT(intensityreplace)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxintensityreplace
  intensityreplace.cxx
)

TARGET_LINK_LIBRARIES(pxintensityreplace pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxintensityreplace
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

