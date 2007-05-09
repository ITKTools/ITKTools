# This project is designed to be built outside the Insight source tree.
PROJECT(fftimage)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

OPTION(USE_FFTIMAGE "Build pxfftimage only if ITK was built with FFTW (float AND double)" OFF)

IF( USE_FFTIMAGE )
	ADD_EXECUTABLE(pxfftimage fftimage.cxx )

	INSTALL_TARGETS(/. pxfftimage)

	TARGET_LINK_LIBRARIES(pxfftimage pxcommon ITKCommon ITKBasicFilters ITKIO ITKAlgorithms)
ENDIF( USE_FFTIMAGE )

