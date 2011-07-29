# This project is intended to be built outside the Insight source tree
PROJECT( logicalimageoperator )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxlogicalimageoperator
  logicalimageoperator.cxx
  itkUnaryLogicalFunctors.h
  itkBinaryLogicalFunctors.h
  LogicalImageOperatorHelper.h )

TARGET_LINK_LIBRARIES( pxlogicalimageoperator ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxlogicalimageoperator
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

