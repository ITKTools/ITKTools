# This project is intended to be built outside the Insight source tree
PROJECT(gaussianimagefilter)

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxgaussianimagefilter
 gaussianimagefilter.cxx
 itkSmoothingRecursiveGaussianImageFilter2.h
 itkSmoothingRecursiveGaussianImageFilter2.txx
 itkHessianRecursiveGaussianImageFilter2.h
 itkHessianRecursiveGaussianImageFilter2.txx
 itkGaussianInvariantsImageFilter.h
 itkGaussianInvariantsImageFilter.txx
)

TARGET_LINK_LIBRARIES(pxgaussianimagefilter pxcommon ITKIO ITKBasicFilters ITKCommon)

INSTALL( TARGETS pxgaussianimagefilter
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

