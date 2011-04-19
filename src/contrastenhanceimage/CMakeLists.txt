# This project is intended to be built outside the Insight source tree
PROJECT( contrastenhanceimage )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcontrastenhanceimage contrastenhanceimage.cxx contrastenhanceimage.h )

TARGET_LINK_LIBRARIES( pxcontrastenhanceimage ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcontrastenhanceimage
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

