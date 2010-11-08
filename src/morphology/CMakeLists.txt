# This project is intended to be built outside the Insight source tree
PROJECT(morphology)

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

# pxmorphology depends on some ITK settings
#IF( NOT ITK_USE_CONSOLIDATED_MORPHOLOGY )
#  message( SEND_ERROR "ITK_USE_CONSOLIDATED_MORPHOLOGY has not been set to ON when compiling ITK. This option is mandatory for pxmorphology to build.\nPlease recompile your ITK installation." )
#ENDIF()

ADD_EXECUTABLE(pxmorphology
  morphology.cxx
  morphology2D.cxx
  morphology3D.cxx
  mainhelper1.h
  mainhelper2.h
  erosion.h
  dilation.h
	opening.h
	closing.h
	gradient.h
)

TARGET_LINK_LIBRARIES(pxmorphology pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxmorphology
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

