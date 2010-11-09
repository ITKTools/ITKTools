# This project is intended to be built outside the Insight source tree
PROJECT(tileimages)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxtileimages tileimages.cxx)

TARGET_LINK_LIBRARIES(pxtileimages pxcommon ITKIO ITKCommon)

INSTALL( TARGETS pxtileimages
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

