# This project is intended to be built outside the Insight source tree
PROJECT(logicalimageoperator)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxlogicalimageoperator
  logicalimageoperator.cxx
  itkLogicalFunctors.h
  LogicalImageOperatorHelper.h )

TARGET_LINK_LIBRARIES( pxlogicalimageoperator pxcommon ${ITK_LIBRARIES})

INSTALL( TARGETS pxlogicalimageoperator
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

