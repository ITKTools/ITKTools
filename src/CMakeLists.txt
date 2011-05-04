CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

# This project is intended to be built outside the Insight source tree
PROJECT( ITKTools )
ENABLE_TESTING()

# Define where to install ITKTools
IF( WIN32 )
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
ELSE()
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
ENDIF()

# Include all subdirs
file( GLOB listOfTools RELATIVE ${CMAKE_SOURCE_DIR} "*/CMakeLists.txt" )
foreach( tool ${listOfTools} )
  get_filename_component( path_to_tool ${tool} PATH )
  subdirs( ${path_to_tool} )
  #add_subdirectory( ${path_to_tool} )
endforeach()

#---------------------------------------------------------------------
# Kill the anoying MS VS warning about non-safe functions.
# They hide real warnings.
IF( MSVC )
  ADD_DEFINITIONS(
    /D_SCL_SECURE_NO_DEPRECATE
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
    )
ENDIF( MSVC )

#---------------------------------------------------------------------
# Output directories.

SET( LIBRARY_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all libraries." )
SET( EXECUTABLE_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all executables." )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )
SET( ITKTools_LIBRARY_PATH    "${LIBRARY_OUTPUT_PATH}" )
SET( ITKTools_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}" )
INCLUDE_DIRECTORIES( ${ITKTools_SOURCE_DIR}/common )

#---------------------------------------------------------------------
# Testing

SET( ITKTOOLS_BUILD_TESTING OFF CACHE BOOL
  "Perform some tests on basic functionality of ITKTools." )
IF( ITKTOOLS_BUILD_TESTING )
 ENABLE_TESTING()
 ADD_SUBDIRECTORY( ${ITKTools_SOURCE_DIR}/../Testing ${ITKTools_BINARY_DIR}/Testing )
 INCLUDE( CTest )
ENDIF()

SET( ITKTOOLS_BUILD_DOCUMENTATION OFF CACHE BOOL
  "Create the documentation of ITKTools." )
IF( ITKTOOLS_BUILD_DOCUMENTATION)
 ENABLE_TESTING()
 ADD_SUBDIRECTORY( ${ITKTools_SOURCE_DIR}/../Documentation ${ITKTools_BINARY_DIR}/Documentation )
 INCLUDE( CTest )
ENDIF()