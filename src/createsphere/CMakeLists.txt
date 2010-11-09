# This project is intended to be built outside the Insight source tree
PROJECT(CreateSphere)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcreatesphere createsphere.cxx)

TARGET_LINK_LIBRARIES(pxcreatesphere pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxcreatesphere
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

