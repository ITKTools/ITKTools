# This project is intended to be built outside the Insight source tree
PROJECT(reflect)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxreflect reflect.cxx )

TARGET_LINK_LIBRARIES(pxreflect pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxreflect
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

