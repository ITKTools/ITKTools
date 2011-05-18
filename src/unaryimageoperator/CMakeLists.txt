# This project is intended to be built outside the Insight source tree
PROJECT( unaryimageoperator )

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
ENDIF ( WIN32 )

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

