CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

# This project is intended to be built outside the Insight source tree
PROJECT( ITKTools )

# Define where to install ITKTools
IF( WIN32 )
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
ELSE()
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
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
 computeoverlap
 contrastenhanceimage
 countnonzerovoxels
 createbox
 createcylinder
 createellipsoid
 creategridimage
 createpointsinimage
 createrandomimage
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
 texture
 tileimages # "could not convert template argument" gcc4.6 bug
 ttest
 thresholdimage
 unaryimageoperator
 weightedaddition )


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

