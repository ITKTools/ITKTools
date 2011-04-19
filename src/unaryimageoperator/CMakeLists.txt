# This project is intended to be built outside the Insight source tree
PROJECT( unaryimageoperator )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxunaryimageoperator
 unaryimageoperator.cxx
 unaryimageoperatorScalar.cxx
 unaryimageoperatorScalar2DA.cxx
 unaryimageoperatorScalar2DB.cxx
 unaryimageoperatorScalar3DA.cxx
 unaryimageoperatorScalar3DB.cxx
 itkUnaryFunctors.h
 UnaryImageOperatorHelper.h
 UnaryImageOperatorMainHelper.h )

TARGET_LINK_LIBRARIES( pxunaryimageoperator pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxunaryimageoperator
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

