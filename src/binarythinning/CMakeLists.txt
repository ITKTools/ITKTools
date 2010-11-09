# This project is intended to be built outside the Insight source tree
PROJECT(binarythinning)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxbinarythinning binarythinning.cxx)

TARGET_LINK_LIBRARIES(pxbinarythinning pxcommon ITKIO ITKBasicFilters ITKCommon)

INSTALL( TARGETS pxbinarythinning
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

