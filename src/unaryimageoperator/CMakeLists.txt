# This project is intended to be built outside the Insight source tree
PROJECT( unaryimageoperator )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxunaryimageoperator
 unaryimageoperator.cxx
 itkUnaryFunctors.h
 UnaryImageOperatorHelper.h
 UnaryImageOperatorMainHelper.h )

TARGET_LINK_LIBRARIES( pxunaryimageoperator ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxunaryimageoperator
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

