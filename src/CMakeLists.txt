CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# This project is intended to be built outside the Insight source tree
PROJECT(praxix)

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
 computedistances
 computemean
 computeoverlap
 contrastenhanceimage
 countnonzerovoxels
 createbox
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
 dilateimage
 distancetransform
 erodeimage
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
 morphologicalclosingimage
 morphologicalopeningimage
 morphology
 pca
 reflect
 replacevoxel
 rescaleintensityimagefilter
 #reshape
 resizeimage
 scripts
 segmentationdistance
 statisticsonimage
 texture
 tileimages
 ttest
 thresholdimage
 unaryimageoperator)


#---------------------------------------------------------------------
# Output directories.

SET (LIBRARY_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(praxix_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(praxix_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")
INCLUDE_DIRECTORIES(${praxix_SOURCE_DIR}/common)


