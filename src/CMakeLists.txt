cmake_minimum_required( VERSION 2.6 )

# This project is intended to be built outside the Insight source tree
project( ITKTools )

#---------------------------------------------------------------------
# Find ITK.
FIND_PACKAGE( ITK 4 REQUIRED )

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
# Kill the anoying MS VS warning about non-safe functions.
# They hide real warnings.
if( MSVC )
  add_definitions(
    /D_SCL_SECURE_NO_DEPRECATE
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
    )
endif( MSVC )

#---------------------------------------------------------------------
# Output directories.
set( LIBRARY_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all libraries." )
set( EXECUTABLE_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all executables." )
mark_as_advanced( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )
set( ITKTools_LIBRARY_PATH    "${LIBRARY_OUTPUT_PATH}" )
set( ITKTools_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}" )
include_directories( ${ITKTools_SOURCE_DIR}/common )

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
  set( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
else()
  set( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
endif()

#---------------------------------------------------------------------
# Testing
set( ITKTOOLS_BUILD_TESTING OFF CACHE BOOL
  "Perform some tests on basic functionality of ITKTools." )
if( ITKTOOLS_BUILD_TESTING )
 enable_testing()
 add_subdirectory( ${ITKTools_SOURCE_DIR}/../Testing ${ITKTools_BINARY_DIR}/Testing )
 include( CTest )
endif()

#---------------------------------------------------------------------
# Documentation
set( ITKTOOLS_BUILD_DOCUMENTATION OFF CACHE BOOL
  "Create the documentation of ITKTools." )
if( ITKTOOLS_BUILD_DOCUMENTATION)
 add_subdirectory( ${ITKTools_SOURCE_DIR}/../Documentation ${ITKTools_BINARY_DIR}/Documentation )
endif()

#---------------------------------------------------------------------
# Macro to simplify the addition of an ITKTool
macro( ADD_ITKTOOL name )
  # Define the project
  project( name )

  # Increases address capacity
  if ( WIN32 )
    set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
  endif()

  # Include ITK headers.
  include( ${ITK_USE_FILE} )

  # Get the list of files in this directory
  file( GLOB filelist RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * )
  list( REMOVE_ITEM filelist "CMakeLists.txt" )

  # Create the executable
  add_executable( px${name} ${filelist} )

  # Link
  target_link_libraries( px${name} ITKTools-Common ${ITK_LIBRARIES} )

  # Install
  install( TARGETS px${name}
    RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )
endmacro()

#---------------------------------------------------------------------
# Include all ITKTools
file( GLOB listOfTools RELATIVE ${CMAKE_SOURCE_DIR} "*/CMakeLists.txt" )
foreach( tool ${listOfTools} )
  get_filename_component( path_to_tool ${tool} PATH )
  add_subdirectory( ${path_to_tool} )
endforeach()

