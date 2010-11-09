# This project is intended to be built outside the Insight source tree
PROJECT(rescaleintensityimagefilter)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxrescaleintensityimagefilter rescaleintensityimagefilter.cxx)

TARGET_LINK_LIBRARIES(pxrescaleintensityimagefilter pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxrescaleintensityimagefilter
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

