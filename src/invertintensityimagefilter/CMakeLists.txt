# This project is intended to be built outside the Insight source tree
PROJECT( invertintensityimagefilter )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxinvertintensityimagefilter invertintensityimagefilter.cxx )

TARGET_LINK_LIBRARIES( pxinvertintensityimagefilter pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxinvertintensityimagefilter
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

