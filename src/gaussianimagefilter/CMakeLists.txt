# This project is intended to be built outside the Insight source tree
PROJECT( gaussianimagefilter )

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxgaussianimagefilter
 gaussianimagefilter.cxx
 itkSmoothingRecursiveGaussianImageFilter2.h
 itkSmoothingRecursiveGaussianImageFilter2.txx
 itkHessianRecursiveGaussianImageFilter2.h
 itkHessianRecursiveGaussianImageFilter2.txx
 itkGaussianInvariantsImageFilter.h
 itkGaussianInvariantsImageFilter.txx
 )

TARGET_LINK_LIBRARIES( pxgaussianimagefilter ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxgaussianimagefilter
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

