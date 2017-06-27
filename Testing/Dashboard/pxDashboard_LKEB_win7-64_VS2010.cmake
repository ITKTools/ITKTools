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
# Setup: Windows 7, Visual Studio 10 2010 Win64, Release mode, ITK latest release
# PC: LKEB, MS personal computer

# Client maintainer: m.staring@lumc.nl
set( CTEST_SITE "LKEB.PCMarius" )
set( CTEST_BUILD_NAME "Win7-64bit-VS2010" )
#set( CTEST_BUILD_FLAGS "-j2" ) # parallel build for makefiles
set( CTEST_BUILD_CONFIGURATION Release )
set( CTEST_CMAKE_GENERATOR "Visual Studio 10 2010 Win64" )
set( CTEST_DASHBOARD_ROOT "D:/toolkits/ITKTools/nightly" )

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
ITK_DIR:PATH=D:/toolkits/ITK/latest_release/bin_VS2010

// Some ITKTools settings, defining the configuration
ITKTOOLS_BUILD_TESTING:BOOL=ON
ITKTOOLS_3D_SUPPORT:BOOL=ON
ITKTOOLS_4D_SUPPORT:BOOL=OFF
")


# Load the common dashboard script.
include( ${CTEST_SCRIPT_DIRECTORY}/pxDashboardCommon.cmake )

