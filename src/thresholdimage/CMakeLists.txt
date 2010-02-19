# This project is intended to be built outside the Insight source tree
PROJECT(thresholdimage)

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

ADD_EXECUTABLE(pxthresholdimage thresholdimage.cxx )

INSTALL_TARGETS(/. pxthresholdimage)

TARGET_LINK_LIBRARIES(pxthresholdimage pxcommon ITKBasicFilters ITKIO ITKCommon ITKStatistics)

