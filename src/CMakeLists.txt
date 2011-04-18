CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

# This project is intended to be built outside the Insight source tree
PROJECT( praxix )

# Define where to install praxix
IF( WIN32 )
  SET( PRAXIX_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
ELSE()
  SET( PRAXIX_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
ENDIF()

# Include all subdirs
SUBDIRS(
 averagevectormagnitude
 binaryimageoperator
 binarythinning
 braindistance
 castconvert
 closestversor3Dtransform
 combinesegmentations
 common
 computeboundingbox
 computedifferenceimageBIG
 computemean
 
 contrastenhanceimage
 countnonzerovoxels
 createbox
 createcylinder
 createellipsoid
 creategridimage
 # createimage
 createpointsinimage
 #createrandomimage
 createsimplebox
 createsphere
 createzeroimage
 cropimage
 deformationfieldgenerator
 deformationfieldoperator
 detectgoldmarkers
 distancetransform
 extracteveryotherslice
 extractindexfromvectorimage
 extractslice
 fftimage
 gaussianimagefilter
 getdicominformation
 getDICOMseriesUIDs
 getimageinformation
 getpointsinimage
 giplconvert
 histogramequalizeimage
 imagecompare
 imagetovectorimage
 intensityreplace
 intensitywindowing
 invertintensityimagefilter
 kappastatistic
 logicalimageoperator
 #localizedmeanshift
 morphology
 naryimageoperator
 pca
 reflect
 replacevoxel
 rescaleintensityimagefilter
 reshape
 resizeimage
 scripts
 segmentationdistance
 statisticsonimage
 
 
 ttest
 thresholdimage
 unaryimageoperator
 weightedaddition )

SET(PROBLEMS OFF CACHE BOOL "Compile things with problems." FORCE)
if(problems)
  SUBDIRS(
  tileimages
  texture
  computeoverlap)
endif(problems)

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

SET (LIBRARY_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(praxix_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(praxix_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")
INCLUDE_DIRECTORIES(${praxix_SOURCE_DIR}/common)

