# This project is intended to be built outside the Insight source tree
PROJECT(replacevoxel)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxreplacevoxel replacevoxel.cxx)

TARGET_LINK_LIBRARIES(pxreplacevoxel pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxreplacevoxel
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

