# This project is intended to be built outside the Insight source tree
PROJECT(invertintensityimagefilter)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxinvertintensityimagefilter invertintensityimagefilter.cxx)

TARGET_LINK_LIBRARIES(pxinvertintensityimagefilter pxcommon ITKIO ITKBasicFilters ITKCommon)

INSTALL( TARGETS pxinvertintensityimagefilter
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

