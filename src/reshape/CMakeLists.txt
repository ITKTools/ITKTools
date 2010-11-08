# This project is intended to be built outside the Insight source tree
PROJECT(reshape)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxreshape
 reshape.cxx
 itkReshapeImageToImageFilter.h
 itkReshapeImageToImageFilter.txx )

TARGET_LINK_LIBRARIES(pxreshape pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxreshape
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

