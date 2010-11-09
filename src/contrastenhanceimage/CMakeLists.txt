# This project is intended to be built outside the Insight source tree
PROJECT(contrastenhanceimage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcontrastenhanceimage contrastenhanceimage.cxx contrastenhanceimage.h)

TARGET_LINK_LIBRARIES(pxcontrastenhanceimage ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxcontrastenhanceimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

