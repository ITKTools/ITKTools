# This project is intended to be built outside the Insight source tree
PROJECT(closestversor3Dtransform)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxclosestversor3Dtransform closestversor3Dtransform.cxx)

TARGET_LINK_LIBRARIES(pxclosestversor3Dtransform pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxclosestversor3Dtransform
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

