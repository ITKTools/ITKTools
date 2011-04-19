# This project is intended to be built outside the Insight source tree
PROJECT( getDICOMseriesUIDs )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxgetDICOMseriesUIDs getDICOMseriesUIDs.cxx )

TARGET_LINK_LIBRARIES( pxgetDICOMseriesUIDs pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxgetDICOMseriesUIDs
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

