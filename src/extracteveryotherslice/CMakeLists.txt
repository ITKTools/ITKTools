# This project is intended to be built outside the Insight source tree
PROJECT( extracteveryotherslice )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxextracteveryotherslice extracteveryotherslice.cxx )

TARGET_LINK_LIBRARIES( pxextracteveryotherslice pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxextracteveryotherslice
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

