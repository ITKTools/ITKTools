#
# To use itktools-code in your own program, use the following
# cmake code:
#
# cmake_minimum_required(VERSION 2.8)
#
# # You may omit this if you use the ADD_ITKTOOL macro:
# project( itktoolsexternalprog )
#
# FIND_PACKAGE( ITK 4 REQUIRED )
# IF( EXISTS ${ITK_USE_FILE} )
# include( ${ITK_USE_FILE} )
# ENDIF()
#
# FIND_PACKAGE( ITKTOOLS REQUIRED )
# IF( EXISTS ${ITKTOOLS_USE_FILE} )
#  INCLUDE( ${ITKTOOLS_USE_FILE} )
# ENDIF()
#
# # Optionally, you could use the ADD_ITKTOOL macro:
# # Then your program yourownprogram.cxx will be automatically
# # linked to the correct libraries in most cases, and it will be
# # installed in the same directory as the other pxtools.
# # Make sure to have one file yourownprogram.cxx in the 
# # current directory, which defines a main() function. All other
# # files in the current directory are also added as source code for 
# # this executable.
# ADD_ITKTOOL( yourownprogram )
#
# # # Or do something like this:
# # ADD_EXECUTABLE( yourownprogram yourownprogram.cxx )
# # TARGET_LINK_LIBRARIES( yourownprogram
# #  ${ITKTOOLS_LIBRARIES} ${ITK_LIBRARIES} )
# 

MESSAGE( STATUS "Including ITKTOOLS settings.")

#---------------------------------------------------------------------
# Set all necessary include directories for itktools
INCLUDE_DIRECTORIES( ${ITKTOOLS_INCLUDE_DIRECTORIES} )

# Set the directory that contains the itktools libraries,
LINK_DIRECTORIES( "${ITKTOOLS_LIBRARY_OUTPUT_PATH}" )

# Read in the library dependencies
INCLUDE( "${ITKTOOLS_LIBRARY_DEPENDS_FILE}" )

#---------------------------------------------------------------------
# Increases address capacity
if ( WIN32 )
  set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
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
endif()

#---------------------------------------------------------------------
# Include the macro definitions
# to use file:
INCLUDE( ${ITKTOOLS_MACRO_DEFINITIONS} )


