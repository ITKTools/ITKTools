# This project is intended to be built outside the Insight source tree
PROJECT(extracteveryotherslice)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxextracteveryotherslice extracteveryotherslice.cxx)

TARGET_LINK_LIBRARIES(pxextracteveryotherslice pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxextracteveryotherslice
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

