# This project is intended to be built outside the Insight source tree
PROJECT(ComputeOverlap)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcomputeoverlap computeoverlap.cxx)

TARGET_LINK_LIBRARIES(pxcomputeoverlap pxcommon ITKIO ITKCommon ITKBasicFilters )

INSTALL( TARGETS pxcomputeoverlap
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

