# This project is intended to be built outside the Insight source tree
PROJECT(praxix)

SUBDIRS(
 averagevectormagnitude
 castconvert
 closestversor3Dtransform
 common
 computeadditionimage
 computeboundingbox
 computedifferenceimage
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
 erodeimage
 extracteveryotherslice
 extractindexfromvectorimage
 extractslice
 getdicominformation
 getDICOMseriesUIDs
 getimageinformation
 getpointsinimage
 giplconvert
 histogramequalizeimage
 imagecompare
 intensitywindowing
 invertintensityimagefilter
 logicalimageoperator
 maximumimagefilter
 minimumimagefilter
 morphologicalclosingimage
 morphologicalopeningimage
 multiplyimages
 rescaleintensityimagefilter
 resizeimage
 scripts
 segmentationdistance
 statisticsonimage
 tileimages
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


