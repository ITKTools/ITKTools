# This project is designed to be built outside the Insight source tree.
PROJECT(fftimage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

OPTION(USE_FFTIMAGE "Build pxfftimage only if ITK was built with FFTW (float AND double)" OFF)

IF( USE_FFTIMAGE )
  ADD_EXECUTABLE(pxfftimage fftimage.cxx )

  TARGET_LINK_LIBRARIES(pxfftimage pxcommon ITKCommon ITKBasicFilters ITKIO ITKAlgorithms)

  INSTALL( TARGETS pxfftimage
    RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )
ENDIF( USE_FFTIMAGE )

