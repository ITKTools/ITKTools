# Require at least ITK_OLDEST_VALIDATED_POLICIES_VERSION from
# https://github.com/InsightSoftwareConsortium/ITK/blob/v5.0.0/CMakeLists.txt#L13 
cmake_minimum_required( VERSION 3.10.2 )

# This project is intended to be built outside the Insight source tree
project( ITKTOOLS )

#---------------------------------------------------------------------
# Find ITK.
FIND_PACKAGE( ITK 5 REQUIRED )
include( ${ITK_USE_FILE} )

# ITKTools depends on some ITK modules. Check for them:
# NOTE: not sure if this is the best way to check for this,
# but for now it works.
#set( listofrequiredcomponents ITKCommon )
#foreach( item ${listofrequiredcomponents} )
#  # Find the item in the list of enabled modules
#  list( FIND ITK_MODULES_ENABLED ${item} found )
#
#  # Notify the user and fail gracefully.
#  if( found EQUAL -1 )
#    message( SEND_ERROR "The module "${item}" has not been set to ON when compiling ITK. This option is mandatory for ITKTools to build.\nPlease recompile your ITK installation." )
#  else()
#    message( STATUS "Found required ITK module "${item} )
#  endif()
#endforeach()
#message( STATUS "Found all ITK modules required to build ITKTools!" )

# Check if ITK was compiled with ITKIOPhilipsREC
list( FIND ITK_MODULES_ENABLED "ITKIOPhilipsREC" found )
if( NOT found EQUAL -1 )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DITKTOOLS_ITKIOPhilipsREC_Found" )
endif()

#---------------------------------------------------------------------
# Add Mevis Dicom/Tiff support 
MARK_AS_ADVANCED( ITKTOOLS_USE_MEVISDICOMTIFF )
OPTION( ITKTOOLS_USE_MEVISDICOMTIFF
  "Support MevisLab DicomTiff image format" OFF )

#---------------------------------------------------------------------
# Kill the anoying MS VS warning about non-safe functions.
# They hide real warnings.
if( MSVC )
  add_definitions(
    /D_SCL_SECURE_NO_DEPRECATE
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
    )
endif()

#---------------------------------------------------------------------
# Increases address capacity
if( WIN32 )
  if ( NOT MINGW )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
  endif()
endif()

#---------------------------------------------------------------------
# Output directories.
set( LIBRARY_OUTPUT_PATH ${ITKTOOLS_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all libraries." )
set( EXECUTABLE_OUTPUT_PATH ${ITKTOOLS_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all executables." )
mark_as_advanced( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )
set( ITKTOOLS_LIBRARY_PATH    "${LIBRARY_OUTPUT_PATH}" )
set( ITKTOOLS_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}" )

#---------------------------------------------------------------------
# Include directories
set( ITKTOOLS_INCLUDE_DIRECTORIES 
  "${ITKTOOLS_SOURCE_DIR}/common" 
  "${ITKTOOLS_SOURCE_DIR}/common/MevisDicomTiff" )
include_directories( ${ITKTOOLS_INCLUDE_DIRECTORIES} )

#---------------------------------------------------------------------
# Link libraries
SET( ITKTOOLS_LIBRARIES ITKTools-Common mevisdcmtiff)

#---------------------------------------------------------------------
# Compilation options
set( ITKTOOLS_3D_SUPPORT ON CACHE BOOL
  "Support operations on 3D Images." )
if( ITKTOOLS_3D_SUPPORT )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DITKTOOLS_3D_SUPPORT" )
endif()

set( ITKTOOLS_4D_SUPPORT OFF CACHE BOOL
  "Support operations on 4D Images." )
if( ITKTOOLS_4D_SUPPORT )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DITKTOOLS_4D_SUPPORT" )
endif()

#---------------------------------------------------------------------
# Define where to install ITKTools
if( WIN32 )
  set( ITKTOOLS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
else()
  set( ITKTOOLS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
endif()

#---------------------------------------------------------------------
# Testing
set( ITKTOOLS_BUILD_TESTING OFF CACHE BOOL
  "Perform some tests on basic functionality of ITKTools." )
if( ITKTOOLS_BUILD_TESTING )
 enable_testing()
 add_subdirectory( ${ITKTOOLS_SOURCE_DIR}/../Testing ${ITKTOOLS_BINARY_DIR}/Testing )
 include( CTest )
endif()

#---------------------------------------------------------------------
# Documentation
set( ITKTOOLS_BUILD_DOCUMENTATION OFF CACHE BOOL
  "Create the documentation of ITKTools." )
if( ITKTOOLS_BUILD_DOCUMENTATION)
 add_subdirectory( ${ITKTOOLS_SOURCE_DIR}/../Documentation ${ITKTOOLS_BINARY_DIR}/Documentation )
endif()

#---------------------------------------------------------------------
# Include macros
SET( ITKTOOLS_MACRO_DEFINITIONS ${ITKTOOLS_SOURCE_DIR}/CMakeMacros.cmake)
INCLUDE( ${ITKTOOLS_MACRO_DEFINITIONS} )

#---------------------------------------------------------------------
# Include all ITKTools
file( GLOB listOfTools RELATIVE ${CMAKE_SOURCE_DIR} "*/CMakeLists.txt" )
foreach( tool ${listOfTools} )
  get_filename_component( path_to_tool ${tool} PATH )
  add_subdirectory( ${path_to_tool} )
endforeach()

#----------------------------------------------------------------------
# Make it easier to include itktools functionality in other programs.
# See UseFile.cmake for instructions.

# Save library dependencies.
SET( ITKTOOLS_LIBRARY_DEPENDS_FILE ${ITKTOOLS_BINARY_DIR}/ITKTOOLSLibraries.cmake )
EXPORT( TARGETS ${ITKTOOLS_LIBRARIES} FILE ${ITKTOOLS_LIBRARY_DEPENDS_FILE} )

# The "use" file.
SET( ITKTOOLS_USE_FILE ${ITKTOOLS_SOURCE_DIR}/UseFile.cmake )

# Create the config file. It defines some variables, and by placing
# this in the binary directory, the find_package command will recognise
# the ITKTools package.
CONFIGURE_FILE( ${ITKTOOLS_SOURCE_DIR}/ITKTOOLSConfig.cmake.in
 ${ITKTOOLS_BINARY_DIR}/ITKTOOLSConfig.cmake @ONLY )

# Store the current build directory in the CMake user package registry
# for package <name>. The find_package command may consider the director
# while searching for package <name>.
export( PACKAGE ITKTOOLS )

