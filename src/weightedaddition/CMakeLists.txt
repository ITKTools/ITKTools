# This project is intended to be built outside the Insight source tree
PROJECT(weightedaddition)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxweightedaddition weightedaddition.cxx)

TARGET_LINK_LIBRARIES(pxweightedaddition pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxweightedaddition
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

