# This project is intended to be built outside the Insight source tree
PROJECT(averagevectormagnitude)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxaveragevectormagnitude averagevectormagnitude.cxx)

TARGET_LINK_LIBRARIES(pxaveragevectormagnitude
ITKBasicFilters
ITKNumerics
ITKIO
ITKCommon )

INSTALL( TARGETS pxaveragevectormagnitude
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )
