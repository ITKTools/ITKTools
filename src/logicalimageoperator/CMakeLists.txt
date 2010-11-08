# This project is intended to be built outside the Insight source tree
PROJECT(logicalimageoperator)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE( pxlogicalimageoperator
  logicalimageoperator.cxx
  itkLogicalFunctors.h
  LogicalImageOperatorHelper.h )

TARGET_LINK_LIBRARIES( pxlogicalimageoperator pxcommon ITKBasicFilters ITKIO ITKCommon )

INSTALL( TARGETS pxlogicalimageoperator
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

