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
# Setup: linux 64bit
# gcc 4.4.6 (RedHat Linux),
# Release mode, ITK 4.1.0
# PC: linux cluster2 at BIGR (SK).

# Client maintainer: s.klein@erasmusmc.nl
set( CTEST_SITE "BIGR.cluster" )
set( CTEST_BUILD_NAME "Linux-64bit-gcc4.4.6" )
set( CTEST_BUILD_FLAGS "-j3" ) # parallel build for makefiles
set( CTEST_TEST_ARGS PARALLEL_LEVEL 3 ) # parallel testing
set( CTEST_BUILD_CONFIGURATION Release )
set( CTEST_CMAKE_GENERATOR "Unix Makefiles" )
set( CTEST_DASHBOARD_ROOT "/cm/shared/apps/itktools/nightly" )

# Overrule binary directory. Use 'release' instead of 'bin',
# for consistency with other itktools verisons on the cluster2
set( CTEST_BINARY_DIRECTORY ${CTEST_DASHBOARD_ROOT}/release )

# default: automatically determined
#set(CTEST_UPDATE_COMMAND /path/to/svn)

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
ITK_DIR:PATH=/cm/shared/apps/itk/4.2.0/releasefftw

// Some ITKTools settings, defining the configuration
ITKTOOLS_BUILD_TESTING:BOOL=ON
ITKTOOLS_3D_SUPPORT:BOOL=ON
ITKTOOLS_4D_SUPPORT:BOOL=ON
ITKTOOLS_USE_MEVISDICOMTIFF:BOOL=ON
USE_FFTIMAGE:BOOL=ON

// Install dir
CMAKE_INSTALL_PREFIX:PATH=/cm/shared/apps/itktools/nightly/install
")


# Load the common dashboard script.
include( ${CTEST_SCRIPT_DIRECTORY}/pxDashboardCommon.cmake )

