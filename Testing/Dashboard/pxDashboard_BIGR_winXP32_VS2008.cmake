# ITKTools Dashboard Script
#
# This script runs a dashboard
# Usage:
#   ctest -S <nameofthisscript> -V
#   OR
#   ctest -S <nameofthisscript>,Model -V
#
# It has 1 optional argument: the build model.
# The build model should be one of {Experimental, Continuous, Nightly}
# and defaults to Nightly.
# NOTE that Model should directly follow the comma: no space allowed!
#
# Setup: Windows XP, 32bit, Visual Studio 9 2008, Release mode, ITK 4.1.0
# PC: BIGR, SK personal computer.

# Client maintainer: s.klein@erasmusmc.nl
set( CTEST_SITE "BIGR.PCStefan" )
set( CTEST_BUILD_NAME "WinXP-32bit-VS2008" )
#set( CTEST_BUILD_FLAGS "-j2" ) # parallel build for makefiles
set( CTEST_BUILD_CONFIGURATION Release )
set( CTEST_CMAKE_GENERATOR "Visual Studio 9 2008" )
set( CTEST_DASHBOARD_ROOT "D:/tk/mydash/${CTEST_SCRIPT_NAME}" )

# Specify the kind of dashboard to submit
# default: Nightly
SET( dashboard_model Nightly )
IF( ${CTEST_SCRIPT_ARG} MATCHES Experimental )
  SET( dashboard_model Experimental )
ELSEIF( ${CTEST_SCRIPT_ARG} MATCHES Continuous )
  SET( dashboard_model Continuous )
ENDIF()

#set(dashboard_do_memcheck 1)
#set(dashboard_do_coverage 1)

SET( dashboard_cache "
// Which ITK to use
ITK_DIR:PATH=D:/tk/itk/4.1.0/bin

// Some ITKTools settings, defining the configuration
ITKTOOLS_BUILD_TESTING:BOOL=ON
ITKTOOLS_3D_SUPPORT:BOOL=ON
ITKTOOLS_4D_SUPPORT:BOOL=OFF
ITKTOOLS_USE_MEVISDICOMTIFF:BOOL=ON

")


# Load the common dashboard script.
include( ${CTEST_SCRIPT_DIRECTORY}/pxDashboardCommon.cmake )

