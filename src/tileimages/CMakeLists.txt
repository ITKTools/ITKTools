# This project is intended to be built outside the Insight source tree
PROJECT( tileimages )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )


# could not convert template argument error

ADD_EXECUTABLE( pxtileimages tileimages.cxx )

TARGET_LINK_LIBRARIES( pxtileimages ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxtileimages
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

