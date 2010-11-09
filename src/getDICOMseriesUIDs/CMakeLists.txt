# This project is intended to be built outside the Insight source tree
PROJECT(getDICOMseriesUIDs)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxgetDICOMseriesUIDs getDICOMseriesUIDs.cxx)

TARGET_LINK_LIBRARIES(pxgetDICOMseriesUIDs pxcommon ITKIO ITKCommon)

INSTALL( TARGETS pxgetDICOMseriesUIDs
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

